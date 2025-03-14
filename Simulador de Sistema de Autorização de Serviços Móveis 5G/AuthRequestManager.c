// Authors: Luis Vaz 2022214707 Henrique Oliveira 2022211169
#include <stdio.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/select.h>
#include <pthread.h>
#include <semaphore.h>
#define VIDEO_ID_STR "345"
#define BUFFER_SIZE 256
volatile sig_atomic_t running = 1;

// Define the structure for a queue node
typedef struct Node {
    char *request;
    struct Node *next;
} Node;

// Define the structure for a queue
typedef struct Queue {
    Node *front;
    Node *rear;
    sem_t sem;                // Semaphore to control access
} Queue;

typedef struct {
    int mobile_users;
    int queue_pos;
    int auth_servers;
    int auth_proc_time;
    int max_video_wait;
    int max_others_wait;
} ConfigData;

typedef struct {
    pid_t pid;
    int pipe_fd[2];
    bool status;  // true for READY, false for BUSY
} EngineInfo;

typedef struct {
    Queue *videoQueue;
    Queue *otherServicesQueue;
    ConfigData config;
} ThreadArgs;

//Global Variables
Queue *VideoQueue;
Queue *OtherServicesQueue;
pthread_cond_t queues_not_empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t receiver, sender, extra_engine;
EngineInfo* engines = NULL;
int num_engines = 0; 

ConfigData deserialize_struct(char* serialized) {
    ConfigData data;
    sscanf(serialized, "%d,%d,%d,%d,%d,%d", &data.mobile_users, &data.queue_pos, &data.auth_servers, &data.auth_proc_time, &data.max_video_wait, &data.max_others_wait);
    return data;
}
void write_log(const char *filename, const char *message) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening log file");
        return;
    }

    time_t now = time(NULL);
    char *time_str = ctime(&now);
    time_str[strlen(time_str) - 1] = '\0';  // Remove newline character

    fprintf(file, "[%s] %s\n", time_str, message);

    fclose(file);
}

void send_message_to_engine(const char* message, ConfigData config) {
    for(int i = 0; i < num_engines; i++) {
        if(engines[i].status == true) {
            write(engines[i].pipe_fd[1], message, strlen(message) + 1);
            printf("Sent message to engine %d (%d): %s\n", i,engines[i].pid, message);
            engines[i].status = false;  // Set status to BUSY
            sleep(config.auth_proc_time/1000); //Process the request
            engines[i].status = true; // Set status to READY
            return;
        }
    } 
    // If no engine is available, drop the request
    printf("All engines are busy, dropping request: %s\n", message);
}

void create_AuthEngine() {
    // Increase the number of engines
    num_engines++;
      // Reallocate memory for the engines array
    engines = realloc(engines, num_engines * sizeof(EngineInfo));
    if (engines == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

     // Create a pipe for communication
    if (pipe(engines[num_engines - 1].pipe_fd) == -1) {
        perror("Pipe creation failed");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {  // Child process (Authengine)
        close(engines[num_engines - 1].pipe_fd[1]);  // Close write end of the pipe
        dup2(engines[num_engines - 1].pipe_fd[0], STDIN_FILENO);  // Redirect stdin to read end of the pipe
        close(engines[num_engines - 1].pipe_fd[0]);  // Close read end after duplication

        execl("./AuthEngine", "AuthEngine", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } else {  // Parent process  (AuthRequestManager)
        close(engines[num_engines - 1].pipe_fd[0]);  // Close read end of the pipe
        engines[num_engines - 1].pid = pid;
        engines[num_engines - 1].status = true;  // Set status to READY
        
    }
}
void remove_AuthEngine() {
    if (num_engines == 0) {
        fprintf(stderr, "No engines to remove\n");
        return;
    }

    // Send termination signal to the engine process
    if (kill(engines[0].pid, SIGTERM) == -1) {
        perror("Failed to send SIGTERM");
    }

    // Wait for the process to terminate
    waitpid(engines[0].pid, NULL, 0);

    // Close the pipe file descriptors
    close(engines[0].pipe_fd[0]);
    close(engines[0].pipe_fd[1]);

    // Shift remaining engines in the array
    for (int i = 1; i < num_engines; i++) {
        engines[i - 1] = engines[i];
    }

    // Decrease the number of engines
    num_engines--;

    // Reallocate the array
    engines = realloc(engines, num_engines * sizeof(EngineInfo));
    if (engines == NULL && num_engines > 0) {
        perror("Realloc failed");
        exit(EXIT_FAILURE);
    }
}
void print_engine_info() {
    printf("Engine Information:\n");
    for (int i = 0; i < num_engines; i++) {
        printf("Engine %d:\n", i + 1);
        printf("  PID: %d\n", engines[i].pid);
        printf("  Pipe FD: %d, %d\n", engines[i].pipe_fd[0], engines[i].pipe_fd[1]);
        printf("  Status: %s\n", engines[i].status ? "READY" : "BUSY");
    }
}
//QueueManagment Functions
Queue* createQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = q->rear = NULL;
    sem_init(&(q->sem), 0, 1);            // Initialize the semaphore
    return q;
}
void enqueue(Queue *q, const char *request) {
    sem_wait(&(q->sem));  // Wait (lock) the semaphore

    // Create a new node
    Node *temp = (Node *)malloc(sizeof(Node));
    temp->request = strdup(request);
    temp->next = NULL;

    // If the queue is empty, new node is front and rear
    if (q->rear == NULL) {
        q->front = q->rear = temp;
    } else {
        // Add the new node at the end of the queue and change rear
        q->rear->next = temp;
        q->rear = temp;
    }
    //Signal the condition variable that the queue is not empty
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&queues_not_empty);
    pthread_mutex_unlock(&mutex);
    sem_post(&(q->sem));  // Signal (unlock) the semaphore
}
char* dequeue(Queue *q) {
    // Lock the semaphore before modifying the queue
    sem_wait(&(q->sem));

    // Store previous front and move front one node ahead
    Node *temp = q->front;
    char *request = strdup(temp->request);

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    // Free the memory of the dequeued node
    free(temp->request);
    free(temp);

    sem_post(&(q->sem));  // Signal (unlock) the semaphore
    return request;
}
void displayQueue(Queue *q) {
    sem_wait(&(q->sem));  // Wait (lock) the semaphore

    Node *temp = q->front;
    printf("Queue: ");
    while (temp != NULL) {
        printf("%s -> ", temp->request);
        temp = temp->next;
    }
    printf("NULL\n");

    sem_post(&(q->sem));  // Signal (unlock) the semaphore
}
int countQueue(Queue *q) {
    sem_wait(&(q->sem));  // Wait (lock) the semaphore

    int count = 0;
    Node *temp = q->front;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }

    sem_post(&(q->sem));  // Signal (unlock) the semaphore
    return count;
}

int check_if_video(char str[]) {
    char *token;
    char copy[BUFFER_SIZE];
    char *words[3];
    int i = 0;

    // Make a copy of the received string
    strcpy(copy, str);

    // Tokenize the string using # as a delimiter
    token = strtok(copy, "#");
    while (token != NULL && i < 3) {
        words[i++] = token;
        token = strtok(NULL, "#");
    }

    // Ensure there are exactly three words
    if (i != 3) {
        return 1;
    }

    // Check if the second word is equal to VIDEO_ID_STR
    if (strcmp(words[1], VIDEO_ID_STR) == 0) {
        return 0;
    } else {
        return 1;
    }
}

void* checkOccupancy(void* arg) {
    printf("CheckOccupancy thread started\n");
    ThreadArgs *args = (ThreadArgs *)arg;
    Queue *VideoQueue = args->videoQueue;
    Queue *OtherServicesQueue = args->otherServicesQueue;
    ConfigData config = args->config;

    int flag_extra_engine = 0;
    int queue_max_size = config.queue_pos;
    while (running) {
        sleep(30);
        float VideoQueuePercentage = (float)(countQueue(VideoQueue) / queue_max_size) * 100;
        float OtherServicesQueuePercentage = (float)(countQueue(OtherServicesQueue) / queue_max_size) * 100;

        if (flag_extra_engine == 1 && (VideoQueuePercentage < 50 || OtherServicesQueuePercentage < 50)) {
            remove_AuthEngine();
            printf("AuthRequestManager: Removed Extra engine\n");
            flag_extra_engine = 0;
        }
        if (flag_extra_engine == 0 && (VideoQueuePercentage == 100 || OtherServicesQueuePercentage == 100)) {
            create_AuthEngine();
            printf("AuthRequestManager: Created Extra engine\n");
            flag_extra_engine = 1;
        }
    }
    printf("Exiting checkOccupancy thread...\n");
    return NULL;
}

void *receiver_thread(void *arg) {
    int user_pipe_fd, back_pipe_fd;
    char buffer[BUFFER_SIZE];
    fd_set read_fds;
    int max_fd;
    printf("Reciver thread started\n");
    write_log("log.txt", "RECIVER THREAD STARTED");
    // Open USER_PIPE for reading
    if ((user_pipe_fd = open("USER_PIPE", O_RDONLY | O_NONBLOCK)) == -1) {
        perror("Error opening USER_PIPE");
        pthread_exit(NULL);
    }

    // Open BACK_PIPE for reading
    if ((back_pipe_fd = open("BACK_PIPE", O_RDONLY | O_NONBLOCK)) == -1) {
        perror("Error opening BACK_PIPE");
        close(user_pipe_fd);
        pthread_exit(NULL);
    }

    max_fd = (user_pipe_fd > back_pipe_fd) ? user_pipe_fd : back_pipe_fd;

    while (running) {
        FD_ZERO(&read_fds);
        FD_SET(user_pipe_fd, &read_fds);
        FD_SET(back_pipe_fd, &read_fds);

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            if (running) {
                perror("Error in select");
                close(user_pipe_fd);
                close(back_pipe_fd);
                pthread_exit(NULL);
            }
            break; // Exit the loop if running is set to 0
        }

        if (FD_ISSET(user_pipe_fd, &read_fds)) {
            ssize_t bytes_read = read(user_pipe_fd, buffer, BUFFER_SIZE - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("Received from USER_PIPE: %s\n", buffer);
                int result = check_if_video(buffer);
                //printf("Result: %d\n", result);
                if (result == 0) {
                    enqueue(VideoQueue, buffer);
                    displayQueue(VideoQueue);
                } else {
                    enqueue(OtherServicesQueue, buffer);
                    displayQueue(OtherServicesQueue);
                }
            }
        }

        if (FD_ISSET(back_pipe_fd, &read_fds)) {
            ssize_t bytes_read = read(back_pipe_fd, buffer, BUFFER_SIZE - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("Received from BACK_PIPE: %s\n", buffer);
                enqueue(OtherServicesQueue, buffer);
                displayQueue(OtherServicesQueue);

            }
        }
    }

    close(user_pipe_fd);
    close(back_pipe_fd);
    printf("Exiting receiver thread...\n");
    return NULL;
}

void *sender_thread(void *arg) {
    ConfigData *config = (ConfigData *)arg;
    printf("Sender thread started\n");
    write_log("log.txt", "SENDER THREAD STARTED");
    while (running) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&queues_not_empty, &mutex);
        pthread_mutex_unlock(&mutex);

        if (VideoQueue->front != NULL) {
            char *request = dequeue(VideoQueue);
            printf("Dequeued request from VideoQueue: %s\n", request);
            displayQueue(VideoQueue);
            send_message_to_engine(request,*config);
            displayQueue(VideoQueue);
            free(request);
        }
        //If VideoQueue is empty, check OtherServicesQueue
        else if (OtherServicesQueue->front != NULL) {
                char *request = dequeue(OtherServicesQueue);
                printf("Dequeued request from OtherServices: %s\n", request);
                displayQueue(OtherServicesQueue);
                send_message_to_engine(request,*config);
                free(request);
        }
    }
    printf("Exiting sender thread...\n");
    return NULL;
}

void handle_sigterm() {
    printf("AuthRequestManager received SIGTERM from System Manager, terminating...\n");
    running = 0;
    // Remove named pipes
    if (unlink("USER_PIPE") < 0) {
        perror("Error removing USER_PIPE");
    }

    if (unlink("BACK_PIPE") < 0) {
        perror("Error removing BACK_PIPE");
    }
    //Remove all AuthEngines
    while (num_engines > 0){
        remove_AuthEngine();
    }
    //Wait for all engines to end
    for (int i = 0; i < num_engines; i++) {
        waitpid(engines[i].pid, NULL, 0);
    }
    //Wait for all threads to end
    pthread_cancel(receiver);
    pthread_cancel(sender);
    pthread_cancel(extra_engine);

    // Clean up and free resources
    sem_destroy(&(VideoQueue->sem));
    sem_destroy(&(OtherServicesQueue->sem));
    free(VideoQueue);
    free(OtherServicesQueue);
    printf("AuthRequestManager: Ended\n");
    
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serialized config data>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, SIG_IGN); // Ignore SIGINT
    signal(SIGTERM, handle_sigterm); // Handle SIGTERM
    ConfigData config = deserialize_struct(argv[1]);

    // Create two queues
    VideoQueue = createQueue();
    OtherServicesQueue = createQueue();

    printf("AuthRequestManager: I will have to create %d Authorization Engines\n",config.auth_servers);

    // Create initial engines
    for (int i = 0; i < config.auth_servers; i++) {
        create_AuthEngine(&engines, &num_engines);
    }
    print_engine_info();

    //Create Named Pipes
    if ((mkfifo("USER_PIPE", O_CREAT | O_EXCL | 0600) < 0) && (errno != EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }

    if ((mkfifo("BACK_PIPE", O_CREAT | O_EXCL | 0600) < 0) && (errno != EEXIST)) {
        perror("Cannot create pipe: ");
        exit(0);
    }
    printf("AuthRequestManager: Named pipes created!\n");
    
    if (pthread_create(&receiver, NULL, receiver_thread, NULL) != 0) {
        perror("Error creating receiver thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&sender, NULL, sender_thread, (void *)&config) != 0) {
        perror("Error creating sender thread");
        exit(EXIT_FAILURE);
    }
    ThreadArgs args = {VideoQueue, OtherServicesQueue, config};
    if (pthread_create(&extra_engine, NULL, checkOccupancy, (void *)&args) != 0) {
        perror("Error creating extra engine thread");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < num_engines; i++) {
        waitpid(engines[i].pid, NULL, 0);
    }

    pthread_join(receiver, NULL);
    pthread_join(sender, NULL);
    pthread_join(extra_engine, NULL);

    // Clean up and free resources
    sem_destroy(&(VideoQueue->sem));
    sem_destroy(&(OtherServicesQueue->sem));
    pthread_cond_destroy(&queues_not_empty);
    pthread_mutex_destroy(&mutex);
    free(VideoQueue);
    free(OtherServicesQueue);
    return 0;
}
