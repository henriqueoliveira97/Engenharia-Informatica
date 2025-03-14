// Authors: Luis Vaz 2022214707 Henrique Oliveira 2022211169
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h> 
#include <semaphore.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define VIDEO_ID_STR "345"
#define MUSIC_ID_STR "5011"
#define SOCIAL_ID_STR "9999"
#define BUFFER_SIZE 256
#define MSG_KEY 1234

struct message {
    long msg_type;           // Message type (here, the PID of the receiving process)
    char msg_text[BUFFER_SIZE]; // Message content
};
// Global variables
int contador_pedidos = 0;
int running = 1;
int user_pipe_fd;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t threadMusic_id, threadVideo_id, threadSocial_id, threadListen_id;

struct ThreadArgs {
    int interval;
    int DadosReservar;
    int max_pedidos;
};

void *threadMusic_function(void *arg) {
    
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int interval = args->interval;
    int DadosReservar = args->DadosReservar;
    int max_pedidos = args->max_pedidos;
    int pid = getpid();

    char requestMessage[BUFFER_SIZE];
    sprintf(requestMessage, "%d#%s#%d", pid, MUSIC_ID_STR, DadosReservar);
    //printf("Request Message: %s\n", requestMessage);

    if ((user_pipe_fd = open("USER_PIPE", O_WRONLY)) < 0) {
        perror("Cannot open pipe for writing: ");
        exit(EXIT_FAILURE);
    }

    while (running) {
        sleep(interval);

        if (contador_pedidos >= max_pedidos) {
            printf("Max number of requests reached!\n");
            running = 0;
            break;
        }

        pthread_mutex_lock(&mutex);
        write(user_pipe_fd, requestMessage, strlen(requestMessage) + 1);
        contador_pedidos++;
        printf("Sent MUSIC request: %s \n", requestMessage);
        pthread_mutex_unlock(&mutex);

    }
    printf("Exiting Music thread...\n");
    pthread_cancel(threadListen_id);
    return NULL;
}

void *threadSocial_function(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int interval = args->interval;
    int DadosReservar = args->DadosReservar;
    int max_pedidos = args->max_pedidos;
    int pid = getpid();

    char requestMessage[BUFFER_SIZE];
    sprintf(requestMessage, "%d#%s#%d", pid, SOCIAL_ID_STR, DadosReservar);
    //printf("Request Message: %s\n", requestMessage);

    if ((user_pipe_fd = open("USER_PIPE", O_WRONLY)) < 0) {
        perror("Cannot open pipe for writing: ");
        exit(EXIT_FAILURE);
    }

    while (running) {
        sleep(interval);

        if (contador_pedidos >= max_pedidos) {
            printf("Max number of requests reached!\n");
            running = 0;
            break;
        }

        pthread_mutex_lock(&mutex);
        write(user_pipe_fd, requestMessage, strlen(requestMessage) + 1);
        contador_pedidos++;
        printf("Sent SOCIAL request: %s \n", requestMessage);
        pthread_mutex_unlock(&mutex);
    }
    printf("Exiting Social thread...\n");
    pthread_cancel(threadListen_id);
    return NULL;
}

void *threadVideo_function(void *arg) {
    struct ThreadArgs *args = (struct ThreadArgs *)arg;
    int interval = args->interval;
    int DadosReservar = args->DadosReservar;
    int max_pedidos = args->max_pedidos;
    int pid = getpid();

    char requestMessage[BUFFER_SIZE];
    sprintf(requestMessage, "%d#%s#%d", pid, VIDEO_ID_STR, DadosReservar);
    //printf("Request Message: %s\n", requestMessage);

    if ((user_pipe_fd = open("USER_PIPE", O_WRONLY)) < 0) {
        perror("Cannot open pipe for writing: ");
        exit(EXIT_FAILURE);
    }

    while (running) {
        sleep(interval);
        if (contador_pedidos >= max_pedidos) {
            printf("Max number of requests reached!\n");
            running = 0;
            break;
        }
        pthread_mutex_lock(&mutex);
        write(user_pipe_fd, requestMessage, strlen(requestMessage) + 1);
        contador_pedidos++;
        printf("Sent VIDEO request: %s \n", requestMessage);
        pthread_mutex_unlock(&mutex);
    }
    printf("Exiting Video thread...\n");
    return NULL;
}

void* listen_to_queue(void* arg) {
    printf("MobileUser: Listening thread started...\n");
    int msgid;
    struct message msg;
    pid_t pid = getpid();
    //printf("Mobile User PID: %d\n", pid);

    // Locate the message queue
    msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        pthread_exit(NULL);
    }

    // Continuously receive messages addressed to this process's PID
    while (running) {
        
        if (msgrcv(msgid, &msg, sizeof(msg.msg_text), pid, 0) == -1) {
            perror("msgrcv");
            pthread_exit(NULL);
        }
        
        pthread_mutex_lock(&mutex);
        printf("\n%s.\n", msg.msg_text);

        //Compare the messafe received with the message "STOP"
        if (strcmp(msg.msg_text, "ALERT: 100% PLAFON USAGE REACHED!") == 0) {
            printf("Ending process...\n");
            close(user_pipe_fd);
            running = 0;
            printf("Listening thread exiting...\n");
            pthread_cancel(threadListen_id);
            pthread_cancel(threadMusic_id);
            pthread_cancel(threadVideo_id);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void handle_sigint(int sig) {
    running = 0;
    printf("\nReceived SIGINT, exiting...\n");
    close(user_pipe_fd);
    printf("Should stop: %d\n", running);
    pthread_cancel(threadMusic_id);
    pthread_cancel(threadVideo_id);
    pthread_cancel(threadSocial_id);
    pthread_cancel(threadListen_id);
}

int main(int argc, char *argv[]) {

    if (argc != 7) {
        printf("Error in usage!\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGINT, handle_sigint);
    
    // Transformation of the input data
    int plafonInicial = atoi(argv[1]);
    int max_pedidos = atoi(argv[2]);
    int intervalo_Video = atoi(argv[3]);
    int intervalo_Musica = atoi(argv[4]);
    int intervalo_Social = atoi(argv[5]);
    int DadosReservar = atoi(argv[6]);
 
    if ((plafonInicial <= 0) || (max_pedidos <= 0) || (intervalo_Video <= 0) || (intervalo_Musica <= 0) || (intervalo_Social <= 0) || (DadosReservar <= 0)) {
        printf("Invalid Inputs!\n");
        exit(0);
    }

    printf("Plafon Inicial do Cliente: %d\n", plafonInicial);
    printf("Máximo de pedidos: %d\n", max_pedidos);
    printf("Intervalo periodico do Video: %d\n", intervalo_Video);
    printf("Intervalo periodico da Musica: %d\n", intervalo_Musica);
    printf("Intervalo periodico do Social: %d\n", intervalo_Social);
    printf("Quantidade de dados a reservar: %d\n", DadosReservar);

    struct ThreadArgs music_args = {intervalo_Musica, DadosReservar, max_pedidos};
    struct ThreadArgs video_args = {intervalo_Video, DadosReservar, max_pedidos};
    struct ThreadArgs social_args = {intervalo_Social, DadosReservar, max_pedidos};

    //Build Register Message MobileUserPid#PlafonInicial
    pid_t ID_mobile_user = getpid();
    char register_message[256];
    sprintf(register_message, "%d#%d", ID_mobile_user, plafonInicial);
    printf("\nRegister Message: %s\n", register_message);

    //Open Named Pipe to Write "USER_PIPE"
    int user_pipe_fd;
    if ((user_pipe_fd = open("USER_PIPE", O_WRONLY)) == -1) {
        perror("Error opening USER_PIPE");
        exit(EXIT_FAILURE);
    }

    //Write Register Message to "USER_PIPE"
    if (write(user_pipe_fd, register_message, strlen(register_message)) == -1) {
        perror("Error writing to USER_PIPE");
        exit(EXIT_FAILURE);
    }

    // Create threads
    if (pthread_create(&threadMusic_id, NULL, threadMusic_function, (void *)&music_args) != 0) {
        fprintf(stderr, "Error creating Music thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&threadVideo_id, NULL, threadVideo_function, (void *)&video_args) != 0) {
        fprintf(stderr, "Error creating Video thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&threadSocial_id, NULL, threadSocial_function, (void *)&social_args) != 0) {
        fprintf(stderr, "Error creating Social thread\n");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&threadListen_id, NULL, listen_to_queue, NULL) != 0) {
        fprintf(stderr, "Error creating Listen thread\n");
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(threadMusic_id, NULL);
    pthread_join(threadVideo_id, NULL);
    pthread_join(threadSocial_id, NULL);
    pthread_join(threadListen_id, NULL);
    
    //Close Named Pipe "USER_PIPE"
    close(user_pipe_fd);
    return 0;
}