#ifndef SERVER_CLIENT_H
#define SERVER_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUF_SIZE 1024
#define NUM_USERS 10
#define MAX_CLASSES 100
#define MAX_CLASS_NAME_LENGTH 50
#define MAX_CLASS_CAPACITY 200
#define MULTICAST_ADDRESS 16
#define NUM_TURMAS 5
#define MAX_TURMA_NAME_LENGTH 50
#define MULTICAST_PORT 9876
#define MULTICAST_TTL 10 

// Mensagens do protocolo
#define MSG_LOGIN "LOGIN"
#define MSG_LIST_CLASSES "LIST_CLASSES"
#define MSG_LIST_SUBSCRIBED "LIST_SUBSCRIBED"
#define MSG_SUBSCRIBE_CLASS "SUBSCRIBE_CLASS"
#define MSG_CREATE_CLASS "CREATE_CLASS"
#define MSG_SEND "SEND"
#define MSG_SAIDA "sair"


// Estrutura para representar um utilizador
struct User {
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    char user_type[BUF_SIZE];
    char turmas[NUM_TURMAS][MAX_TURMA_NAME_LENGTH];
    int num_turmas;
    //ENDEREÇO?
};

// Estrutura para representar uma turma
struct Class {
    char name[MAX_CLASS_NAME_LENGTH];
    int capacity;
    int enrolled;
    char multicast_address[MULTICAST_ADDRESS];
    char students[NUM_USERS][BUF_SIZE];
    //ARRAY COM TODOS OS ENDEREÇOS?
};


struct SharedMemory {
    struct User users[NUM_USERS];
    struct Class classes[MAX_CLASSES];
};
// Funções usadas tanto pelo servidor quanto pelo cliente
void erro(char *msg);
struct User users[NUM_USERS];
static struct Class classes[MAX_CLASSES];

#endif // SERVER_CLIENT_H