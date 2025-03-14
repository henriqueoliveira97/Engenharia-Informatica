       //////////////////////////////////////////////////
      ///  Autores: Daniel Pereira     2021237092    ///
     ///            Henrique Oliveira  2022211169   ///
    ///                                            ///
   ///  Titulo: Aplicacao de Gestao Hospitalar    ///
  ///                                            ///
 /// Projeto PPP 2023/2024                      ///
//////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> 
#include <ctype.h>
#include "projeto.h"


// Esta funcao garante que nao ha falhas na leitura de informacao
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Esta funcao valida se a data esta no formato correto (DD/MM/AAAA)
int validarData(const char *data){

    int dia,mes,ano;
    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3 || strlen(data)>10) {
        return 0; // Formato invalido
    }
    if(mes == 2 && dia > 29 ) return 0; // Excepcao do mes de fevereiro
    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1900 ||ano > 2024) { 
        return 0; 
    }
    return 1;
}

// Esta funcao valida o numero de cc esta no formato correto (aaaaaaaa-a-lla) onde a=algarismo e l=letra
int validarCC(const char *cc){
    
    // Verificar o tamanho
    if (strlen(cc) != 14) {
        return 0;
       
    }
    
    // Confirmar primeiros 8 algarismos
    for (int i = 0; i < 8; i++) {
        if (!isdigit(cc[i])) {
             
            return 0;
        }
    }

   // Confirmar primeira separacao
    if (cc[8] != '-') {
         
        return 0;
    }
    // Confirmar algarismo
    if (!isdigit(cc[9]) ) {
         
        return 0;
    }
    // Confirmar segundo separacao
    if (cc[10] != '-') {
        return 0;
    }
    // Confirmar as duas letras apos a segunta separacao
    for (int i = 11; i < 13; i++) {
        if (!isalpha(cc[i])) {
            return 0;
        }
    }

    // Confirmar ultimo algarismo
    if (!isdigit(cc[13])) {
        return 0;
    }

    
    return 1;


}

// Esta funcao valida o numero de telefone, pode conter apenas algarismos, espacos e o indicativo "+"
int validarTelefone(const char *telefone) {
    int tamanho = strlen(telefone);

    if(tamanho>20){
        return 0;
    }
    
    
    for (int i = 0; i < tamanho; i++) {
        if (telefone[i] != ' ' && !isdigit(telefone[i]) && telefone[i] != '+' ) {
            return 0;
        }
    }
    
    return 1; 
}

// Esta funcao valida email's. Os email's tem que conter um "@" algures no meio da string e um "." depois do "@" e antes do fim da string
int validarEMail(const char *mail){
    int tamanho = strlen(mail);
    int i = 0 ;

    for (int j = 0; j < tamanho; j++) {
        if (mail[j] == ' ') {
            return 0; // Se encontrar um espaco, retorna falso
        }
    }

    // Posicao do "@"
    while(i<tamanho && mail[i]!='@'){
        i++;
    }

    int count = 0;

    // Conta o numero de "." apos o "@"
    for(int k = i; k < tamanho ; k++){
        if (mail[k]=='.'){
            count++;
        }
    }

    
    if(!(count == 1) ){
        return 0;
    }

    int x = i;

    // Posicao do "."
    while(x<tamanho && mail[x]!='.'){
        x++;
    }

    // Confirmar se o "@" existe na string, mas que nao esta nem no inicio nem no fim. Confirmar se existe um "." algures apos o "@", mas que nao esta logo apos o "@" e que nao esta no fim 
    if(i>0 && i <tamanho -1 && x>i+1 && x<tamanho-1){
        return 1;
    }
    else return 0;

}

// Esta funcao serve para verificar se os valores fazem sentido. Por exemplo nao faz sentido ter um peso negativo
int valoresAjustados(int valor,int min,int max){
    if(valor>max || valor<=min){
        return 0;
    }
    else return 1;
}

// Esta funcao permite adicionar doentes a lista por ordem alfabetica
void adicionar_doente_lista(pListaDoentes *lista,struct Doente novoDoente){
    pListaDoentes novoNo = (pListaDoentes)malloc(sizeof(noListaDoentes));
    if(novoNo==NULL){
        printf("Erro ao alocar memoria.\n");
        exit(EXIT_FAILURE);
    }
    novoNo->pessoaLista = novoDoente;
    

    // Comparar nomes
    if ((*lista)->prox == NULL || strcmp((*lista)->pessoaLista.nome, novoDoente.nome) > 0) {
        novoNo->prox = *lista;
        *lista = novoNo;
    } else {
        // Procura a posicao correta para inserir o novo doente
        pListaDoentes atual = *lista;
        while (atual->prox != NULL && strcmp(atual->prox->pessoaLista.nome, novoDoente.nome) < 0) {
            atual = atual->prox;
        }
        novoNo->prox = atual->prox;
        atual->prox = novoNo;
    }
}

// Esta funcao permite ler o ficheiro "doentes.txt" e armazenar os doentes numa lista por ordem alfabetica
void ler_doentes(pListaDoentes *lista){
    FILE *arquivo = fopen("doentes.txt","r"); // Abertura do ficheiro no modo leitura
    if(arquivo==NULL){
        printf("Erro ao abrir o arquivo doentes.txt.\n");
        exit(EXIT_FAILURE);
    }
    struct Doente novoDoente; 
    while (fscanf(arquivo, "%d\n", &novoDoente.id) == 1) {
        fscanf(arquivo, "%[^\n]\n", novoDoente.nome);
        fscanf(arquivo, "%[^\n]\n", novoDoente.data_nascimento);
        fscanf(arquivo, "%[^\n]\n", novoDoente.cc);
        fscanf(arquivo, "%[^\n]\n", novoDoente.telefone);
        fscanf(arquivo, "%[^\n]\n", novoDoente.mail);

       
        adicionar_doente_lista(lista,novoDoente);
    }
     fclose(arquivo);
}

// Esta funcao permite adicionar registos a uma lista por ordem decrescente da tensao maxima
void adicionar_registo_lista(pListaRegistos *lista,struct Registo novoRegisto){
    pListaRegistos novoNo = (pListaRegistos) malloc(sizeof(noListaRegistos));

     if (novoNo == NULL) {
        printf("Erro ao alocar memoria.\n");
        return;
    }
    
    novoNo->registoLista = novoRegisto;

    // Compara as tensoes maximas
    if ((*lista)->prox == NULL || (*lista)->registoLista.tensao_max < novoRegisto.tensao_max) {
        novoNo->prox = *lista;
        *lista = novoNo;
    } else {

        // Procura a posicao correta para inserir o novo registo
        pListaRegistos atual = *lista;
        while (atual->prox != NULL && atual->prox->registoLista.tensao_max >= novoRegisto.tensao_max) {
            atual = atual->prox;
        }
        novoNo->prox = atual->prox;
        atual->prox = novoNo;
    }

}

// Esta funcao permite ler o ficheiro "registos.txt" e armazenar os registos numa lista ligada por ordem decrescente da tensao maxima
void ler_registos(pListaRegistos *lista){
    FILE *arquivo = fopen("registos.txt","r"); // Abertura do ficheiro no modo leitura
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo registos.txt.\n");
        exit(EXIT_FAILURE);
    }
    struct Registo novoRegisto;
    while (fscanf(arquivo, "%d\n", &novoRegisto.id) == 1) {
        char tensao_min_aux[10],tensao_max_aux[10],peso_aux[10],altura_aux[10];
        fscanf(arquivo, "%[^\n]\n", novoRegisto.data_registo);
        fscanf(arquivo, "%[^\n]\n", tensao_max_aux);
        fscanf(arquivo, "%[^\n]\n", tensao_min_aux);
        fscanf(arquivo, "%[^\n]\n", peso_aux);
        fscanf(arquivo, "%[^\n]\n", altura_aux);


        // Passagem para o tipo de variavel correto
        novoRegisto.tensao_max=strtol(tensao_max_aux,NULL,10);
        novoRegisto.tensao_min=strtol(tensao_min_aux,NULL,10);
        novoRegisto.peso=strtol(peso_aux,NULL,10);
        novoRegisto.altura=strtol(altura_aux,NULL,10);

        adicionar_registo_lista(lista,novoRegisto);
         
    }
    fclose(arquivo);
}

// Esta funcao serve para realizar a opcao 3 do menu que e listar doentes (id e nome) por ordem alfabetica
void listar_doentes( const pListaDoentes *lista){
    if((*lista)->prox == NULL){
        printf("A lista de doentes esta vazia.\n");
        return;
    }
    pListaDoentes atual = *lista;

    // Imprimir
    while (atual != NULL) {
        if(atual->pessoaLista.id>0){
            printf("ID: %d | Nome: %s\n", atual->pessoaLista.id, atual->pessoaLista.nome);
            
        }
        atual = atual->prox;
    }
}

// Esta funcao serve para realizar a opcao 4 do menu que e listar doentes por ordem decrescente da sua tensao maxima acima de um certo valor
void listar_doentes_tensao_maxima(const pListaRegistos *lista, const pListaDoentes *listaDoentes,int valor){
    if ((*lista)->prox == NULL) {
        printf("A lista de registos esta vazia.\n");
        return;
    }
    pListaRegistos registos_aux =*lista;
    int flag = 0; // Flag para confirmar se foram encontrados registos com tensao maxima superior ao valor pedido
    
    // Visto que a lista ja se encontra ordenada por ordem decrescente da tensao maxima e so necessario verificar se esta acima do valor pedido
    while(registos_aux != NULL){
        if (registos_aux->registoLista.tensao_max>valor){
            flag=1;
            pListaDoentes doentes_aux = *listaDoentes;
            while(doentes_aux!= NULL){
                if(doentes_aux->pessoaLista.id == registos_aux->registoLista.id){
                    printf("Nome: %s \n",doentes_aux->pessoaLista.nome);
                    printf("Tensao maxima: %d.\n\n",registos_aux->registoLista.tensao_max);
                    break;
                }
                doentes_aux=doentes_aux->prox;
            }
        }
        registos_aux=registos_aux->prox;
    }
    if(!flag) printf("Nao foi encontrado nenhum registo com tensao maxima superior a %d,\n",valor);
    
   
}

// Esta funcao serve para realizar a opcao 5 do menu que e apresentar toda a informacao de um doente
void apresentar_informacao_doente(const pListaRegistos *lista_registos, const pListaDoentes * lista_doentes , int id_doente){
    if (id_doente == 0){
         printf("Não foi encontrado nenhum doente com o id %d.\n",id_doente);
        return ;
    }
    pListaDoentes doente = *lista_doentes;
    bool flag = false; // Para verificar se foi encontrado o doente com o id dado
    while(doente != NULL){
        // Caso seja encontrado imprime todas as suas caracteristicas pessoais
        if(doente->pessoaLista.id == id_doente ){
            flag = true;
            printf("ID do doente : %d\n",doente->pessoaLista.id);
            printf("Nome : %s\n",doente->pessoaLista.nome);
            printf("Data de nascimento : %s\n",doente->pessoaLista.data_nascimento);
            printf("Numero do cartao de cidadao: %s\n",doente->pessoaLista.cc);
            printf("Numero de telefone : %s\n",doente->pessoaLista.telefone);
            printf("Email: %s\n",doente->pessoaLista.mail);
            
            
            break;
        }
        doente = doente->prox;
    }
    // Se nao for encontrado nenhum doente com esse id imprime um aviso e da return
    if (!flag) {
        printf("Não foi encontrado nenhum doente com o id %d.\n",id_doente);
        return;}
    // Se o doente foi encontrado verifica se existem registos associados a ele
    pListaRegistos registo_aux =*lista_registos;
    bool flag2 = false; // Serve para verificar se foi encontrado algum registo associado a esse doente
    while(registo_aux != NULL){
        // Sempre que for encontrado algum registo associado sera imprimido
        if(registo_aux->registoLista.id==id_doente){
            flag2=true;
            printf("-------------------------------------------\n");
            printf("Data do registo: %s\n",registo_aux->registoLista.data_registo);
            printf("Tensão maxima: %d\n",registo_aux->registoLista.tensao_max);
            printf("Tensão minima: %d\n",registo_aux->registoLista.tensao_min);
            printf("Peso: %d\n",registo_aux->registoLista.peso);
            printf("Altura: %d\n",registo_aux->registoLista.altura);
            
        }
         registo_aux = registo_aux->prox;
    }
    // Se nao for encontrado nenhum registo associado a esse doente imprime um aviso
    if(!flag2){
        printf("\nEste doente nao tem nenhum registo associado.\n");
    }


}

// Esta funcao serve para adicionar um novo doente ao arquivo de texto
void adicionar_doente_arquivo(struct Doente novoDoente){
    FILE *arquivo = fopen("doentes.txt","a"); //Abertura do ficheiro no modo de escrita com a flag "a"
    if(arquivo == NULL){
        printf("Erro ao abrir o arquivo doentes.txt.\n");
        exit(EXIT_FAILURE);
    }
    fprintf(arquivo, "%d\n", novoDoente.id);
    fprintf(arquivo, "%s\n", novoDoente.nome);
    fprintf(arquivo, "%s\n", novoDoente.data_nascimento);
    fprintf(arquivo, "%s\n", novoDoente.cc);
    fprintf(arquivo, "%s\n", novoDoente.telefone);
    fprintf(arquivo, "%s\n", novoDoente.mail);

    fclose(arquivo);
}

// Esta funcao serve para criar um novo id automaticamente, evitando assim id's repetidos
int gerar_novo_id(const pListaDoentes lista){
    int novo_id = 0;
    pListaDoentes atual = lista;
    // Vai percorrer a lista ate encontrar o maior id
    while (atual != NULL) {
        if (atual->pessoaLista.id > novo_id) {
            novo_id = atual->pessoaLista.id;
        }
        atual = atual->prox;
    }
    // Depois de o encontrar e incrementado em 1 unidade (se a lista estiver vazia o primeiro doente tera o id 1)
    return novo_id + 1;
}

// Esta funcao serve para realizar a opcao 1 do menu que e introduzir um novo doente (adiciona a lista e ao arquivo)
void introduzir_novo_doente(pListaDoentes *lista) {
    struct Doente novoDoente;
    
    char cc[tamanho_maximo];
    char telefone[tamanho_maximo];
    char email[tamanho_maximo];
    char data[tamanho_maximo];

    limparBuffer();


    printf("Nome: ");
    fgets(novoDoente.nome, tamanho_maximo, stdin);
    novoDoente.nome[strcspn(novoDoente.nome, "\n")] = '\0';
    
    
    do {
        printf("Data de nascimento (DD/MM/AAAA): ");
        fgets(data, tamanho_maximo, stdin);
        data[strcspn(data, "\n")] = '\0';
        //limparBuffer();
        if (!validarData(data)) {
            printf("Data de nascimento invalida. Por favor, insira novamente no formato dd/mm/aaaa.\n");
        }
    } while (!validarData(data));

    strcpy(novoDoente.data_nascimento,data);

     do {
        printf("Numero de cartão de cidadão: ");
        fgets(cc, tamanho_maximo, stdin);
        cc[strcspn(cc, "\n")] = '\0';
        if (!validarCC(cc)) {
            printf("CC invalido. Por favor, insira novamente no formato aaaaaaaa a lla.\n");
        }
    } while (!validarCC(cc));

    strcpy(novoDoente.cc,cc);

    do {
        printf("Telefone: ");
        fgets(telefone, tamanho_maximo, stdin);
        telefone[strcspn(telefone, "\n")] = '\0';
        if (!validarTelefone(telefone)) {
            printf("Numero de telefone invalido. Por favor, insira novamente sabendo que so pode ter algarismos e o indicativo +, e que so pode ter ate 20 caracteres.\n");
        }
    } while (!validarTelefone(telefone));
    strcpy(novoDoente.telefone,telefone);

     do {
        printf("Email: ");
        fgets(email, tamanho_maximo, stdin);
        email[strcspn(email, "\n")] = '\0'; 
        if (!validarEMail(email)) {
            printf("Email invalido. Por favor, insira novamente um Email valido.\n");
        }
    } while (!validarEMail(email));

    strcpy(novoDoente.mail,email);
    
    novoDoente.id = gerar_novo_id(*lista);
    printf("Foi atribuido ao doente o id %d.\n",novoDoente.id);
    
    adicionar_doente_arquivo(novoDoente);
    adicionar_doente_lista(lista, novoDoente);
    printf("Novo doente adicionado com sucesso!\n");

    
}

// Esta funcao serve para remover um doente do ficheiro de texto
void remover_doente_arquivos(int id) {
    FILE *arquivo = fopen("doentes.txt", "r"); // Abertura do ficheiro no modo leitura
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de doentes.\n");
        return;
    }

    // Arquivo temporario para armazenar os doentes que nao serao apagados
    FILE *arquivo_temporario = fopen("temp.txt", "w"); // Abertura do ficheiro no modo escrita
    if (arquivo_temporario == NULL) {
        printf("Erro ao criar arquivo temporario1.\n");
        fclose(arquivo);
        return;
    }

    char linha[tamanho_maximo];
    int id_atual;

    // Procura o doente a ser apagado
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (sscanf(linha, "%d", &id_atual) == 1 && id_atual != id) {
            
            fprintf(arquivo_temporario, "%s", linha);
            // Escreve as proximas 5 linhas do registo no arquivo temporario
            for (int i = 0; i < 5; i++) {
                fgets(linha, sizeof(linha), arquivo);
                fprintf(arquivo_temporario, "%s", linha);
            }
        } else {
            // Se corresponder avanca 5 linhas sem as copiar para o arquivo temporario
            for (int i = 0; i < 5; i++) {
                fgets(linha, sizeof(linha), arquivo);
            }
        }
    }

    fclose(arquivo);
    fclose(arquivo_temporario);

    remove("doentes.txt");
    rename("temp.txt","doentes.txt");

    FILE *arquivo2 = fopen("registos.txt","r");
    if(arquivo2 == NULL){
        printf("Erro ao abrir o arquivo de registos.\n");
        return;
    }

    FILE *arquivo_temporario2 = fopen("temp2.txt","w");
    if(arquivo_temporario2 == NULL){
        printf("Erro ao criar arquivo temporario2.\n");
        fclose(arquivo2);
        return;
    }
    // Procura os registos a serem apagados
    while(fgets(linha,sizeof(linha),arquivo2)){
        
        if (sscanf(linha, "%d", &id_atual) == 1 && id_atual != id) {
            fprintf( arquivo_temporario2, "%s", linha);
             // Escreve as proximas 5 linhas do registo no arquivo temporario
             for (int i = 0; i < 5; i++) {
                fgets(linha, sizeof(linha), arquivo2); 
                fprintf(arquivo_temporario2, "%s", linha);
            }
        } else {
            // Se corresponder avanca 5 linhas sem as copiar para o arquivo temporario
            for (int i = 0; i < 5; i++) {
                fgets(linha, sizeof(linha), arquivo2);
            }
        }
    }

    fclose(arquivo2);
    fclose(arquivo_temporario2);

    remove("registos.txt");
    rename("temp2.txt", "registos.txt");    
    

    
}

// Esta funcao serve para remover um doente da lista de doentes
void remover_doente_lista(int id,pListaDoentes *lista_doentes){

    pListaDoentes doente_atual = *lista_doentes;
    pListaDoentes doente_anterior = NULL;

    

    if(doente_atual == NULL){
        printf("A lista de doentes esta vazia.\n");
        return;
    }
    //percorrer lista a procura do id
    while(doente_atual !=NULL && doente_atual->pessoaLista.id != id ){
        doente_anterior = doente_atual;
        doente_atual = doente_atual->prox;
    }

    if (doente_atual == NULL){ // quer dizer que chegou ao fim da lista e nao encontrou o id
        printf("Não foi encontrado nenhum doente com o id %d na lista de doentes.\n",id);
        return;
    }
    if(doente_anterior == NULL){
        *lista_doentes = doente_atual->prox;
    }
    else{
        doente_anterior->prox = doente_atual->prox;
    }
    
    free(doente_atual);

}

// Esta funcao serve para remover todos os registos de um determinado doente
void remover_registos_lista(int id,pListaRegistos *lista_registos){
    pListaRegistos registo_atual = *lista_registos;
    pListaRegistos registo_anterior = NULL;

    if (registo_atual == NULL) {
        printf("A lista de registos esta vazia.\n");
        return;
    }

    while (registo_atual != NULL) {
        if (registo_atual->registoLista.id == id) {
            if (registo_anterior == NULL) {
                *lista_registos = registo_atual->prox;
                free(registo_atual);
                registo_atual = *lista_registos;
            } else {
                registo_anterior->prox = registo_atual->prox;
                free(registo_atual);
                registo_atual = registo_anterior->prox;
            }
        } else {
            registo_anterior = registo_atual;
            registo_atual = registo_atual->prox;
        }
    }
}

// Esta funcao serve para realizar a opcao 2 do menu que e remover toda a informacao guardada de um determinado doente
void remover_doente(int id,pListaDoentes *lista_doentes,pListaRegistos *lista_registos){
    pListaDoentes doente_atual = *lista_doentes;
    bool doente_encontrado = false;
    // Verifica se exite um doente com esse id
    while (doente_atual != NULL) {
        if (doente_atual->pessoaLista.id == id) {
            doente_encontrado = true;
            break;
        }
        doente_atual = doente_atual->prox;
    }
    // Se nao existir um doente com esse id da um aviso e da return
    if (!doente_encontrado || id == 0) {
        printf("Não foi encontrado nenhum doente com o id %d.\n",id);
        return;
    }
    else{
        remover_doente_arquivos(id);
        remover_doente_lista(id,lista_doentes);
        remover_registos_lista(id,lista_registos);
    }

}

// Esta funcao serve para realizar a opcao 6 do menu que e adicionar um novo registo a um determinado doente
void inserir_registo(const pListaDoentes *lista_doentes,pListaRegistos *lista_registos, int id){
    pListaDoentes doente_atual = *lista_doentes;
    bool doente_encontrado = false;

    // Verifica se exite um doente com esse id
    while (doente_atual != NULL) {
        if (doente_atual->pessoaLista.id == id) {
            doente_encontrado = true;
            break;
        }
        doente_atual = doente_atual->prox;
    }
    // Se nao existir um doente com esse id da um avisa e da return
    if (!doente_encontrado || id == 0) {
        printf("Não foi encontrado nenhum doente com o id %d.\n",id);
        return;
    }
    // Caso exista, vai ser pedida toda a informacao necessaria para o registo
    struct Registo novoRegisto;

    do {
        printf("Insira a data do registo (DD/MM/AAAA): ");
        scanf("%s", novoRegisto.data_registo);
        limparBuffer();
        if (!validarData(novoRegisto.data_registo)) {
            printf("Data do registo invalida. Por favor, insira novamente no formato dd/mm/aaaa.\n");
        }
    } while (!validarData(novoRegisto.data_registo));

    int flag;

    do {
        printf("Insira a tensão maxima: ");
        flag = 0;
        if (scanf("%d", &novoRegisto.tensao_max) != 1) {
            printf("Entrada invalida. Por favor, insira um numero.\n");
            limparBuffer(); // Limpa o buffer de entrada
            flag = 1;
            continue;
        }
        if (!valoresAjustados(novoRegisto.tensao_max, 0, 200)) {
            printf("Insira uma tensão maxima adequada (0-200).\n");
            flag = 1;
        }
    } while (flag);

    do {
        printf("Insira a tensão minima: ");
        flag = 0; // Declaracao e inicializacao da variavel flag dentro do loop
        if (scanf("%d", &novoRegisto.tensao_min) != 1) {
            printf("Entrada invalida. Por favor, insira um numero.\n");
            limparBuffer(); // Limpa o buffer de entrada
            flag = 1;
            continue;
        }
        // Verifica se a tensão minima esta no intervalo adequado
        if (!valoresAjustados(novoRegisto.tensao_min, 0, 200)) {
            printf("Insira uma tensão minima adequada (0-200).\n");
            flag = 1;
        }
    } while (flag); // Condicao de repeticao do loop


    do {
        printf("Insira o peso: ");
        flag = 0; // Declaracao e inicializacao da variavel flag dentro do loop
        if (scanf("%d", &novoRegisto.peso) != 1) {
            printf("Entrada invalida. Por favor, insira um numero.\n");
            limparBuffer(); // Limpa o buffer de entrada
            flag = 1;
            continue;
        }
        // Verifica se o peso esta no intervalo adequado
        if (!valoresAjustados(novoRegisto.peso, 0, 500)) {
            printf("Insira um peso adequado (0-500).\n");
            flag = 1;
        }
    } while (flag); 
    limparBuffer();
    do {
        printf("Insira a altura em centimetros: ");
        flag = 0; // Declaracao e inicializacao da variavel flag dentro do loop
        if (scanf("%d", &novoRegisto.altura) != 1) {
            printf("Entrada invalida. Por favor, insira um numero.\n");
            limparBuffer(); // Limpa o buffer de entrada
            flag = 1;
            continue;
        }
        // Verifica se a altura esta no intervalo adequado
        if (!valoresAjustados(novoRegisto.altura, 0, 280 )) {
            printf("Insira uma altura adequada (0-280).\n");
            flag = 1;
        }
    } while (flag); 

    
    novoRegisto.id = id;
    adicionar_registo_lista(lista_registos,novoRegisto);

    FILE *arquivo = fopen("registos.txt", "a"); //Abertura do ficheiro no modo de escrita com a flag "a"
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de registos.\n");
        return;
    }
    fprintf(arquivo, "%d\n", novoRegisto.id);
    fprintf(arquivo, "%s\n", novoRegisto.data_registo);
    fprintf(arquivo, "%d\n", novoRegisto.tensao_max);
    fprintf(arquivo, "%d\n", novoRegisto.tensao_min);
    fprintf(arquivo, "%d\n", novoRegisto.peso);
    fprintf(arquivo, "%d\n", novoRegisto.altura);
    fclose(arquivo);
}

// Esta funcao serve para, quando o programa estiver para terminar, liberta toda a memoria alocada para a lista de registos
void libertar_lista_registos(pListaRegistos *lista) {
    pListaRegistos atual = *lista;
    pListaRegistos proximo;

    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }

    *lista = NULL;
}

// Esta funcao serve para, quando o programa estiver para terminar, liberta toda a memoria alocada para a lista de doentes
void libertar_lista_doentes(pListaDoentes *lista) {
    pListaDoentes atual = *lista;
    pListaDoentes proximo;

    while (atual != NULL) {
        proximo = atual->prox;
        free(atual);
        atual = proximo;
    }

    *lista = NULL;
}
// Funcao main 
int main(){

    // Sao criadas as listas ligadas
    pListaDoentes listaDoentes = cria_doentes();
    pListaRegistos listaRegistos = cria_registos();
    
    // Sao lidos os ficheiros
    ler_doentes(&listaDoentes);
    ler_registos(&listaRegistos);

    
    // Menu que apresenta todas as opcoes. E usado um switch para realizar as operacoes
    printf("Bem-vindo ao portal de saude.\n");
    while(true){
        int resposta;
        printf("\nOpcoes:\n\n"
               "1-Introduzir um novo doente\n"
               "2-Eliminar um doente\n"
               "3-Listar doentes por ordem alfabetica\n"
               "4-Listar doentes com tensoes maximas acima de um valor\n"
               "5-Apresentar toda a informacao de um doente\n"
               "6-Inserir tensoes, peso e altura de um doente num determinado dia\n"
               "7-Sair\n\n"
               "O que pretende efetuar? ");


        // Garantir que introduziu um numero
        if (scanf("%d", &resposta) != 1) {
            
            while (getchar() != '\n');
            printf("Entrada invalida. Por favor, insira um numero.\n\n");
            continue;
        }


        if(resposta >= 1 && resposta <= 7) {
            printf("\n");
            switch(resposta) {
                case 1:
                    printf("Inserir novo doente\n\n");
                    introduzir_novo_doente(&listaDoentes);
                    
                    break;
                case 2:
                    printf("Eliminar doente\n\n");
                    int id;
                    printf("Insira o id do doente a eliminar: ");
                    if (scanf("%d", &id) != 1) {
                        while (getchar() != '\n');
                        printf("Entrada invalida.\n\n");
                        continue;
                    }
                    remover_doente(id,&listaDoentes,&listaRegistos);
                    
                    break;
                case 3:
                    printf("Listar doentes\n\n");
                    listar_doentes(&listaDoentes);
                    
                    break;
                case 4:
                    int valor;
                    printf("Listar doentes com tensoes maximas acima de um valor\n\n");
                    printf("Quer listar tensoes maximas a cima de que valor? ");
                    if (scanf("%d", &valor) != 1) {
                        while (getchar() != '\n');
                        printf("Entrada invalida.\n\n");
                        continue;
                    }
                    listar_doentes_tensao_maxima(&listaRegistos, &listaDoentes,valor);
                    
                    break;
                case 5: 
                    printf("Apresentar toda a informacao de um doente\n\n");
                    int id_doente;
                    printf("Insira o id do doente que quer pesquisar: ");
                   if (scanf("%d", &id_doente) != 1) {
                        while (getchar() != '\n');
                        printf("Entrada invalida.\n\n");
                        continue;
                    }
                    apresentar_informacao_doente(&listaRegistos ,&listaDoentes ,id_doente);

                    
                    break;
                case 6:
                    printf("Inserir tensoes, peso e altura de um doente num determinado dia\n\n");
                    printf("Insira o id do doente:");
                    if (scanf("%d", &id) != 1) {
                        while (getchar() != '\n');
                        printf("Entrada invalida.\n\n");
                        continue;
                    }
                    inserir_registo(&listaDoentes, &listaRegistos, id);
                    break;
                case 7:
                    libertar_lista_registos(&listaRegistos);
                    libertar_lista_doentes(&listaDoentes);
                    printf("Adeus!\n");
                    return 0; 
            }


        // Se o numero que introduziu nao estiver na lista de opcoes
        }else {
            printf("Opcao invalida. Por favor, escolha uma opcao valida.\n\n");  
        }

    } 
}
