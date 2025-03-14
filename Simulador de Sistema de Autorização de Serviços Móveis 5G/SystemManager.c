// Authors: Luis Vaz 2022214707 Henrique Oliveira 2022211169
#include <stdio.h> 
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <sys/msg.h>
#include <pthread.h>

#define MSG_KEY 1234
#define BUFFER_SIZE 256
#define SHARED_MEMORY_KEY 1001
int shm_id;
struct SharedMemory {
    pthread_mutex_t mutex;
    int num_mobile_users;
    int data[]; // Flexible array member
};
struct message {
    long msg_type;           // Message type (PID of the receiving process)
    char msg_text[BUFFER_SIZE]; // Message content
};
int msgid;
pid_t AuthRequestManager_pid;
pid_t MonitorEngine_pid;
typedef struct {
    int MOBILE_USERS;
    int QUEUE_POS;
    int AUTH_SERVERS;
    int AUTH_PROC_TIME;
    int MAX_VIDEO_WAIT;
    int MAX_OTHERS_WAIT;
} ConfigData;

ConfigData config_data;

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
char* serialize_struct(ConfigData* data) {
    char* serialized = malloc(256); // Allocate enough space
    sprintf(serialized, "%d,%d,%d,%d,%d,%d", data->MOBILE_USERS, data->QUEUE_POS, data->AUTH_SERVERS, data->AUTH_PROC_TIME, data->MAX_VIDEO_WAIT, data->MAX_OTHERS_WAIT);
    return serialized;
}
ConfigData read_and_validate_config_file(const char* filename) {
    ConfigData config;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening config file");
        exit(EXIT_FAILURE);
    }
    
    if (fscanf(file, "%d %d %d %d %d %d", &config.MOBILE_USERS, &config.QUEUE_POS, &config.AUTH_SERVERS, &config.AUTH_PROC_TIME, &config.MAX_VIDEO_WAIT, &config.MAX_OTHERS_WAIT) != 6) {
        perror("Error reading config file");
        exit(EXIT_FAILURE);
    }
    
    printf("Mobile Users: %d\n", config.MOBILE_USERS);
    printf("Queue Pos: %d\n", config.QUEUE_POS);
    printf("Auth Servers : %d\n", config.AUTH_SERVERS);
    printf("Auth Proc Time: %d\n", config.AUTH_PROC_TIME);
    printf("Max Video Wait: %d\n", config.MAX_VIDEO_WAIT);
    printf("Max Others Wait: %d\n", config.MAX_OTHERS_WAIT);
    printf("\n");

    if (config.QUEUE_POS <= 0 || config.AUTH_SERVERS <= 0 || config.AUTH_PROC_TIME <= 0 || config.MAX_VIDEO_WAIT <= 0 || config.MAX_OTHERS_WAIT <= 0 || config.MOBILE_USERS <= 0) {
        fprintf(stderr, "Error: Invalid configuration data\n");
        exit(EXIT_FAILURE);
    }
    
    fclose(file);
    return config;
}

void handle_sigint(int sig) {
    printf("\nSystemManager: Received SIGINT, cleaning other processes first\n");
    write_log("log.txt", "RECEIVED SIGINT");
    write_log("log.txt", "5G_AUTH_PLATAFORM SIMULATOR TERMINATING");
    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
    } 
    kill(AuthRequestManager_pid, SIGTERM);
    kill(MonitorEngine_pid, SIGTERM);
    wait(NULL);
    wait(NULL);
    printf("SystemManager: Exiting...\n");
    write_log("log.txt", "5G_AUTH_PLATAFORM SIMULATOR CLOSING");
    exit(0);
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        printf("Error in usage! <config-file>\n");
        exit(-1);
    }
    write_log("log.txt", "5G_AUTH_PLATAFORM SIMULATOR STARTING");
    write_log("log.txt", "PROCESS SYSTEM MANAGER CREATED");

    signal(SIGINT, handle_sigint);  
    config_data = read_and_validate_config_file(argv[1]);
    char* serialized = serialize_struct(&config_data);
    
    // Create the message queue
    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
    write_log("log.txt", "MESSAGE QUEUE CREATED");
    printf("SystemManager: Message queue created with id %d\n", msgid);

    int num_slots = 6 + config_data.MOBILE_USERS * 3;
    //printf("Number of slots: %d\n", num_slots);
    size_t SHARED_MEMORY_SIZE = sizeof(struct SharedMemory) + num_slots * sizeof(int);

    // Create shared memory segment
    shm_id = shmget(SHARED_MEMORY_KEY, SHARED_MEMORY_SIZE, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    // Attach to the shared memory segment
    struct SharedMemory *shared_memory = (struct SharedMemory *)shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Initialize the mutex
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&shared_memory->mutex, &mutex_attr);

    // Initialize shared memory data
    pthread_mutex_lock(&shared_memory->mutex);

    shared_memory->num_mobile_users = config_data.MOBILE_USERS;

    // Initialize the data stats variables to 0
    for (int i = 0; i < 6; ++i) {
        shared_memory->data[i] = 0;
    }

    // Initialize the user mobile variables to -1
    for (int i = 6; i < num_slots; i++){
        shared_memory->data[i] = -1;
    }
    //print all the data
    //for (int i = 0; i < num_slots; i++){
      //  printf("Data[%d]: %d\n", i, shared_memory->data[i]);
    //}
    printf("SystemManager: Shared memory created with id %d\n", shm_id);
    write_log("log.txt", "SHARED MEMORY CREATED");

    // Create the AuthRequestManager and MonitorEngine processes
    AuthRequestManager_pid = fork();
    if (AuthRequestManager_pid == 0) {
        execl("./AuthRequestManager", "AuthRequestManager",serialized, NULL);
        perror("Error creating AuthRequestManager");
        exit(EXIT_FAILURE);
    }
    write_log("log.txt", "PROCESS AUTHORIZATION REQUEST MANAGER CREATED");
    
    MonitorEngine_pid = fork();
    if (MonitorEngine_pid == 0) {
        execl("./MonitorEngine", "MonitorEngine",serialized, NULL);
        perror("Error creating MonitorEngine");
        exit(EXIT_FAILURE);
    }
    write_log("log.txt", "PROCESS MOTOR ENGINE CREATED");
    free(serialized);
    wait(NULL);
    wait(NULL);
    return 0;
}






    

