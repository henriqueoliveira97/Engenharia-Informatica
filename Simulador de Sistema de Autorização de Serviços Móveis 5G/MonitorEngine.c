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
#include <sys/msg.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#define SHARED_MEMORY_KEY 1001
#define BUFFER_SIZE 256
#define MSG_KEY 1234
volatile sig_atomic_t running = 1;
pthread_t thread1, thread2;
int msgid;
struct SharedMemory *shared_memory;

struct SharedMemory {
    pthread_mutex_t mutex;
    int num_mobile_users;
    int data[]; // Flexible array member
};

struct message {
    long msg_type;           // Message type (here, the PID of the receiving process)
    char msg_text[BUFFER_SIZE]; // Message content
};
typedef struct {
    int mobile_users;
    int queue_pos;
    int auth_servers;
    int auth_proc_time;
    int max_video_wait;
    int max_others_wait;
} ConfigData;

ConfigData deserialize_struct(char* serialized) {
    ConfigData data;
    sscanf(serialized, "%d,%d,%d,%d,%d,%d", &data.mobile_users, &data.queue_pos, &data.auth_servers, &data.auth_proc_time, &data.max_video_wait, &data.max_others_wait);
    return data;
}

void* send_periodic_stats(void* arg) {
    printf("MonitorEngine: Thread to send periodical stats created!\n");
    struct message msg;
    msg.msg_type = 1; // Backoffice ID

    // Locate the message queue
    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        pthread_exit(NULL);
    }

    while (running) {
        sleep(30);
        char info_string[256];
        snprintf(info_string, sizeof(info_string), "%d %d %d %d %d %d",
                 shared_memory->data[0], shared_memory->data[1],
                 shared_memory->data[2], shared_memory->data[3],
                 shared_memory->data[4], shared_memory->data[5]);

        //printf("Periodic Info String: %s\n", info_string);
        snprintf(msg.msg_text, sizeof(msg.msg_text), "%s", info_string);

        if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
            perror("msgsnd failed");
        }
        else {
            printf("Sent stats to backoffice: %s\n", msg.msg_text);
        }
    }
    return NULL;
}

void* check_plafon(void* arg) {
    printf("MonitorEngine: Plafon Check Started\n");
    struct message msg;
    int num_mobile_users = shared_memory->num_mobile_users;
    int size = 6 + num_mobile_users * 3;
    
    // Locate the message queue
    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        pthread_exit(NULL);
    }
    //Send message to Message Queue if plafon is 80%, 90%, or 100% used
    while (running) {
        sleep(10); 
        for (int i = 6; i < size; i += 3) {
            int mobile_user_id = shared_memory->data[i];
            int initial_plafon = shared_memory->data[i + 1];
            int used_plafon = shared_memory->data[i + 2];
            if(used_plafon > 0 && initial_plafon > 0){
                msg.msg_type = mobile_user_id;
                printf("Mobile User %d Initial Plafon: %d, Used Plafon: %d\n", mobile_user_id, initial_plafon, used_plafon);
                float percentage_used = (float)used_plafon / initial_plafon * 100;
                //printf("Percentage Used: %.2f\n", percentage_used);

                if (percentage_used >= 80 && percentage_used < 90) {
                    snprintf(msg.msg_text, sizeof(msg.msg_text), "ALERT: 80%% PLAFON USAGE REACHED!");
                    if (msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
                        perror("msgsnd failed");
                    }
                    printf("Sending to %ld using Message Queue: %s\n",msg.msg_type, msg.msg_text);
                    printf("Alert: Mobile User %d has used 80%% of their plafon\n",mobile_user_id);
                }
                else if (percentage_used >= 90 && percentage_used < 100) {
                    snprintf(msg.msg_text, sizeof(msg.msg_text), "ALERT: 90%% PLAFON USAGE REACHED!");
                    if(msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
                        perror("msgsnd failed");
                    }
                    printf("Sending to %ld using Message Queue: %s\n",msg.msg_type, msg.msg_text);
                    printf("Alert: Mobile User %d has used 90%% of their plafon\n",mobile_user_id);
                }
                else if (percentage_used >= 100) {
                    snprintf(msg.msg_text, sizeof(msg.msg_text), "ALERT: 100%% PLAFON USAGE REACHED!");
                    if(msgsnd(msgid, &msg, sizeof(msg.msg_text), 0) == -1) {
                        perror("msgsnd failed");
                    }
                    printf("Sending to %ld using Message Queue: %s\n",msg.msg_type, msg.msg_text);
                    printf("Alert: Mobile User %d has reached 100%% of their plafon\n",mobile_user_id);
                    shared_memory->data[i + 2] = 0;
                }
            }
            else {
                continue;
            }
        }
    }
    return NULL;
}

void handle_sigterm(int sig) {
    printf("MonitorEngine: received SIGTERM from System Manager, terminating...\n");
    running = 0;
    pthread_cancel(thread1);
    pthread_cancel(thread2);
    printf("MonitorEngine: terminated\n");
}

int main(int argc, char *argv[]) {
    signal(SIGINT, SIG_IGN); // Ignore SIGINT
    signal(SIGTERM, handle_sigterm); // Handle SIGTERM

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <serialized config data>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    printf("Monitor Engine: I will generate alerts!\n");

    size_t SHARED_MEMORY_SIZE = sizeof(struct SharedMemory) + 6 * sizeof(int); 

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

    if (pthread_create(&thread1, NULL, send_periodic_stats, NULL) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread2, NULL, check_plafon, NULL) != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    // Wait for the thread to finish
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    return 0;
}
