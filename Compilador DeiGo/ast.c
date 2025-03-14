#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"
#include "semantics.h"

int call_flag = 0;
int flag_undef = 0;

extern struct scope_list *global_symbol_table;

// create a node of a given category with a given lexical symbol
struct node *newnode(enum category category, char *token) {
    struct node *new = malloc(sizeof(struct node));
    new->category = category;
    new->token = token;
    new->type = no_type;
    new->children = malloc(sizeof(struct node_list));
    new->children->node = NULL;
    new->children->next = NULL;
    return new;
}

// Conta o número de filhos de um nó
int childcount(struct node *parent) {
    if (parent == NULL || parent->children == NULL) return 0;

    int count = 0;
    struct node_list *child = parent->children->next;
    while (child != NULL) {
        count++;
        child = child->next;
    }
    return count;
}


// get a pointer to a specific child, numbered 0, 1, 2, ...
struct node *getchild(struct node *parent, int position) {
    struct node_list *children = parent->children;
    while((children = children->next) != NULL)
        if(position-- == 0)
            return children->node;
    return NULL;
}

int countchildren(struct node *node) {
    int i = 0;
    while(getchild(node, i) != NULL)
        i++;
    return i;
}

struct node* removeblock(struct node* block) {
    if (block == NULL || childcount(block) == 0) return NULL;

    struct node_list *child = block->children->next;  
    struct node* child_node = child->node;          
    free(block->children);                         
    free(block);                                     
    return child_node;                                
}

void add_to_node_list(struct node_list *list, struct node *new_node) {
    // Aloca memória para um novo nó da lista
    struct node_list *new_entry = malloc(sizeof(struct node_list));
    new_entry->node = new_node;
    new_entry->next = NULL;

    // Se a lista estiver vazia, o novo nó será o primeiro
    if (list->next == NULL) {
        list->next = new_entry;
        list->head = new_entry;  // Armazena o primeiro nó na "head"
    } else {
        // Caso contrário, adiciona o nó no final da lista
        struct node_list *current = list;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_entry;
    }
}



// Inicializa a lista global para armazenar VarDecls temporários
void init_global_list(struct node_list *list) {
    list->node = NULL;
    list->next = NULL;
    list->head = NULL;
}

// Limpa a lista de nós global
void clear_node_list(struct node_list *list) {
    struct node_list *current = list->next;
    struct node_list *temp;
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    list->next = NULL;
}

void addchild(struct node *parent, struct node *child) {
    // Aloca memória para um novo nó da lista de filhos
    struct node_list *new = malloc(sizeof(struct node_list));
    if (new == NULL) {
        fprintf(stderr, "Erro ao alocar memória para o filho.\n");
        exit(1);
    }

    // Inicializa o novo nó com o filho
    new->node = child;
    new->next = NULL;

    // Verifica se a lista de filhos do pai está vazia
    if (parent->children == NULL) {
        // Se a lista de filhos estiver vazia, o novo nó se torna o primeiro filho
        parent->children = new;
    } else {
        // Caso contrário, encontra o último filho e o adiciona à lista
        struct node_list *children = parent->children;
        while (children->next != NULL) {
            children = children->next;
        }
        children->next = new;
    }
}

char* get_category_name(enum category cat) {
    switch (cat) {
        case Program: return "Program";

        case VarDeclaration: return "VarDecl";

        case Integer: return "Int";
        case Float_32: return "Float32";
        case Bool: return "Bool";
        case String: return "String";
        case Natural: return "Natural";
        case Decimal: return "Decimal";
        case Identifier: return "Identifier";
        case StrLit: return "StrLit";

        case FuncDeclaration: return "FuncDecl";
        case FuncHeader: return "FuncHeader";;
        case FuncParams: return "FuncParams";
        case FuncBody: return "FuncBody";
        case ParamDecl: return "ParamDecl";

        case If: return "If";
        case For: return "For";
        case Return: return "Return";
        case Call: return "Call";
        case Block: return "Block";
        case Print: return "Print";
        case ParseArgs: return "ParseArgs";

        case Or: return "Or";
        case And: return "And";
        case Equal: return "Eq";
        case NotEqual: return "Ne";
        case LessThan: return "Lt";
        case GreaterThan: return "Gt";
        case LessEqual: return "Le";
        case GreaterEqual: return "Ge";
        case Add: return "Add";
        case Sub: return "Sub";
        case Mul: return "Mul";
        case Div: return "Div";
        case Mod: return "Mod";
        case Not: return "Not";
        case UnaryMinus: return "Minus";
        case UnaryPlus: return "Plus";
        case Assign: return "Assign";


        
        default: return "Unknown";
    }
}


void show_s(struct node *node, int depth) {
    if (node == NULL) return;
    
    char* category = get_category_name(node->category);

    if (strcmp(category, "Unknown") != 0) {
        for (int i = 0; i < depth; i++) printf("..");

        if (node->token == NULL) {
            if (node->type != no_type) {
                if (node->category == Call) {
                    call_flag = 1;
                }

                printf("%s - %s\n", category, type_name(node->type));
            } else {
                if (node->category == Call) {
                    call_flag = 1;
                }
                printf("%s\n", category);
            }
        } else {
            if (node->type != no_type) {
                printf("%s(%s) - %s\n", category, node->token, type_name(node->type));

            } else {
                if (call_flag) {
                    printf("%s(%s)", category, node->token);

                    // Verificar na tabela de símbolos globais
                    struct scope_list *symbol = global_symbol_table->head;
                    while (symbol != NULL) {
                        if (strcmp(symbol->identifier, node->token) == 0) {
                            // Encontrou o identificador correspondente
                            flag_undef = 0;
                            struct param_list *params = symbol->params->next;
                            printf(" - (");
                            int first = 1;
                            while (params != NULL) {
                                if (!first) printf(",");
                                printf("%s", type_name(params->type));
                                first = 0;
                                params = params->next;
                            }
                            printf(")\n");
                            break;
                        }
                        symbol = symbol->next;
                    }
                    if (symbol == NULL) {
                        printf("\n");
                    }
                    call_flag=0;
                }
                else printf("%s(%s)\n", category, node->token);
            }
        }  
    }

    

    struct node_list *child = node->children->next;
    
    while (child != NULL) {
        show_s(child->node, depth + 1);
        child = child->next;
    }
}

void show_t(struct node *node, int depth) {
    if (node == NULL) return;

    
    char* category = get_category_name(node->category);

    if (strcmp(category, "Unknown") != 0) {
        for (int i = 0; i < depth; i++) printf("..");
        printf("%s", get_category_name(node->category));
        if (node->token != NULL) printf("(%s)", node->token);
        printf("\n");
    }
    

    struct node_list *child = node->children->next;
    while (child != NULL) {
        show_t(child->node, depth + 1);
        child = child->next;
    }
}