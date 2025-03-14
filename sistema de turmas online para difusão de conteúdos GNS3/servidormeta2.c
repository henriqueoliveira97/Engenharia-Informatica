#include "definicoesrc.h"

#define CREDENTIALS_FILE "config.txt"



void erro(char *msg);
void process_client(int client_fd, struct sockaddr_in client_addr, int sock_config, struct SharedMemory *shared_memory);
void handle_admin_commands(int sock_config);
void add_user(const char *username, const char *password, const char *user_type);
void delete_user(const char *username);
void list_users();
void list_classes(int client_fd);
void save_users_to_file(const char *filename);
void load_users_from_file(const char *filename);
void list_subscribed(int client_fd, const char *username, const char *password);

int subscribe_class(const char *class_name, const char *username);
int create_class(const char *class_name, int class_size, char *multicast_address);
int send_content(const char *username, const char *class_name, const char *content);
int authenticate_client(const char *username, const char *password);
int authenticate_admin(const char *username, const char *password);
bool is_professor(const char *username); 
char sanitize_input(char *input);
struct SharedMemory *shared_memory;
int main(int argc, char *argv[]) {
    int fd, cliente;
    struct sockaddr_in addr, client_addr;
    socklen_t client_addr_size;
    int sock_config = socket(AF_INET, SOCK_DGRAM, 0);

    // Configurar a opção SO_REUSEADDR
    int optval = 1;
    if (setsockopt(sock_config, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        error("Erro ao definir a opção SO_REUSEADDR");
    }

    if (argc != 4) {
        printf("Uso: %s <PORTO_TURMAS> <PORTO_CONFIG> <FICHEIRO_UTILIZADORES>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    key_t key = ftok("/tmp", 'A');
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    int shm_id = shmget(key, sizeof(struct SharedMemory), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shared_memory = shmat(shm_id, NULL, 0);
    if (shared_memory == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }
    memset(shared_memory, 0, sizeof(struct SharedMemory));
    

    int PORTO_TURMAS = atoi(argv[1]);
    int PORTO_CONFIG = atoi(argv[2]);

    bzero((void *) &addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORTO_TURMAS);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        erro("na funcao socket");
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
        erro("na funcao bind");
    if (listen(fd, 5) < 0)
        erro("na funcao listen");

    printf("Servidor aguardando conexões na porta %d...\n", PORTO_TURMAS);

    signal(SIGCHLD, SIG_IGN);
    char *filename = argv[3];
    load_users_from_file(filename);


    struct sockaddr_in config_addr;
    bzero((char *)&config_addr, sizeof(config_addr));
    config_addr.sin_family = AF_INET;
    config_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    config_addr.sin_port = htons(PORTO_CONFIG);

    // Associar o socket à porta de configuração
    if (bind(sock_config, (struct sockaddr *)&config_addr, sizeof(config_addr)) < 0)
        erro("Erro ao fazer bind do socket de configuração");

    // Chamada da função para lidar com os comandos do administrador por UDP
    if (fork() == 0) {
        handle_admin_commands(sock_config);
        exit(0);
    } 

    while (1) {
        client_addr_size = sizeof(client_addr);
        cliente = accept(fd, (struct sockaddr *)&client_addr, &client_addr_size);
        if (cliente < 0) {
            perror("accept");
            continue; 
        }
        if (fork() == 0) {
            close(fd); 
            process_client(cliente, client_addr, sock_config, shared_memory);
            exit(0);
        } else {
            close(cliente);
        }
        save_users_to_file(filename);
    }
    
    if (shmdt(shared_memory) == -1) {
        perror("shmdt");
        exit(EXIT_FAILURE);
    }
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(EXIT_FAILURE);
    }
    
    close(fd);
    return 0;
}
char sanitize_input(char *input) {
    size_t len = strlen(input);
    if (len > 0 && input[len - 1] == '\n') {
        input[len - 1] = '\0';
    }
    return *input;
}

bool is_professor(const char *username) {
    for (int i = 0; i < NUM_USERS; ++i) {
        if (strcmp(shared_memory->users[i].username, username) == 0 && strcmp(shared_memory->users[i].user_type, "professor") == 0) {
            return true; // O usuário é um professor
        }
    }
    return false; // O usuário não é um professor
}
bool is_admin(const char *username) {
    for (int i = 0; i < NUM_USERS; ++i) {
        if (strcmp(shared_memory->users[i].username, username) == 0 && strcmp(shared_memory->users[i].user_type, "administrador") == 0) {
            return true; // O usuário é um professor
        }
    }
    return false; // O usuário não é um professor
}
int authenticate_client(const char *username, const char *password) {
    

    for (int i = 0; i < NUM_USERS; i++) {
        if (strcmp(shared_memory->users[i].username, username) == 0 && strcmp(shared_memory->users[i].password, password) == 0) {
            return 1; // Autenticação bem-sucedida
        }
    }
    return 0; // Autenticação falhou
}

void process_client(int client_fd, struct sockaddr_in client_addr, int sock_config, struct SharedMemory *shared_memory) {
    int autenticado = 0;
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    char buffer[BUF_SIZE];
    int nread;

    printf("Cliente conectado: %s\n", inet_ntoa(client_addr.sin_addr));

    while (1) {
        nread = read(client_fd, buffer, BUF_SIZE - 1);
        if (nread < 0) {
            perror("read");
            close(client_fd);
            return; // Encerrar apenas a conexão com o cliente
        } else if (nread == 0) {
            printf("Cliente desconectado\n");
            break;
        }
        buffer[nread] = '\0';
        sanitize_input(buffer);

        if (!autenticado) { // procede à autenticação
            if (strcmp(buffer, "LOGIN") == 0) {
                write(client_fd, "Username: ", strlen("Username: "));
                nread = read(client_fd, username, BUF_SIZE - 1);
                if (nread < 0) {
                    perror("read");
                    close(client_fd);
                    return;
                }
                username[nread] = '\0';
                sanitize_input(username);

                write(client_fd, "Password: ", strlen("Password: "));
                nread = read(client_fd, password, BUF_SIZE - 1);
                if (nread < 0) {
                    perror("read");
                    close(client_fd);
                    return;
                }
                password[nread] = '\0';
                sanitize_input(password);

                autenticado = authenticate_client(username, password);
                if (autenticado) {
                    write(client_fd, "Autenticação completa.\n", strlen("Autenticação completa.\n"));
                } else {
                    write(client_fd, "Autenticação falhou. Tente novamente.\n", strlen("Autenticação falhou. Tente novamente.\n"));
                }
            }
        } else {
            // Processar comandos do cliente após a autenticação
            printf("Comando recebido do cliente %s: %s\n", inet_ntoa(client_addr.sin_addr), buffer);
            sanitize_input(buffer);
            if (strcmp(buffer, MSG_SUBSCRIBE_CLASS) == 0) {
                write(client_fd, "Nome da turma a subscrever: \n", strlen("Nome da turma a subscerver: \n"));
                nread = read(client_fd, buffer, BUF_SIZE - 1);
                if (nread < 0) {
                    perror("read");
                    close(client_fd);
                    return;
                }
                buffer[nread] = '\0';
                sanitize_input(buffer);
                char class_name[MAX_CLASS_NAME_LENGTH];
                char address_multicast[MULTICAST_ADDRESS];
                strcpy(class_name, buffer);
                int result = subscribe_class(class_name, username);
                for (int i = 0; i < MAX_CLASSES; ++i) {
                    if (strcmp(shared_memory->classes[i].name, class_name) == 0) {
                        strcpy(address_multicast, shared_memory->classes[i].multicast_address);
                        break;
                    }
                }
                if (result) {
                    char response[BUF_SIZE];
                    snprintf(response, sizeof(response), "OK %s\n",address_multicast);
                    write(client_fd, response, strlen(response));
                } else {
                    write(client_fd, "REJECTED\n", strlen("REJECTED\n"));
                }
            } else if (strcmp(buffer, MSG_LIST_CLASSES) == 0) {
                list_classes(client_fd);
            } else if (strcmp(buffer, MSG_LIST_SUBSCRIBED) == 0) {
                list_subscribed(client_fd,username,password);
            } else if (is_professor(username)) {
                if (strcmp(buffer, MSG_CREATE_CLASS) == 0) {
                    write(client_fd, "Nome da turma: \n", strlen("Nome da turma: \n"));
                    nread = read(client_fd, buffer, BUF_SIZE - 1);
                    if (nread < 0) {
                        perror("read");
                        close(client_fd);
                        return;
                    }
                    buffer[nread] = '\0';
                    sanitize_input(buffer);
                    char class_name[MAX_CLASS_NAME_LENGTH];
                    strncpy(class_name, buffer, MAX_CLASS_NAME_LENGTH - 1);
                    class_name[MAX_CLASS_NAME_LENGTH - 1] = '\0';
                    
                    write(client_fd, "Tamanho da turma: \n", strlen("Tamanho da turma: \n")); 
                    nread = read(client_fd, buffer, BUF_SIZE - 1);
                    if (nread < 0) {
                        perror("read");
                        close(client_fd);
                        return;
                    }
                    buffer[nread] = '\0';
                    sanitize_input(buffer);
                    int class_size = atoi(buffer);
                    if(class_size>MAX_CLASS_CAPACITY){
                        printf("A capacidade da turma foi mudada para o valor máximo (100). Saia e Volte a criar turma se quiser menos\n");
                        class_size=MAX_CLASS_CAPACITY;
                    }
                    printf("Nome da turma: %s, Tamanho da turma: %d\n", class_name, class_size);
                    char multicast_address[MULTICAST_ADDRESS];
                    int result=0;
                    result = create_class(class_name, class_size, multicast_address);
                    if (result) {
                        write(client_fd, "OK ", strlen("OK "));
                        write(client_fd, multicast_address, strlen(multicast_address));
                        write(client_fd, "\n", 1);
                    } else {
                        write(client_fd, "LIMIT_REACHED\n", strlen("LIMIT_REACHED\n"));
                    }
                } else if (strcmp(buffer, MSG_SEND) == 0) {
                    write(client_fd, "Nome da turma: \n", strlen("Nome da turma: \n")); 
                    nread = read(client_fd, buffer, BUF_SIZE - 1);
                    if (nread < 0) {
                        perror("read");
                        close(client_fd);
                        return;
                    }
                    buffer[nread] = '\0';
                    sanitize_input(buffer);
                    char class_name[MAX_CLASS_NAME_LENGTH];
                    char content[BUF_SIZE];
                    strncpy(class_name, buffer, MAX_CLASS_NAME_LENGTH - 1);
                    class_name[MAX_CLASS_NAME_LENGTH - 1] = '\0';
                    
                    write(client_fd, "Conteúdo a enviar: \n", strlen("Conteúdo a enviar: \n")); 
                    nread = read(client_fd, buffer, BUF_SIZE - 1);
                    if (nread < 0) {
                        perror("read");
                        close(client_fd);
                        return;
                    }
                    buffer[nread] = '\0';
                    sanitize_input(buffer);
                    strncpy(content, buffer,BUF_SIZE - 1);

                    int result = send_content(username, class_name, content);

                    if (result) {
                        char response[BUF_SIZE];
                        snprintf(response, sizeof(response), "ENVIADO");
                        write(client_fd, response, strlen(response));
                    } else {
                        write(client_fd, "Conteúdo não enviado\n", strlen("Conteúdo não enviado\n"));
                    }
                }else if (strcmp(buffer, MSG_SAIDA) == 0) {
                    write(client_fd, "sair", strlen("sair"));
                }else {
                write(client_fd, "Comando inválido.\n", strlen("Comando inválido.\n"));
                }
            }else if (strcmp(buffer, MSG_SAIDA) == 0) {
                write(client_fd, "sair", strlen("sair"));   
            } else {
                write(client_fd, "Comando inválido.\n", strlen("Comando inválido.\n"));
            }

        }
    }

    close(client_fd);
}

#include "definicoesrc.h"

void handle_admin_commands(int sock_config) {
    printf("Entrei no handle! ");
    struct sockaddr_in server_config_addr;
    socklen_t server_config_addr_len = sizeof(server_config_addr);
    char buffer[BUF_SIZE];
    char username[BUF_SIZE];
    char password[BUF_SIZE];
    int authenticated = 0;
    int running = 1; // Flag para controlar a execução do loop

    // Loop para processar comandos do administrador
    while (running) {
        // Limpar o buffer antes de receber um novo comando
        memset(buffer, 0, sizeof(buffer));

        // Receber comando do administrador
        int bytes_received = recvfrom(sock_config, buffer, BUF_SIZE - 1, 0, (struct sockaddr *)&server_config_addr, &server_config_addr_len);
        if (bytes_received <= 0) {
            // Se houver erro ou nenhum dado recebido, encerrar a função
            perror("recvfrom");
            break;
        }
        buffer[bytes_received] = '\0'; // Adicionar terminador nulo
        sanitize_input(buffer);
        // Analisar o comando recebido e executar a ação correspondente
        if (!authenticated) {
            // Verificar se o comando recebido é um pedido de login
            if (strncmp(buffer, "LOGIN", 5) == 0) {
                printf("Vou realizar o Login\n");
                // Extrair username e password dos parâmetros recebidos
                if (sscanf(buffer, "LOGIN %s %s", username, password) != 2) {
                    sendto(sock_config, "Formato inválido para LOGIN. Use 'LOGIN username password'.", strlen("Formato inválido para LOGIN. Use 'LOGIN username password'."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
                    continue;
                }
                printf("Vou realizar a autenticação!\n");
                username[bytes_received] = '\0';
                password[bytes_received] = '\0';
                // Realizar autenticação
                if (authenticate_admin(username, password)) {
                    printf("Vou enviar o OK!");
                    sendto(sock_config, "OK", strlen("OK"), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
                    authenticated = 1; // Usuário autenticado como administrador
                } else {
                    sendto(sock_config, "Autenticação falhou. Tente novamente.", strlen("Autenticação falhou. Tente novamente."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
                }
            } else {
                // Comando inválido antes da autenticação
                sendto(sock_config, "Autenticação necessária. Envie o comando 'LOGIN username password'.", strlen("Autenticação necessária. Envie o comando 'LOGIN username password'."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
            }
        } else {
            sanitize_input(buffer);
            if (strncmp(buffer, "ADD_USER", 8) == 0) {
                char new_username[BUF_SIZE];
                char new_password[BUF_SIZE];
                char user_type[BUF_SIZE];
                sscanf(buffer, "%*s %s %s %s", new_username, new_password, user_type);
                add_user(new_username, new_password, user_type);
                save_users_to_file("config.txt");
                sendto(sock_config, "Usuário adicionado.", strlen("Usuário adicionado."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
            } else if (strncmp(buffer, "DELETE_USER", 11) == 0) {
                // Receber o username do utilizador a ser removido
                char username_to_delete[BUF_SIZE];
                sscanf(buffer, "%*s %s", username_to_delete);
                delete_user(username_to_delete);
                sendto(sock_config, "Usuário removido.", strlen("Usuário removido."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
            } else if (strncmp(buffer, "LIST", 4) == 0) {
                list_users();
                sendto(sock_config, "Lista de usuários enviada.", strlen("Lista de usuários enviada."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
            } else if (strcmp(buffer, "QUIT_SERVER") == 0) {
                // Enviar mensagem de desligamento antes de fechar o servidor
                sendto(sock_config, "Servidor será desligado.", strlen("Servidor será desligado."), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
                printf("Servidor desligado pelo administrador.\n");
                running = 0; // Encerrar o loop
                break;
            } else {
                // Comando inválido
                sendto(sock_config, "Comando inválido", strlen("Comando inválido"), 0, (struct sockaddr *)&server_config_addr, server_config_addr_len);
            }
        }
    }
    
    // Fechar o socket antes de encerrar
    close(sock_config);
}

void add_user(const char *username, const char *password, const char *user_type) {
    for (int i = 0; i < NUM_USERS; i++) {
        if (strlen(shared_memory->users[i].username) == 0) {
            snprintf(shared_memory->users[i].username, BUF_SIZE, "%s", username);
            snprintf(shared_memory->users[i].password, BUF_SIZE, "%s", password);
            snprintf(shared_memory->users[i].user_type, BUF_SIZE, "%s", user_type);
            printf("Utilizador adicionado: %s\n", username);
            return;
        }
    }
    printf("Não foi possível adicionar o utilizador. Limite de utilizadores alcançado.\n");
}
void delete_user(const char *username) {
    for (int i = 0; i < NUM_USERS; i++) {
        if (strcmp(shared_memory->users[i].username, username) == 0) {
            // Encontrou o utilizador, remover da lista
            shared_memory->users[i] = (struct User){0}; // Limpa os dados do utilizador
            printf("Utilizador removido: %s\n", username);
            return;
        }
    }
    printf("Utilizador não encontrado: %s\n", username);
}
// Função para listar as turmas disponíveis
void list_classes(int client_fd) {
    char response[BUF_SIZE];
    strcpy(response, "CLASS: ");
    int first = 1;
    for (int i = 0; i < MAX_CLASSES; ++i) {
        if (strlen(shared_memory->classes[i].name) > 0) {
            if (!first) {
                strcat(response, ", ");
            }
            strcat(response, shared_memory->classes[i].name);
            strcat(response,"/");
            strcat(response, shared_memory->classes[i].multicast_address);
            first = 0;
        }
    }
    strcat(response, "\n FIM LISTA\n");
    write(client_fd, response, strlen(response));
}

void list_users() {
    printf("Lista de utilizadores:\n");
    for (int i = 0; i < NUM_USERS; i++) {
        if (strlen(shared_memory->users[i].username) > 0) {
            printf("Username: %s | Tipo: %s\n", shared_memory->users[i].username, shared_memory->users[i].user_type);
        }
    }
}

// Função para listar as turmas inscritas de um usuário e enviar para o cliente
void list_subscribed(int client_fd, const char *username, const char *password) {
    char response[BUF_SIZE] = "CLASS ";
    int i, j;
    int found = 0;

    // Procurar o usuário pelo username e password
    for (i = 0; i < NUM_USERS; i++) {
        if (strcmp(shared_memory->users[i].username, username) == 0 && strcmp(shared_memory->users[i].password, password) == 0) {
            for (j = 0; j < shared_memory->users[i].num_turmas; j++) {
                // Encontrar a turma correspondente na lista de classes
                for (int k = 0; k < MAX_CLASSES; k++) {
                    if (strcmp(shared_memory->users[i].turmas[j], shared_memory->classes[k].name) == 0) {
                        // Concatenar o nome e o endereço multicast da turma
                        strncat(response, "{", sizeof(response) - strlen(response) - 1);
                        strncat(response, shared_memory->classes[k].name, sizeof(response) - strlen(response) - 1);
                        strncat(response, "/", sizeof(response) - strlen(response) - 1);
                        strncat(response, shared_memory->classes[k].multicast_address, sizeof(response) - strlen(response) - 1);
                        strncat(response, "}, ", sizeof(response) - strlen(response) - 1);
                        break;
                    }
                }
            }
            found = 1;
            break;
        }
    }

    if (!found) {
        snprintf(response, sizeof(response), "Usuário não encontrado ou credenciais inválidas.\n");
    } else {
        // Remover a última vírgula e espaço, e adicionar um \n no final
        size_t len = strlen(response);
        if (len > 6) {  // 6 é o comprimento de "CLASS "
            response[len - 2] = '\n'; // Remove ", " e adiciona \n
            response[len - 1] = '\0'; // Garantir que a string termine corretamente
        } else {
            snprintf(response, sizeof(response), "Nenhuma turma inscrita.\n");
        }
    }

    // Enviar a resposta para o cliente
    if (write(client_fd, response, strlen(response)) < 0) {
        perror("Erro ao enviar resposta para o cliente");
    }
}
void save_users_to_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Erro ao abrir o ficheiro de utilizadores para escrita");
        return;
    }

    for (int i = 0; i < NUM_USERS; i++) {
        if (strlen(shared_memory->users[i].username) > 0) {
            fprintf(file, "%s;%s;%s\n", shared_memory->users[i].username, shared_memory->users[i].password, shared_memory->users[i].user_type);
        }
    }

    fclose(file);
    printf("Informações dos utilizadores foram salvas em %s\n", filename);
}

void load_users_from_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Erro ao abrir o ficheiro de utilizadores para leitura");
        return;
    }

    // Limpar os dados dos utilizadores antes de carregar do arquivo
    memset(users, 0, sizeof(users));

    char username[BUF_SIZE];
    char password[BUF_SIZE];
    char user_type[BUF_SIZE];
    char buffer[BUF_SIZE];
    int numusers=0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        // Remover a quebra de linha do final da string lida
        buffer[strcspn(buffer, "\n")] = '\0';

        // Separar os campos usando strtok
        char *token = strtok(buffer, ";");
        if (token == NULL) continue; // Tratar linhas vazias ou mal formatadas

        char username[BUF_SIZE], password[BUF_SIZE], user_type[BUF_SIZE];
        snprintf(username, sizeof(username), "%s", token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;

        snprintf(password, sizeof(password), "%s", token);

        token = strtok(NULL, ";");
        if (token == NULL) continue;

        snprintf(user_type, sizeof(user_type), "%s", token);
        sanitize_input(user_type);
        // Adicionar usuário com os campos extraídos
        add_user(username, password, user_type);
        printf("%s\n", username);
        printf("%s\n", password);
        printf("%s\n", user_type);
        numusers++;
    }

    fclose(file);
    printf("Informações dos utilizadores foram carregadas de %s\n",filename);
}

void trim(char *str) {
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  
        return;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
}

int authenticate_admin(const char *username, const char *password) {
    printf("Autenticando usuário: %s\n", username);
    printf("Autenticando password: %s\n", password);

    char trimmed_username[100];
    char trimmed_password[100];
    strncpy(trimmed_username, username, sizeof(trimmed_username) - 1);
    trimmed_username[sizeof(trimmed_username) - 1] = '\0';
    trim(trimmed_username);

    strncpy(trimmed_password, password, sizeof(trimmed_password) - 1);
    trimmed_password[sizeof(trimmed_password) - 1] = '\0';
    trim(trimmed_password);

    for (int i = 0; i < NUM_USERS; i++) {
        char temp_username[100];
        char temp_password[100];
        char temp_user_type[50];

        strncpy(temp_username, shared_memory->users[i].username, sizeof(temp_username) - 1);
        temp_username[sizeof(temp_username) - 1] = '\0';
        trim(temp_username);

        strncpy(temp_password, shared_memory->users[i].password, sizeof(temp_password) - 1);
        temp_password[sizeof(temp_password) - 1] = '\0';
        trim(temp_password);

        strncpy(temp_user_type, shared_memory->users[i].user_type, sizeof(temp_user_type) - 1);
        temp_user_type[sizeof(temp_user_type) - 1] = '\0';
        trim(temp_user_type);

        printf("Comparando com: %s %s %s\n", temp_username, temp_password, temp_user_type);

        if (strcmp(temp_username, trimmed_username) == 0 && strcmp(temp_password, trimmed_password) == 0) {
            printf("Encontrei com esse nome %s\n", trimmed_username);
            printf("user type: %s\n", temp_user_type);
            if (strcmp(temp_user_type, "administrador") == 0) {
                printf("ESTOU NO OK DA FUNÇÃO AUTENTICATE\n");
                printf("OK\n");
                return 1; // Autenticação bem-sucedida
            }
        }
    }
    printf("REJECTED\n");
    return 0; // Autenticação falhou
}

int subscribe_class(const char *class_name, const char *username) {
    // Encontrar a turma
    for (int i = 0; i < MAX_CLASSES; ++i) {
        if (strcmp(shared_memory->classes[i].name, class_name) == 0) {
            // Verificar se a turma está cheia
            if (shared_memory->classes[i].enrolled < shared_memory->classes[i].capacity) {
                // Inscrever na turma
                strcpy(shared_memory->classes[i].students[shared_memory->classes[i].enrolled], username);
                shared_memory->classes[i].enrolled++;
                
                // Atualizar a lista de turmas do usuário
                for (int j = 0; j < NUM_USERS; ++j) {
                    if (strcmp(shared_memory->users[j].username, username) == 0) {
                        strcpy(shared_memory->users[j].turmas[shared_memory->users[j].num_turmas], class_name);
                        shared_memory->users[j].num_turmas++;
                        return 1; // Inscrição bem-sucedida
                    }
                }
            } else {
                return 0; // Turma cheia
            }
        }
    }
    return 0; // Turma não encontrada
}

int create_class(const char *class_name, int class_size, char *multicast_address) {
    snprintf(multicast_address,MULTICAST_ADDRESS, "224.%d.%d.%d", rand() % 256, rand() % 256, rand() % 256);

    int i;
    for (i = 0; i < MAX_CLASSES; ++i) {
        if (strlen(shared_memory->classes[i].name) == 0) {
            break;
        }
    }

    if (i == MAX_CLASSES) {
        return 0; // Não há espaço para mais turmas
    }
    if (is_professor) {
        // Preencher os detalhes da nova turma
        strcpy(shared_memory->classes[i].name, class_name);
        shared_memory->classes[i].capacity = class_size;
        shared_memory->classes[i].enrolled = 0;
        strcpy(shared_memory->classes[i].multicast_address,multicast_address);
        return 1; // Envio bem-sucedido
    } else {
        return 0; // Cliente não é professor
    }
}

int multicast_send(const char *multicast_address, const char *content) {
    int sockfd;
    struct sockaddr_in multicast_addr;

    // Criar um socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return 0;
    }
    int ttl=MULTICAST_TTL;

    // Definir o TTL (Time-To-Live) para as mensagens multicast
    if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&ttl, sizeof(ttl)) < 0) {
        perror("setsockopt");
        close(sockfd);
        return 0;
    }

    // Configurar o endereço multicast
    memset(&multicast_addr, 0, sizeof(multicast_addr));
    multicast_addr.sin_family = AF_INET;
    multicast_addr.sin_addr.s_addr = inet_addr(multicast_address);
    multicast_addr.sin_port = htons(MULTICAST_PORT);

    // Enviar a mensagem para o grupo multicast
    if (sendto(sockfd, content, strlen(content), 0, (struct sockaddr*)&multicast_addr, sizeof(multicast_addr)) < 0) {
        perror("sendto");
        close(sockfd);
        return 0;
    }

    printf("Mensagem '%s' enviada para o endereço multicast '%s' na porta %d\n", content, multicast_address, MULTICAST_PORT);

    close(sockfd);
    return 1;
}

int send_content(const char *username, const char *class_name, const char *content) { 
    if (!is_professor(username)) {
        return 0; // Cliente não é professor
    }
    // Verificar se a turma existe
    for (int i = 0; i < MAX_CLASSES; ++i) {
        if (strcmp(shared_memory->classes[i].name, class_name) == 0) {
            // Obter o endereço multicast da turma
            const char *multicast_address = shared_memory->classes[i].multicast_address;

            // Enviar conteúdo para o grupo multicast
            int send_result = multicast_send(multicast_address, content);
            if (send_result) {
                printf("Conteúdo enviado para a turma '%s' no endereço multicast '%s': %s\n", class_name, multicast_address, content);
                return 1; // Envio bem-sucedido
            } else {
                printf("Falha ao enviar conteúdo para o endereço multicast '%s'.\n", multicast_address);
                return 0; // Falha no envio
            }
        }
    }
    return 0; // Turma não encontrada
}

void erro(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
