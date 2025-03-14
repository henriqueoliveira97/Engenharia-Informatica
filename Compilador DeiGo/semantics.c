#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantics.h"

// o scope é o "ambiente" ou "região" onde os nomes declarados estão visíveis e acessíveis.
//Evita que nomes de variáveis e funções se "choquem" ao limitar onde eles podem ser acessados.
//Permite reusar nomes de variáveis sem conflitos, desde que em escopos diferentes.

int semantic_errors = 0;

struct scope_list *global_symbol_table;
void check_var_declaration(struct node *var_decl, struct scope_list *scope);
void check_expression(struct node *expression, struct scope_list *scope);

char* operator_name(enum category category) {
    switch (category) {
        case Add: return "+";
        case Sub: return "-";
        case Mul: return "*";
        case Div: return "/";
        case Mod: return "%";
        case Or: return "||";
        case And: return "&&";
        case LessThan: return "<";
        case GreaterThan: return ">";
        case LessEqual: return "<=";
        case GreaterEqual: return ">=";
        case Equal: return "==";
        case Assign: return "=";
        case NotEqual: return "!=";
        case Not: return "!";
        case UnaryMinus: return "-";
        case UnaryPlus: return "+";
        default: return "unknown";
    }
}

void check_parameters(struct node *parameters, struct scope_list *scope) {
    if (parameters == NULL || parameters->children == NULL) {
        return; // Evita acessar filhos inexistentes.
    }

    struct node_list *parameter = parameters->children;
    while (parameter != NULL) {
        struct node *param_node = parameter->node;
        if (param_node == NULL) {
            parameter = parameter->next;
            continue; // Ignorar nós nulos.
        }

        struct node *idnt = getchild(param_node, 1); // Identificador está na posição 1.
        struct node *type_node = getchild(param_node, 0); // Tipo está na posição 0.

        if (idnt == NULL || type_node == NULL) {
            parameter = parameter->next;
            continue; // Ignorar parâmetros inválidos.
        }

        enum type type = category_type(type_node->category);
        if (search_symbol(scope->list, idnt->token) == NULL) {
            insert_symbol(scope->list, idnt->token, type, param_node);
        } else {
            printf("Line %d, column %d: Symbol %s already defined\n", 
                    idnt->token_line, idnt->token_column, idnt->token);
            semantic_errors++;
        }
    
        parameter = parameter->next;
    }
}

void check_func_invocation(struct node *call, struct scope_list *scope) {
    if (call == NULL || call->children->next == NULL) {
        semantic_errors++;
        call->type = undef; 
        return;
    }

    struct node *identifier_node = getchild(call, 0);
    if (identifier_node == NULL || identifier_node->token == NULL) {
        semantic_errors++;
        call->type = undef;
        return;
    }

    // Verifica os argumentos
    struct node_list *passed_args = call->children->next->next;
    while (passed_args != NULL) {
        struct node *arg_node = passed_args->node;
        if (arg_node == NULL) break;
        check_expression(arg_node, scope);  // Verifica o tipo
        passed_args = passed_args->next;
    }

    // Busca a função na tabela de símbolos global
    struct symbol_list *func_symbol = search_symbol(global_symbol_table->head->list, identifier_node->token);
    if (func_symbol == NULL) {

        // Obter os argumentos passados para construir a mensagem com o tipo
        struct node_list *passed_args = call->children->next->next;
        printf("Line %d, column %d: Cannot find symbol %s(",
               call->token_line, call->token_column, identifier_node->token);
               call->type = undef;
               identifier_node->type = undef;

        int first = 1;
        while (passed_args != NULL) {
            struct node *arg_node = passed_args->node;
            if (arg_node == NULL) break;
            if (!first) printf(",");
            printf("%s", type_name(arg_node->type)); 
            first = 0;

            passed_args = passed_args->next;
        }
        printf(")\n");

        semantic_errors++;
        return;
    }

    if (func_symbol->node->category != FuncDeclaration) {
        semantic_errors++;
        call->type = undef;
        return;
    }

    // Obtém os parâmetros da função a partir do scope
    struct scope_list *scope_function = global_symbol_table->head->next;
    while(scope_function != NULL){

        if (strcmp(scope_function->identifier, identifier_node->token) == 0) break;
        scope_function = scope_function->next;
    }
    if (scope_function == NULL) {
        printf("Line %d, column %d: Cannot find symbol %s\n", 
                identifier_node->token_line, identifier_node->token_column, identifier_node->token);
        semantic_errors++;
        call->type = undef;
        identifier_node->type = undef;
    }
    
    struct param_list *declared_params = scope_function->params->next;

    int i = 1; // Começa a partir do segundo filho, já que o primeiro é o identificador

    while (declared_params != NULL) {
        struct node *arg_node = getchild(call, i);  // Obtém o argumento i-ésimo
        struct param_list *param_node = declared_params;

        if (arg_node == NULL || param_node == NULL) {
            semantic_errors++;
            continue;
        }

        enum type param_type = param_node->type;

        if (arg_node->type != param_type) {
            
            // Obter os argumentos passados para construir a mensagem com o tipo
            struct node_list *passed_args = call->children->next->next;
            printf("Line %d, column %d: Cannot find symbol %s(",
                call->token_line, call->token_column, identifier_node->token);
                call->type = undef;
                identifier_node->type = undef;

            int first = 1;
            while (passed_args != NULL) {
                struct node *arg_node = passed_args->node;
                if (arg_node == NULL) break;

                if (!first) printf(",");
                printf("%s", type_name(arg_node->type));
                first = 0;

                passed_args = passed_args->next;
            }
            printf(")\n");

            semantic_errors++;
            return;
        }

        i++; 
        declared_params = declared_params->next;
    }

    // Verifica se há um número incorreto de argumentos
    if (getchild(call, i) != NULL || declared_params != NULL) {

       // Obter os argumentos passados para construir a mensagem com o tipo
        struct node_list *passed_args = call->children->next->next;
        printf("Line %d, column %d: Cannot find symbol %s(",
               call->token_line, call->token_column, identifier_node->token);
               call->type = undef;  // Tipo permanece indefinido
               identifier_node->type = undef;

        int first = 1;
        while (passed_args != NULL) {
            struct node *arg_node = passed_args->node;
            if (arg_node == NULL) break;

            if (!first) printf(",");
            printf("%s", type_name(arg_node->type));
            first = 0;

            passed_args = passed_args->next;
        }
        printf(")\n");

        semantic_errors++;
        return;
    }

    // Define o tipo da call como o tipo de retorno da função
    struct node *func_header = getchild(func_symbol->node, 0);
    struct node *return_type_node = getchild(func_header, 1);
    call->type = category_type(return_type_node->category);
}


void check_pars_args(struct node *parsarg, struct scope_list *scope){
    if(parsarg==NULL) return;

    struct node *var_node= getchild(parsarg,0);
    if(var_node== NULL || var_node->token==NULL){
        printf("ParseArgs invalid!");
        semantic_errors++;
        return;
    }
    struct symbol_list *var_symbol = search_symbol(scope->list, var_node->token);
    if(var_symbol == NULL){
        printf("Line %d, column %d: Cannot find symbol %s\n", 
                var_node->token_line, var_node->token_column, var_node->token);
        semantic_errors++;
    }else if (var_symbol->type != integer_type) {
        semantic_errors++;
    } else {
        var_node->type = var_symbol->type;
    }

    struct node *expr_node=getchild(parsarg,1);
    if(expr_node == NULL){
        printf("Invalid parseargs...");
        semantic_errors++;
        return;
    }
    check_expression(expr_node,scope);
    if (var_node->type != integer_type || expr_node->type != integer_type) {
        printf("Line %d, column %d: Operator strconv.Atoi cannot be applied to types %s, %s\n",
               parsarg->token_line, parsarg->token_column,
               type_name(var_symbol->type),
               type_name(expr_node->type));
        semantic_errors++;
        return;
    }
    parsarg->type = integer_type;
}

void check_expression(struct node *expression, struct scope_list *scope) {
    switch (expression->category) {
        case Identifier:
            if (search_symbol(scope->list, expression->token) == NULL && 
                search_symbol(global_symbol_table->head->list, expression->token) == NULL) {
                printf("Line %d, column %d: Cannot find symbol %s\n", 
                        expression->token_line, expression->token_column, expression->token);
                semantic_errors++;
                expression->type = undef;
            } else {
                struct symbol_list *symbol = search_symbol(scope->list, expression->token);
                if (!symbol) {
                    symbol = search_symbol(global_symbol_table->head->list, expression->token);
                    
                    if (symbol != NULL ){
                        if (symbol->node->category == FuncDeclaration){
                            printf("Line %d, column %d: Cannot find symbol %s\n", 
                                expression->token_line, expression->token_column, expression->token);
                            semantic_errors++;
                            expression->type = undef;
                            break;
                        }
                    }
                }
                expression->type = symbol->type;
            }
            break;

        case Natural:
            expression->type = integer_type;
            break;

        case Decimal:
            expression->type = float32_type;
            break;

        case Add:
        case Sub:
        case Mul:
        case Div:
        case Mod: {
            struct node *left = getchild(expression, 0);
            struct node *right = getchild(expression, 1);

            check_expression(left, scope);
            check_expression(right, scope);
            

            if ((left->type != integer_type && left->type != float32_type) || 
                (right->type != integer_type && right->type != float32_type)) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                    expression->token_line, expression->token_column,
                    operator_name(expression->category),
                    type_name(left->type), type_name(right->type));
                semantic_errors++;
                expression->type = undef;
                break;
            }

            else if (left->type != right->type) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                    expression->token_line, expression->token_column,
                    operator_name(expression->category),
                    type_name(left->type), type_name(right->type));
                semantic_errors++;
                expression->type = undef;
                break;
            }

            expression->type = (left->type == float32_type || right->type == float32_type) 
                            ? float32_type : integer_type;
            break;
        }

        case Or:
        case And: {
            struct node *left = getchild(expression, 0);
            struct node *right = getchild(expression, 1);

            check_expression(left, scope);
            check_expression(right, scope);

            if (left->type != bool_type || right->type != bool_type) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                       expression->token_line, expression->token_column,
                       operator_name(expression->category),
                       type_name(left->type), type_name(right->type));
                semantic_errors++;
                expression->type = undef;
            }
            expression->type = bool_type;
            break;
        }

        case LessThan:
        case GreaterThan:
        case LessEqual:
        case GreaterEqual: {

            struct node *left = getchild(expression, 0);
            struct node *right = getchild(expression, 1);

            check_expression(left, scope);
            check_expression(right, scope);

            if (left->type != right->type) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                       expression->token_line, expression->token_column,
                       operator_name(expression->category),
                       type_name(left->type), type_name(right->type));
                semantic_errors++;
                expression->type = undef;
            }

            else if ((left->type != integer_type && left->type != float32_type) || (right->type != integer_type && right->type != float32_type)) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                       expression->token_line, expression->token_column,
                       operator_name(expression->category),
                       type_name(left->type), type_name(right->type));
                semantic_errors++;
                expression->type = undef;
            }

            expression->type = bool_type;
            break;
        }
           
        case Equal:
        case NotEqual: {
            struct node *left = getchild(expression, 0);
            struct node *right = getchild(expression, 1);

            check_expression(left, scope);
            check_expression(right, scope);

            if (left->type != right->type) {
                printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                       expression->token_line, expression->token_column,
                       operator_name(expression->category),
                       type_name(left->type), type_name(right->type));
                semantic_errors++;
                expression->type = undef;
            }

            expression->type = bool_type;
            break;
        }

        case Not: {
            struct node *child = getchild(expression, 0);
            check_expression(child, scope);

            if (child->type != bool_type) {
                printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",
                       expression->token_line, expression->token_column,
                       operator_name(expression->category),
                       type_name(child->type));
                semantic_errors++;
                expression->type = undef; 
            }
            expression->type = bool_type;
            break;
        }

        case UnaryMinus:
        case UnaryPlus: {
            struct node *child = getchild(expression, 0);
            check_expression(child, scope);

            if (child->type != integer_type && child->type != float32_type) {
                printf("Line %d, column %d: Operator %s cannot be applied to type %s\n",
                       expression->token_line, expression->token_column,
                       operator_name(expression->category),
                       type_name(child->type));
                semantic_errors++;
                expression->type = undef; 
            }
            expression->type = child->type;
            break;
        }

        case Call:
            check_func_invocation(expression, scope);
            break;

        default:
            break;
    }
}

void check_statement(struct node *statement, struct scope_list *scope){
    switch(statement->category){
        case Assign: {
            struct node *id_node = getchild(statement, 0); // Identificador à esquerda
            struct node *expr = getchild(statement, 1);    // Expressão à direita

            // Verificar e anotar a expressão do lado direito
            if (expr != NULL) {
                check_expression(expr, scope);
            }

            // Buscar o tipo do identificador à esquerda no escopo
           if (id_node != NULL && id_node->token != NULL) {
                struct symbol_list *id_symbol = NULL;

                // Primeiro, tenta procurar no escopo local
                if (scope != NULL) {
                    id_symbol = search_symbol(scope->list, id_node->token);
                }

                // Se não encontrado no escopo local, busca globalmente
                if (id_symbol == NULL) {
                    id_symbol = search_symbol(global_symbol_table->head->list, id_node->token);
                }


                if (id_symbol == NULL) {
                    printf("Line %d, column %d: Cannot find symbol %s\n", 
                            id_node->token_line, id_node->token_column, id_node->token);
                    semantic_errors++;  
                    id_node->type = undef; 
                } else {
                    id_node->type = id_symbol->type; 
                }



                if (id_node->type != expr->type) {
                    printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                        statement->token_line, statement->token_column,
                        operator_name(statement->category),
                        type_name(id_node->type), type_name(expr->type));
                    semantic_errors++;
                    statement->type = undef;  
                }

                if (id_node->type == undef && expr->type == undef) {
                    printf("Line %d, column %d: Operator %s cannot be applied to types %s, %s\n",
                        statement->token_line, statement->token_column,
                        operator_name(statement->category),
                        type_name(id_node->type), type_name(expr->type));
                    semantic_errors++;
                    statement->type = undef; 
                }
            }
            
            
            // Anotar o tipo do `Assign` com base no identificador à esquerda
            if (id_node != NULL && id_node->type != undef) {
                statement->type = id_node->type;
            } else {
                statement->type = undef; 
            }
            

            break;
        }
        case Return: {
            struct symbol_list *return_symbol = search_symbol(scope->list, "return"); // Buscar tipo de retorno da função
            if (return_symbol == NULL) {
                printf("Error: Return type not found in function scope.\n");
                semantic_errors++;
                break;
            }

            enum type return_type = return_symbol->type; 
            struct node *expr = getchild(statement, 0);

            // Caso o return seja vazio (void)
            if (expr == NULL) {
                if (return_type != no_type) {
                    printf("Line %d, column %d: Incompatible type void in return statement\n",
                        statement->token_line, statement->token_column);
                    semantic_errors++;
                }
            } else {
                check_expression(expr, scope);

                if (expr->type != return_type && expr->type != no_type) {
                    printf("Line %d, column %d: Incompatible type %s in return statement\n",
                        expr->token_line, expr->token_column,
                        type_name(expr->type));
                    semantic_errors++;
                }
            }
            break;
        }

        case If: {
            struct node *if_part = getchild(statement, 0);
            struct node *then_part = getchild(statement, 1);
            struct node *else_part = getchild(statement, 2);

            // Verifica a condição do `if`
            if (if_part != NULL) {
                check_expression(if_part, scope);
                if (if_part->type != bool_type) {
                    printf("Line %d, column %d: Incompatible type %s in if statement\n",
                        if_part->token_line, if_part->token_column,
                        type_name(if_part->type));
                    semantic_errors++;

                }
            }

            // Verifica o bloco do `then`
            if (then_part != NULL) {
                struct node_list *then_statements = then_part->children->next;
                while (then_statements != NULL) {
                    struct node *child_statement = then_statements->node;
                    if (child_statement != NULL) {
                        check_statement(child_statement, scope);
                    }
                    then_statements = then_statements->next;
                }
            }

            // Verifica o bloco do `else`, se existir
            if (else_part != NULL) {
                struct node_list *else_statements = else_part->children->next; 
                while (else_statements != NULL) {
                    struct node *child_statement = else_statements->node;
                    if (child_statement != NULL) {
                        check_statement(child_statement, scope);
                    }
                    else_statements = else_statements->next;
                }
            }
            break;
        }

        case For: {
            struct node *expr_init= getchild(statement,0);
            struct node *for_block = getchild(statement, 1); 

            if(for_block==NULL){
                struct node_list *condition_statements= expr_init->children->next;
                while(condition_statements != NULL){
                    struct node *child_for= condition_statements->node;
                    if(child_for!= NULL){
                        check_statement(child_for,scope);
                    }
                    condition_statements=condition_statements->next;
                }
            }else{
                check_expression(expr_init,scope);
                if (expr_init->type != bool_type) { 
                    printf("Line %d, column %d: Incompatible type %s in for statement\n",
                        expr_init->token_line, expr_init->token_column, 
                        type_name(expr_init->type));
                    semantic_errors++;
                    expr_init->type = undef; 
                }
                struct node_list *for_statements= for_block->children->next;
                while(for_statements != NULL){
                    struct node *child_for_block= for_statements->node;
                    if(child_for_block!= NULL){
                        check_statement(child_for_block,scope);
                    }
                    for_statements=for_statements->next;
                }
            }
            break;
        }
        case Print: {
            struct node *print_expr= getchild(statement,0);
            if(print_expr->category==StrLit){
                print_expr->type = string_type; 
            }else{
                check_expression(print_expr,scope);
                if (print_expr->type == undef) 
                    printf("Line %d, column %d: Incompatible type undef in fmt.Println statement\n", 
                            print_expr->token_line, print_expr->token_column);
                semantic_errors++;
            }
            break;
        }
        case Call:{
            check_func_invocation(statement, scope);
            break;
        }
        case ParseArgs: {
            check_pars_args(statement,scope);
        }
        default:
            break;
    }
}

void check_vars_and_statements(struct node *body, struct scope_list *scope) {
    if (body == NULL || body->children == NULL) {
        return; 
    }

    struct node_list *vrst = body->children;
    while (vrst != NULL) {
        struct node *item = vrst->node;
        if (item == NULL) {
            vrst = vrst->next;
            continue; 
        }

        if (item->category == VarDeclaration) {
            check_var_declaration(item, scope);
        } else {
            check_statement(item, scope);
        }

        vrst = vrst->next;
    }
} 

struct node* safe_getchild(struct node* parent, int index) {
    if (parent == NULL) {
        return NULL; 
    }

    int children_count = countchildren(parent);
    if (index < 0 || index >= children_count) {
        return NULL;  
    }

    return getchild(parent, index);
}

void check_func_declaration(struct node *func_decl, struct scope_list *symbol_table) {
    struct node *header = safe_getchild(func_decl, 0); 
    struct node *body = safe_getchild(func_decl, 1);

    if (header != NULL) {
        struct node *func_id = safe_getchild(header, 0); // Nome da função
        struct node *func_type = safe_getchild(header, 1); // Tipo da função
        struct node *parameters = NULL;

        // Verificar se o segundo filho é FuncParams ou outro nó
        if (func_type->category == FuncParams) {
            parameters = func_type; 
        } else {
            parameters = safe_getchild(header, 2); 
        }

        enum type var_type = category_type(func_type->category);

        // Insere a função na tabela de símbolos global
        if (search_symbol(symbol_table->head->list, func_id->token) == NULL) {
            insert_symbol(symbol_table->head->list, func_id->token, var_type, func_decl);
        } else {
            printf("Line %d, column %d: Symbol %s already defined\n", 
                    func_id->token_line, func_id->token_column, func_id->token);
            semantic_errors++;
            body->error=1;
            return;
        }

        // Cria um novo escopo para a função
        struct scope_list *scope = (struct scope_list *)malloc(sizeof(struct scope_list));
        scope->head = NULL;
        scope->identifier = func_id->token;

        scope->list = (struct symbol_list *)malloc(sizeof(struct symbol_list));
        scope->list->identifier = NULL;
        scope->list->next = NULL;
        scope->list->type = no_type;
        scope->list->node = NULL;

        scope->params = (struct param_list *)malloc(sizeof(struct param_list));
        scope->params->next = NULL;
        scope->params->type = no_type;

        scope->next = NULL;

        // Processa os parâmetros da função
        int i = 0;
        if (parameters != NULL) {
            struct node *paramdecl = safe_getchild(parameters, i); // O primeiro ParamDecl
            while (paramdecl != NULL) {
                // Obtém o tipo de cada ParamDecl
                struct node *param_type = safe_getchild(paramdecl, 0);
                if (param_type != NULL) {
                    enum type param_type_enum = category_type(param_type->category);
                    insert_param_list(scope->params, param_type_enum);
                }

                // Avança para o próximo ParamDecl
                paramdecl = safe_getchild(parameters, ++i);
            }
        }

        // liga o novo scope à tabela de símbolos global
        struct scope_list *current_scope = symbol_table;
        while (current_scope->next != NULL) {
            current_scope = current_scope->next;
        }
        current_scope->next = scope;

        insert_symbol(scope->list, "return", var_type, NULL);
        check_parameters(parameters, scope);
    }
}

void check_var_declaration(struct node *var_decl, struct scope_list *scope){
    struct node *type_node= getchild(var_decl,0);
    struct node *id_node= getchild(var_decl, 1);
    enum type var_type = category_type(type_node->category);
    
    if(search_symbol(scope->list, id_node->token)== NULL){
        insert_symbol(scope->list, id_node->token, var_type, var_decl);
        
    }else{
        printf("Line %d, column %d: Symbol %s already defined\n", 
                id_node->token_line, id_node->token_column, id_node->token);
        semantic_errors++;
    }
}

void check_func_headers(struct node *decl, struct scope_list *symbol_table) {
    if (decl == NULL) return;

    if (decl->category == FuncDeclaration) {
        check_func_declaration(decl, symbol_table); o
    }
}

void check_func_bodies(struct node *decl, struct scope_list *symbol_table) {
    if (decl == NULL) return;

    if (decl->category == FuncDeclaration) {

    
        struct node *body = safe_getchild(decl, 1);
        struct scope_list *current_scope = symbol_table;
       

        // Vai até ao scope da função
        while (current_scope != NULL && strcmp(current_scope->identifier, safe_getchild(safe_getchild(decl, 0), 0)->token) != 0) {
            current_scope = current_scope->next;
        }

        if (body && current_scope) {
            if(body->error==0)
                check_vars_and_statements(body, current_scope);  // Verifica as variáveis e declarações dentro do body
        }
    }
}

void check_declarations(struct node *decl, struct scope_list *symbol_table) {
    if (decl == NULL) return;

    if (decl->category == VarDeclaration) {
        check_var_declaration(decl, symbol_table->head);  // Verifica variáveis globais
    }
}


int check_program(struct node *program) {
    global_symbol_table = (struct scope_list *) malloc(sizeof(struct scope_list));
    global_symbol_table->identifier = "Global";

    global_symbol_table->list = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    global_symbol_table->list->identifier = NULL;
    global_symbol_table->list->next = NULL;
    global_symbol_table->list->type = no_type;
    global_symbol_table->list->node = NULL;

    global_symbol_table->params =NULL;

    global_symbol_table->next = NULL;
    

    global_symbol_table->head = global_symbol_table;
    
    struct node_list *child = program->children;

    // Primeira passagem: verifica todos os FuncHeaders
    struct node_list *current = child;
    while ((current = current->next) != NULL) {
        if (current->node->category == FuncDeclaration) {
            check_func_headers(current->node, global_symbol_table);
        } else {
            check_declarations(current->node, global_symbol_table);
        }
    }

    // Segunda passagem: verifica os FuncBodies e outras declarações
    current = child;
    while ((current = current->next) != NULL) {
            check_func_bodies(current->node, global_symbol_table);
    }

    return semantic_errors;
}


// insert a new symbol in the list, unless it is already there
struct symbol_list *insert_symbol(struct symbol_list *table, char *identifier, enum type type, struct node *node) {
    struct symbol_list *new = (struct symbol_list *) malloc(sizeof(struct symbol_list));
    new->identifier = strdup(identifier);
    new->type = type;
    new->node = node;
    new->next = NULL;
    struct symbol_list *symbol = table;

    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } else if(strcmp(symbol->next->identifier, identifier) == 0) {
            free(new);
            return NULL;           /* return NULL if symbol is already inserted */
        }
        symbol = symbol->next;
    }
    return new;
}


// insert a new symbol in the list, unless it is already there
struct param_list *insert_param_list(struct param_list *table, enum type type) {
    struct param_list *new = (struct param_list *) malloc(sizeof(struct param_list));
    new->type = type;
    new->next = NULL;
    struct param_list *symbol = table;

    while(symbol != NULL) {
        if(symbol->next == NULL) {
            symbol->next = new;    /* insert new symbol at the tail of the list */
            break;
        } 
        symbol = symbol->next;
    }
    return new;
}

// look up a symbol by its identifier
struct symbol_list *search_symbol(struct symbol_list *table, char *identifier) {
    struct symbol_list *symbol;
    for(symbol = table->next; symbol != NULL; symbol = symbol->next)
        if(strcmp(symbol->identifier, identifier) == 0)
            return symbol;
    return NULL;
}


void show_symbol_table() {
    // Imprime a Tabela Global de Símbolos
    printf("===== Global Symbol Table =====\n");

    struct symbol_list *symbol = global_symbol_table->head->list->next;
    while (symbol != NULL) {
        // Se for uma função, imprime com os parâmetros e tipo de retorno
        if (symbol->node->category == FuncDeclaration) {
            printf("%s\t", symbol->identifier); 

            struct node *header = getchild(symbol->node, 0);  
            struct node *func_type = getchild(header, 1);

            struct node *params = NULL;
        
            if(func_type->category == FuncParams){
                params = safe_getchild(header, 1); 
            } else {
                params = safe_getchild(header, 2);
            }

            // Imprimir parâmetros
            if (params != NULL && params->children != NULL) {
                struct node_list *param = params->children;
                printf("(");
                int first = 1;
                while (param != NULL) {
                    if (param->node == NULL) {  
                        param = param->next;
                        continue;
                    }
                    struct node *param_type_node = getchild(param->node, 0);
                    if (param_type_node != NULL) {  
                        enum type param_type = category_type(param_type_node->category);
                        if (!first) printf(",");
                        printf("%s", type_name(param_type));
                        first = 0;
                    }
                    param = param->next;
                }
                printf(")\t");
            } else {
                printf("()\t");
            }

            // Imprimir tipo de retorno
            struct node *return_type_node = getchild(header, 1);
            enum type return_type = category_type(return_type_node->category);
            printf("%s", type_name(return_type)); 
        } else {
            // Se for uma variável, imprime nome e tipo
            printf("%s\t\t%s", symbol->identifier, type_name(symbol->type));
        }
        symbol = symbol->next;

        printf("\n");
    }
    printf("\n");
    // Imprimir a tabela de símbolos das funções
    struct scope_list *scope = global_symbol_table->next;
    while (scope != NULL) {
        
        printf("===== Function %s(", scope->identifier);

        // Imprimir os parâmetros no cabeçalho da tabela de símbolos
        struct param_list *params = scope->params->next;
        int first = 1;
        while (params != NULL) {
                if (!first) printf(",");
                printf("%s", type_name(params->type));
                first = 0;
            params = params->next;
        }
        printf(") Symbol Table =====\n");

        struct symbol_list *func_symbol = scope->list ? scope->list->next : NULL;
        while (func_symbol != NULL) {
            printf("%s\t\t%s", func_symbol->identifier, type_name(func_symbol->type));

            if (func_symbol->node && func_symbol->node->category == ParamDecl) {
                printf("\tparam");
            }
            printf("\n");
            func_symbol = func_symbol->next;
        }

        printf("\n");
        scope = scope->next;


    }
}
