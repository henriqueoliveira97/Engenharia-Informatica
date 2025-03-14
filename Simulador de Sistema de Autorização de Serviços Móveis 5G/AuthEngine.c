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
#include <sys/msg.h>
#define MSG_KEY 1234
#define BUFFER_SIZE 256
#define SHARED_MEMORY_KEY 1001
#define VIDEO_ID 345
#define MUSIC_ID 5011
#define SOCIAL_ID 9999
int msgid;
struct SharedMemory {
    pthread_mutex_t mutex;
    int num_mobile_users;
    int data[]; // Flexible array member
};
struct SharedMemory *shared_memory;
struct message {
    long msg_type;           // Message type (here, the PID of the receiving process)
    char msg_text[BUFFER_SIZE]; // Message content
};
volatile sig_atomic_t sigint_received = 0;
void handle_sigint(int sig) {
    sigint_received = 1;
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

void Register_request(char *request) {
    printf("Register request recived\n");
    char *token = strtok(request, "#");
    int id = strtol(token, NULL, 10);
    token = strtok(NULL, "#");
    int plafon_inicial = strtol(token, NULL, 10);
    printf("id: %d, plafon_inicial: %d\n", id, plafon_inicial);

    //Add user to shared memory
    int size = shared_memory->num_mobile_users * 3 + 6;
    //pthread_mutex_lock(&shared_memory->mutex);
    for(int i = 6; i < size; i++) {
        if(shared_memory->data[i] == -1) {
            shared_memory->data[i] = id;
            shared_memory->data[i+1] = plafon_inicial;
            shared_memory->data[i+2] = 0;
            //pthread_mutex_unlock(&shared_memory->mutex);
            printf("Registered MobileUser: %d| plafon_inicial %d| used_plafon: %d registered\n", shared_memory->data[i], shared_memory->data[i+1],shared_memory->data[i+2]);
            return;
        }
    }
    //pthread_mutex_unlock(&shared_memory->mutex);
    printf("Didn't found -1\n");
}

void Service_request(char *request) {
    printf("Service request recived\n");
    char *token = strtok(request, "#");
    int id = strtol(token, NULL, 10);
    token = strtok(NULL, "#");
    int service_id = strtol(token, NULL, 10);
    token = strtok(NULL, "#");
    int data = strtol(token, NULL, 10);
    printf("id: %d, service_id: %d, data: %d\n", id, service_id, data);

    //Find user in shared memory
    int size = shared_memory->num_mobile_users * 3 + 6;
    //pthread_mutex_lock(&shared_memory->mutex);
    for(int i = 6; i < size; i++) {
        if(shared_memory->data[i] == id) {
            if(shared_memory->data[i+1] >= data) {
                shared_memory->data[i+2] += data;
                if(service_id == VIDEO_ID) {
                    shared_memory->data[0] += data; //VIDEO TOTAL DATA
                    shared_memory->data[1] += 1;    //VIDEO TOTAL REQUESTS
                    //pthread_mutex_unlock(&shared_memory->mutex);
                    printf("Video request proccessed\n");
                    return;
                } else if(service_id == MUSIC_ID) {
                    shared_memory->data[2] += data; //MUSIC TOTAL DATA
                    shared_memory->data[3] += 1;    //MUSIC TOTAL REQUESTS
                    //pthread_mutex_unlock(&shared_memory->mutex);
                    printf("Music request proccessed\n");
                    return;
                } else if(service_id == SOCIAL_ID) {
                    shared_memory->data[4] += data; //SOCIAL TOTAL DATA
                    shared_memory->data[5] += 1;    //SOCIAL TOTAL REQUESTS
                    //pthread_mutex_unlock(&shared_memory->mutex);
                    printf("Social request proccessed\n");
                    return;
                }
            }
        }
    }
    pthread_mutex_unlock(&shared_memory->mutex);
    printf("User not found or not enough plafon\n");
}

void Reset_request() {
    printf("Reset Statistics request recived\n");
    for (int i = 0; i < 6; i++) {
        shared_memory->data[i] = 0;
    }
    printf("Reset Statistics request proccessed\n");
}

void Data_Stats_request() {
    printf("Data Stats request recived\n");
    struct message msg;
    msg.msg_type = 1;

    char info_string[256];
    snprintf(info_string, sizeof(info_string), "%d %d %d %d %d %d",
                shared_memory->data[0], shared_memory->data[1],
                shared_memory->data[2], shared_memory->data[3],
                shared_memory->data[4], shared_memory->data[5]);

    strcpy(msg.msg_text, info_string);
    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
    printf("Engine %d sent: %s to Backoffice\n", getpid(), msg.msg_text);
    printf("Data Stats request proccessed\n");
}

int main(int argc, char *argv[]) {
    
    printf("Hello, I am AuthEngine with pid: %d\n",getpid());
    write_log("log.txt","AUTHORIZATION_ENGINE READY");

    // Locate the message queue
    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    size_t SHARED_MEMORY_SIZE = sizeof(struct SharedMemory) + 6 * sizeof(int); // Minimum size for initial attachment

    // Get shared memory segment
    int shm_id = shmget(SHARED_MEMORY_KEY, SHARED_MEMORY_SIZE, 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    shared_memory = (struct SharedMemory *)shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Main loop
    char buffer[256];
    while (!sigint_received) {
        // Read message from the pipe
        ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (count > 0) {
            buffer[count] = '\0';  // Null-terminate the string
            printf("Engine %d: received: %s.\n", getpid(), buffer);

            //Verification to check what kind of request arrived
            if (strcmp(buffer, "1#reset") == 0) {
                printf("Engine %d: RESET REQUEST RECIVED!\n",getpid());
                Reset_request();
            } else if (strcmp(buffer, "1#data_stats") == 0) {
                printf("Engine %d: DATA STATS REQUEST RECIVED!\n",getpid());
                Data_Stats_request();
            }
            else {
                printf("It's a Mobile User request\n");;
                char *tokens[3];
                int token_count = 0;
                //Create a buffer copy to avoid strtok changes
                char buffer_copy[256];
                strcpy(buffer_copy, buffer);
                
                char *token = strtok(buffer_copy, "#");
                
                while (token != NULL) {
                    tokens[token_count] = token;
                    token = strtok(NULL, "#");
                    token_count++;   
                }
                printf("Buffer: %s\n", buffer);
                //printf("TokenCount: %d\n", token_count);

                if (token_count == 2) {
                    printf("Engine %d: REGISTER REQUEST RECIVED!\n",getpid());
                    Register_request(buffer);
                } 
                else if (token_count == 3) {
                    int service_id = strtol(tokens[1], NULL, 10);
                    if(service_id == VIDEO_ID || service_id == MUSIC_ID || service_id == SOCIAL_ID) {
                        printf("Engine %d: SERVICE %d REQUEST RECIVED!\n",getpid(),service_id);
                        Service_request(buffer);
                    } 
                }
            }
        }
    }
    return 0;
}