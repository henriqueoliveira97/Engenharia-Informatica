#ifndef GESTAO_DOENTES_H
#define GESTAO_DOENTES_H
#define tamanho_maximo 50

struct Doente{
    int id;
    char nome[tamanho_maximo];
    char data_nascimento[11];
    char cc[15];
    char telefone[tamanho_maximo];
    char mail[tamanho_maximo];
};

typedef struct noListaDoentes{
    struct Doente pessoaLista;
    struct noListaDoentes *prox;
}noListaDoentes;

struct Registo{
    int id;
    char data_registo[11];
    int tensao_max;
    int tensao_min;
    int peso;
    int altura;
};


typedef struct noListaRegistos{
    struct Registo registoLista;
    struct noListaRegistos *prox;
}noListaRegistos;



typedef noListaDoentes *pListaDoentes;
typedef noListaRegistos *pListaRegistos;

pListaDoentes cria_doentes(){
    pListaDoentes aux;
    struct Doente d1 = {0,"","","","",""};
    aux = (pListaDoentes) malloc (sizeof(noListaDoentes));
    if(aux!=NULL){
        aux->pessoaLista = d1;
        aux->prox = NULL;
    }
    return aux;
}
pListaRegistos cria_registos(){
    pListaRegistos aux;
    struct Registo r1 = {0,"",0,0,0,0.0};
    aux = (pListaRegistos) malloc (sizeof(noListaRegistos));
    if(aux!=NULL){
        aux->registoLista = r1;
        aux->prox = NULL;
    }
    return aux;
}

void limparBuffer();
int validarData(const char *data);
int validarCC(const char *cc);
int validarTelefone(const char *telefone);
int validarEMail(const char *mail);
int valoresAjustados(int valor,int min,int max);
void adicionar_doente_lista(pListaDoentes *lista,struct Doente novoDoente);
void ler_doentes(pListaDoentes *lista);
void adicionar_registo_lista(pListaRegistos *lista,struct Registo novoRegisto);
void ler_registos(pListaRegistos *lista);
void listar_doentes( const pListaDoentes *lista);
void listar_doentes_tensao_maxima(const pListaRegistos *lista, const pListaDoentes *listaDoentes,int valor);
void apresentar_informacao_doente(const pListaRegistos *lista_registos, const pListaDoentes * lista_doentes , int id_doente);
void adicionar_doente_arquivo(struct Doente novoDoente);
int gerar_novo_id(const pListaDoentes lista);
void introduzir_novo_doente(pListaDoentes *lista);
void remover_doente_arquivos(int id);
void remover_doente_lista(int id,pListaDoentes *lista_doentes);
void remover_registos_lista(int id,pListaRegistos *lista_registos);
void remover_doente(int id,pListaDoentes *lista_doentes,pListaRegistos *lista_registos);
void inserir_registo(const pListaDoentes *lista_doentes,pListaRegistos *lista_registos, int id);
void libertar_lista_registos(pListaRegistos *lista);
void libertar_lista_doentes(pListaDoentes *lista);

#endif