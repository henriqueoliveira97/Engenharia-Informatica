#include "definicoesrc.h"

#define BUF_SIZE 1024
#define SERVER_IP "193.137.100.1"
#define SERVER_PORT 9900

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in server_addr;
    char buffer[BUF_SIZE];
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    int authenticated = 0;
    

    if (argc != 3) {
        printf("Uso: %s <endereço_do_servidor> <PORTO_TURMAS>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Criação do socket UDP
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        error("Erro ao criar o socket UDP");
    }


    // Configuração do endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        error("Endereço IP inválido");
    }

    printf("Conectado ao servidor\n");

    while (1) {
        // Se não estiver autenticado, solicitar username e password
        if (!authenticated) {
            printf("Consola do Administrador - Autenticação:\n");
            printf("Username: ");
            fgets(username, BUF_SIZE, stdin);
            username[strcspn(username, "\n")] = '\0'; // Remover quebra de linha

            printf("Password: ");
            fgets(password, BUF_SIZE, stdin);
            password[strcspn(password, "\n")] = '\0'; // Remover quebra de linha

            // Enviar username e password para o servidor para autenticação
            snprintf(buffer, BUF_SIZE, "LOGIN %s %s", username, password);
            if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
                error("Erro ao enviar credenciais para o servidor");

            // Receber resposta do servidor
            int server_addr_len = sizeof(server_addr);
            int bytes_received = recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
            if (bytes_received < 0) {
                error("Erro ao receber resposta do servidor");
            }

            buffer[bytes_received] = '\0'; 

            printf("Resposta recebida: %s\n", buffer); 

            // Verificar se a autenticação foi bem-sucedida
            if (strcmp(buffer, "OK") == 0) {
                printf("Autenticação bem-sucedida.\n");
                authenticated = 1;
            } else {
                printf("Autenticação falhou. Tente novamente.\n");
            }
        } else {
            // Autenticado, permitir que o administrador envie comandos
            printf("Consola do Administrador - Comandos disponíveis:\n");
            printf("1. ADD_USER <username> <password> <user_type>\n");
            printf("2. DELETE_USER <username>\n");
            printf("3. LIST\n");
            printf("4. QUIT_SERVER\n");
            printf("Comando: ");

            // Ler o comando do administrador
            fgets(buffer, BUF_SIZE, stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Remover quebra de linha

            // Enviar o comando para o servidor
            if (sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
                error("Erro ao enviar o comando para o servidor");
            
            if (strcmp(buffer, "BYE") == 0){
                printf("A sair...");
                break;
            }

            // Receber a resposta do servidor
            int server_addr_len = sizeof(server_addr);
            if (recvfrom(sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len) < 0)
                error("Erro ao receber resposta do servidor");

            // Exibir a resposta do servidor
            printf("Resposta do servidor: %s\n", buffer);

        }
    }

    close(sock);
    printf("Conexão encerrada.\n");

    return 0;
}