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
#include <pthread.h>
#include <semaphore.h>
#include <sys/msg.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#define BACK_PIPE "BACK_PIPE"
#define BUFFER_SIZE 256
#define MSG_KEY 1234

struct message {
    long msg_type;           // Message type (here, the PID of the receiving process)
    char msg_text[BUFFER_SIZE]; // Message content
};
int back_pipe_fd;
int running = 1;
pthread_t send_thread, listen_thread;



void handle_sigint(int sig) {
    running = 0;
    printf("\nReceived SIGINT, exiting...\n");
    printf("Running: %d\n", running);
    close(back_pipe_fd);
    pthread_mutex_destroy(&mutex);
    pthread_cancel(send_thread);
    pthread_cancel(listen_thread);
    printf("Cleanup done\n");   
    exit(EXIT_SUCCESS);
}

void *send_commands(void *arg) {
    char command[BUFFER_SIZE];

    while (running) {
        printf("%ls",&running);
        pthread_mutex_lock(&mutex);
        printf("Enter command (data_stats, reset): ");
        
        if (fgets(command, BUFFER_SIZE, stdin) == NULL) {
            if (feof(stdin)) break; // End of input
            continue;
        }
        pthread_mutex_unlock(&mutex);

        command[strcspn(command, "\n")] = '\0'; // Remove newline character

        if (strcmp(command, "data_stats") == 0 || strcmp(command, "reset") == 0) {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, BUFFER_SIZE+10, "1#%s", command);

            if (write(back_pipe_fd, buffer, strlen(buffer)) == -1) {
                perror("Error writing to BACK_PIPE");
                exit(EXIT_FAILURE);
            }
        } else {
            printf("Unknown command. Please enter 'data_stats' or 'reset'.\n");
        }
    }
    printf("Send thread exiting...\n");
    return NULL;
}

void* listen_to_queue(void* arg) {
    int msgid;
    struct message msg;
    int values[6];

    // Locate the message queue
    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        pthread_exit(NULL);
    }

    // Continuously receive messages addressed to this process's PID
    while (running) {
        if (msgrcv(msgid, &msg, sizeof(msg.msg_text), 1, 0) == -1) {
            perror("msgrcv");
            pthread_exit(NULL);
        }
        pthread_mutex_lock(&mutex);
        printf("\nListening thread: %s\n", msg.msg_text);
        
        // Split the input string to extract six tokens
        char* token;
        char* rest = msg.msg_text;
        int count = 0;

        while ((token = strtok_r(rest, " ", &rest))) {
            if (count < 6) {
                values[count] = atoi(token);
            }
            count++;
        }
        if (count == 6) {
            sscanf(msg.msg_text, "%d %d %d %d %d %d",
            &values[0], &values[1], &values[2],
            &values[3], &values[4], &values[5]);

            printf("%-10s %12s %12s\n", "Service", "Total Data", "Auth Reqs");
            printf("%-10s %12d %12d\n", "VIDEO", values[0], values[1]);
            printf("%-10s %12d %12d\n", "MUSIC", values[2], values[3]);
            printf("%-10s %12d %12d\n", "SOCIAL", values[4], values[5]);
            printf("\n");
            pthread_mutex_unlock(&mutex);
          
        }else{
            pthread_mutex_unlock(&mutex);
            continue;
        }   
    }
    printf("Listening thread exiting...\n");
    return NULL;
}


int main() {
    signal(SIGINT, handle_sigint);

    // Open named pipe
    if ((back_pipe_fd = open(BACK_PIPE, O_WRONLY)) == -1) {
        perror("Error opening BACK_PIPE");
        exit(EXIT_FAILURE);
    }

    if (pthread_create(&send_thread, NULL, send_commands, NULL) != 0) {
        perror("Error creating send_commands thread");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&listen_thread, NULL, listen_to_queue, NULL) != 0) {
        perror("Error creating listen_to_queue thread");
        exit(EXIT_FAILURE);
    }
    pthread_join(send_thread, NULL);
    pthread_join(listen_thread, NULL); 
    return 0;
}