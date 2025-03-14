#include "definicoesrc.h"


void erro(char *msg) {
    printf("Erro: %s\n", msg);
    exit(EXIT_FAILURE);
}
// Função para receber mensagens multicast de uma turma
void receive_multicast(const char *multicast_address) {
    int sockfd;
    struct sockaddr_in multicast_addr, client_addr;
    socklen_t addr_len;
    char buffer[BUF_SIZE];

    // Criação do socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço de recepção multicast
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(multicast_address);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    // Associa o socket ao endereço multicast
    if (bind(sockfd, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Adere ao grupo multicast
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(multicast_address);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Aguardando mensagens multicast da turma %s...\n", multicast_address);

    // Loop para receber mensagens multicast continuamente
    while (1) {
        addr_len = sizeof(client_addr);
        bzero(buffer, sizeof(buffer));
        if (recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len) < 0) {
            perror("recvfrom");
            break; // Encerra o loop em caso de erro
        }

        // Processa e imprime a mensagem recebida
        printf("Mensagem recebida da turma %s: %s\n", multicast_address, buffer);
    }

    // Fecha o socket UDP
    close(sockfd);
}

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    struct User users[NUM_USERS];

    if (argc != 3) {
        printf("Uso: %s <endereço_do_servidor> <PORTO_TURMAS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor\n");

    // Envio de login
    write(sockfd, "LOGIN", strlen("LOGIN"));

    // Recebe mensagem "Username: "
    bzero(buffer, sizeof(buffer));
    if (read(sockfd, buffer, BUF_SIZE) < 0) {
        perror("read");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("%s", buffer);

    // Pede nome de usuário
    fgets(buffer, BUF_SIZE, stdin);
    write(sockfd, buffer, strlen(buffer));
    char username[BUF_SIZE];
    strcpy(username,buffer);
    if (username[strlen(username) - 1] == '\n') {
    username[strlen(username) - 1] = '\0';}

    // Recebe mensagem "Password: "
    bzero(buffer, sizeof(buffer));
    if (read(sockfd, buffer, BUF_SIZE) < 0) {
        perror("read");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("%s", buffer);

    // Pede a senha
    fgets(buffer, BUF_SIZE, stdin);
    write(sockfd, buffer, strlen(buffer));

    // Recebe confirmação de autenticação
    bzero(buffer, sizeof(buffer));
    if (read(sockfd, buffer, BUF_SIZE) < 0) {
        perror("read");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Autenticado, envia comandos para o servidor
    if (strcmp(buffer, "Autenticação completa.\n") == 0) {
        printf("OK\n");
        printf("Insira o comando. Para sair do programa escreva 'sair'.\n");

        while (1) {
            printf(": ");
            fgets(buffer, sizeof(buffer), stdin);

            // Enviar comandos para o servidor
            write(sockfd, buffer, strlen(buffer));

            // Receber resposta do servidor
            bzero(buffer, sizeof(buffer));
            if (read(sockfd, buffer, sizeof(buffer) - 1) < 0) {
                perror("read");
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            // Loop para buscar o nome e o endereço multicast de cada turma em que está inscrito
            
            if (strncmp(buffer, "sair", 4) == 0) {
                printf("A sair...\n");
                break;
            }else if (strncmp(buffer, "ENVIADO",strlen("ENVIADO")) == 0) {
                char multicast[MULTICAST_ADDRESS]; 
                strcpy(multicast,"224.105.198.103");
                receive_multicast(multicast);
                break;
            }
            printf("%s", buffer);
        }

        // Processar comandos do professor
        
    } else if (strcmp(buffer, "Autenticação falhou. Tente novamente.\n") == 0) {
        printf("REJECTED\n");
    }
    close(sockfd);
    exit(0);
}