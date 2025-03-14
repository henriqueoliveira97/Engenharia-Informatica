%{
#include "ast.h"
#include <stddef.h>
#include <stdio.h>

int yylex(void);
void yyerror(char *);

struct node *program;
struct node *current_type=NULL;
struct node_list* childs;

%}

%token PACKAGE IF ELSE VAR SEMICOLON LPAR RPAR FUNC COMMA LBRACE RBRACE ASSIGN FOR RETURN PRINT BLANKID PARSEINT CMDARGS LSQ RSQ RESERVED
%token<lexeme> IDENTIFIER NATURAL DECIMAL INTEGER FLOAT32 BOOL STRING STRLIT
%type<node> program declarations vardeclaration varspec type funcdeclaration funcbody statement varsandstatements expr funcinvocation parseargs argumentsopt commaIdentifier statementopt parameters paramdecl

%left LOW
%left OR 
%left AND
%left EQ NE LT GT LE GE
%left PLUS MINUS
%left STAR DIV MOD
%right UMINUS UPLUS
%right NOT

%union{
    char *lexeme;
    struct node *node;
}

%locations
%{
#define LOCATE(node, line, column) { node->token_line = line; node->token_column = column; }
%}

%%

program: PACKAGE IDENTIFIER SEMICOLON declarations {  }
    ;

declarations:
      declarations vardeclaration SEMICOLON {
          $$ = $1;
          addchild($$, $2); // Adiciona a declaração de variável à AST
          struct node_list* current = childs->head; 
                while (current != NULL) {
                    addchild($$, current->node);
                    current = current->next;
                }
      }
    | declarations funcdeclaration SEMICOLON { 
          $$ = $1;
          addchild($$, $2); // Adiciona a declaração de função à AST
      }
    | { 
          $$ = program = newnode(Program, NULL); // Cria o nó raiz "Program"
      }
    ;

vardeclaration:
      VAR varspec { 
          $$ = $2; // A declaração de variável é representada pelo nó retornado por varspec
      }
    | VAR LPAR varspec SEMICOLON RPAR { 
          $$ = $3; // Declaração de variável em formato agrupado
      }
    ;

varspec:
      IDENTIFIER commaIdentifier type { 
          current_type = $3; // Define o tipo atual para todas as variáveis nesta declaração
          childs = malloc(sizeof(struct node_list));
          init_global_list(childs);

          clear_node_list(childs);

          // Primeiro nó de declaração de variável
          struct node* first_var_decl = newnode(VarDeclaration, NULL);
          addchild(first_var_decl, $3); // Tipo da variável
          addchild(first_var_decl, newnode(Identifier, $1)); // Identificador principal
          LOCATE(getchild(first_var_decl,1), @1.first_line, @1.first_column);

          struct node_list* current = $2->children->next;
            while (current != NULL) {
                struct node* var_decl = newnode(VarDeclaration, NULL);
                addchild(var_decl, current_type);  
                addchild(var_decl, current->node);       
                add_to_node_list(childs, var_decl);
                LOCATE(getchild(var_decl, 1), current->node->token_line, current->node->token_column);
                current = current->next;
            }

          $$ = first_var_decl; // Retorna o nó principal da declaração
          
      }
    ;





commaIdentifier:
    { $$ = newnode(Ignora, NULL); } // Cria um nó Ignora no caso base
| commaIdentifier COMMA IDENTIFIER {
        struct node* id_node = newnode(Identifier, $3);
        LOCATE(id_node, @3.first_line, @3.first_column);
        addchild($1, id_node);
        $$ = $1; // Retorna a lista de identificadores

};

funcdeclaration: FUNC IDENTIFIER LPAR parameters RPAR type funcbody
                                            {
                                              struct node *fhead = newnode(FuncHeader,NULL);
                                              $$ = newnode(FuncDeclaration, NULL);
                                              addchild(fhead, newnode(Identifier, $2));
                                              addchild(fhead, $6);  //type
                                              addchild(fhead, $4);  //parms
                                              addchild($$, fhead);
                                              addchild($$, $7);     // funcbody
                                              LOCATE(getchild(fhead, 0), @2.first_line, @2.first_column);
                                            }
    | FUNC IDENTIFIER LPAR RPAR type funcbody {
                                              struct node *fhead = newnode(FuncHeader,NULL);
                                              $$ = newnode(FuncDeclaration, NULL);
                                              addchild(fhead, newnode(Identifier, $2));
                                              addchild(fhead, $5);
                                              addchild(fhead, newnode(FuncParams, NULL));
                                              addchild($$, fhead);
                                              addchild($$, $6);
                                              LOCATE(getchild(fhead, 0), @2.first_line, @2.first_column);
                                            }
    | FUNC IDENTIFIER LPAR parameters RPAR funcbody {
                                              struct node *fhead = newnode(FuncHeader,NULL);
                                              $$ = newnode(FuncDeclaration, NULL);
                                              addchild(fhead, newnode(Identifier, $2));
                                              addchild(fhead, $4);
                                              addchild($$, fhead);
                                              addchild($$, $6);
                                              LOCATE(getchild(fhead, 0), @2.first_line, @2.first_column);
                                            }
    | FUNC IDENTIFIER LPAR RPAR funcbody {
                                              struct node *fhead = newnode(FuncHeader,NULL);
                                              $$ = newnode(FuncDeclaration, NULL);
                                              addchild(fhead, newnode(Identifier, $2));
                                              addchild(fhead, newnode(FuncParams, NULL));
                                              addchild($$, fhead);
                                              addchild($$, $5);
                                              LOCATE(getchild(fhead, 0), @2.first_line, @2.first_column);
                                            }
    ;
  
funcbody: LBRACE varsandstatements RBRACE   {
                                              $$ = $2;
                                            }
    ;

type: INTEGER                       { $$ = newnode(Integer,NULL);
                                      LOCATE($$, @1.first_line, @1.first_column);
                                    }
    | FLOAT32                        { $$ = newnode(Float_32,NULL);
                                       LOCATE($$, @1.first_line, @1.first_column);
                                    }
    | BOOL                           {
                                      $$ = newnode(Bool,NULL);
                                      LOCATE($$, @1.first_line, @1.first_column);
                                    }
    | STRING                         {
                                      $$ = newnode(String,NULL);
                                      LOCATE($$, @1.first_line, @1.first_column);
                                    }
    ;

parameters: paramdecl            { $$ = newnode(FuncParams, NULL); addchild($$, $1);/*LOCATE($$, @1.first_line, @1.first_column); */}
         | parameters COMMA paramdecl { $$ = $1; addchild($$, $3);  }
         ;

paramdecl: IDENTIFIER type       {
                                    struct node *param = newnode(ParamDecl, NULL);
                                    addchild(param, $2);
                                    addchild(param, newnode(Identifier, $1));
                                    $$ = param;
                                    LOCATE(getchild($$, 1), @1.first_line, @1.first_column);
                                 }
         ;

varsandstatements: varsandstatements vardeclaration SEMICOLON { $$ = $1;
                addchild($$, $2);
                struct node_list* current = childs->head; 
                while (current != NULL) {
                    addchild($$, current->node);
                    current = current->next;
                } }
    | varsandstatements statement SEMICOLON      { $$ = $1; addchild($$, $2); }
    | varsandstatements SEMICOLON               { $$ = $1; }
    |                                           { $$ = newnode(FuncBody, NULL); }
    ;

statement: IDENTIFIER ASSIGN expr                {

                                                    struct node *assign = newnode(Assign, NULL);
                                                    addchild(assign, newnode(Identifier, $1));  
                                                    addchild(assign, $3);                        
                                                    $$ = assign;
                                                    LOCATE($$, @2.first_line, @2.first_column);
                                                    LOCATE(getchild(assign,0), @1.first_line, @1.first_column);
                                                }
    | LBRACE statementopt RBRACE                 { 
                                      
                                                      if (childcount($2) >= 2) $$ = $2; else $$ = removeblock($2);
                                                      
                                               }
    | IF expr LBRACE statementopt RBRACE ELSE LBRACE statementopt RBRACE  {
                                                                         struct node *if_stmt = newnode(If, NULL);
                                                                         addchild(if_stmt, $2);
                                                                         addchild(if_stmt, $4);
                                                                         addchild(if_stmt, $8);
                                                                         $$ = if_stmt;
                                                                         LOCATE($$, @1.first_line, @1.first_column);
                                                                        } %prec LOW
    | IF expr LBRACE statementopt RBRACE                  {
                                                                         struct node *if_stmt = newnode(If, NULL);
                                                                         addchild(if_stmt, $2);
                                                                         addchild(if_stmt, $4);
                                                                         addchild(if_stmt, newnode(Block, NULL));
                                                                         $$ = if_stmt;
                                                                         LOCATE($$, @1.first_line, @1.first_column);
                                                                     } %prec LOW
    | FOR LBRACE statementopt RBRACE                      { 
                                                             struct node *for_stmt = newnode(For, NULL);
                                                             addchild(for_stmt, $3);
                                                             $$ = for_stmt;
                                                             LOCATE($$, @1.first_line, @1.first_column);
                                                           } %prec LOW
    | FOR expr LBRACE statementopt RBRACE                 { 
                                                             struct node *for_stmt = newnode(For, NULL);
                                                             addchild(for_stmt, $2);
                                                             addchild(for_stmt, $4);
                                                             $$ = for_stmt;
                                                             LOCATE($$, @1.first_line, @1.first_column);
                                                           } %prec LOW
    | RETURN                                              { $$ = newnode(Return, NULL); LOCATE($$, @1.first_line, @1.first_column);}
    | RETURN expr                                         { 
                                                             struct node *rtrn = newnode(Return, NULL);
                                                             addchild(rtrn, $2);
                                                             $$ = rtrn;
                                                             LOCATE($$, @1.first_line, @1.first_column);
                                                           }
    | funcinvocation                                       {
                                                             $$ = $1;
                                                           }
    | parseargs                                            {
                                                            $$ = $1;
                                                           }
    | PRINT LPAR expr RPAR                                 { 
                                                             struct node *prtn = newnode(Print, NULL);
                                                             addchild(prtn, $3);
                                                             $$ = prtn;
                                                             LOCATE($$, @1.first_line, @1.first_column);
                                                           }
    | PRINT LPAR STRLIT RPAR                               { 
                                                             struct node *prtn = newnode(Print, NULL);
                                                             addchild(prtn, newnode(StrLit, $3));
                                                             $$ = prtn;
                                                             LOCATE($$, @1.first_line, @1.first_column);
                                                           }
    | error                                                { }
  ;

statementopt: statementopt statement SEMICOLON             { $$ = $1;  if ($2 != NULL) addchild($$, $2); }
    
    |                                           { $$ = newnode(Block, NULL); }
  ;

expr: expr OR expr        { struct node *or_expr = newnode(Or, NULL); addchild(or_expr, $1); addchild(or_expr, $3); $$ = or_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr AND expr       { struct node *and_expr = newnode(And, NULL); addchild(and_expr, $1); addchild(and_expr, $3); $$ = and_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr LT expr        { struct node *lt_expr = newnode(LessThan, NULL); addchild(lt_expr, $1); addchild(lt_expr, $3); $$ = lt_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr GT expr        { struct node *gt_expr = newnode(GreaterThan, NULL); addchild(gt_expr, $1); addchild(gt_expr, $3); $$ = gt_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr EQ expr        { struct node *eq_expr = newnode(Equal, NULL); addchild(eq_expr, $1); addchild(eq_expr, $3); $$ = eq_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr NE expr        { struct node *ne_expr = newnode(NotEqual, NULL); addchild(ne_expr, $1); addchild(ne_expr, $3); $$ = ne_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr LE expr        { struct node *le_expr = newnode(LessEqual, NULL); addchild(le_expr, $1); addchild(le_expr, $3); $$ = le_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr GE expr        { struct node *ge_expr = newnode(GreaterEqual, NULL); addchild(ge_expr, $1); addchild(ge_expr, $3); $$ = ge_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr PLUS expr      { struct node *add_expr = newnode(Add, NULL); addchild(add_expr, $1); addchild(add_expr, $3); $$ = add_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr MINUS expr     { struct node *sub_expr = newnode(Sub, NULL); addchild(sub_expr, $1); addchild(sub_expr, $3); $$ = sub_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr STAR expr      { struct node *mul_expr = newnode(Mul, NULL); addchild(mul_expr, $1); addchild(mul_expr, $3); $$ = mul_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr DIV expr       { struct node *div_expr = newnode(Div, NULL); addchild(div_expr, $1); addchild(div_expr, $3); $$ = div_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | expr MOD expr       { struct node *mod_expr = newnode(Mod, NULL); addchild(mod_expr, $1); addchild(mod_expr, $3); $$ = mod_expr;LOCATE($$, @2.first_line, @2.first_column); }
    | NOT expr            { struct node *not_expr = newnode(Not, NULL); addchild(not_expr, $2); $$ = not_expr; LOCATE($$, @1.first_line, @1.first_column); } %prec NOT
    | MINUS expr          { struct node *unary_minus = newnode(UnaryMinus, NULL); addchild(unary_minus, $2); $$ = unary_minus; LOCATE($$, @1.first_line, @1.first_column); } %prec UMINUS
    | PLUS expr           { struct node *unary_plus = newnode(UnaryPlus, NULL); addchild(unary_plus, $2); $$ = unary_plus; LOCATE($$, @1.first_line, @1.first_column);  } %prec UPLUS
    | IDENTIFIER          { $$ = newnode(Identifier, $1);  LOCATE($$, @1.first_line, @1.first_column);}
    | NATURAL             { $$ = newnode(Natural, $1); LOCATE($$, @1.first_line, @1.first_column);}
    | DECIMAL             { $$ = newnode(Decimal, $1); LOCATE($$, @1.first_line, @1.first_column);}
    | LPAR expr RPAR      { $$ = $2; }
    | funcinvocation      { $$ = $1;}
    | LPAR error RPAR     { }

  ;


funcinvocation: IDENTIFIER LPAR RPAR      {struct node* call = newnode(Call, NULL);
                                            addchild(call, newnode(Identifier, $1));
                                            $$ = call;
                                            LOCATE($$, @1.first_line, @1.first_column);
                                          }
    | IDENTIFIER LPAR expr argumentsopt RPAR {
                                                struct node* call = newnode(Call, NULL);
                                                
                                                addchild(call, newnode(Identifier, $1));
                                                
                                                addchild(call, $3);

                                                struct node_list* argument = $4->children->next;
                                                while (argument != NULL) {
                                                    addchild(call, argument->node);
                                                    argument = argument->next;
                                                }

                                                $$ = call;
                                                LOCATE($$, @1.first_line, @1.first_column);
                                              }


argumentsopt: argumentsopt COMMA expr {   
    addchild($1, $3);
    $$ = $1;
}
           | { $$ = newnode(Call, NULL); }  
           ;



parseargs: IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR CMDARGS LSQ expr RSQ RPAR  {
                                                  struct node *parse_args = newnode(ParseArgs, NULL);
                                                  addchild(parse_args, newnode(Identifier, $1));
                                                  addchild(parse_args, $9);
                                                  $$ = parse_args;
                                                  LOCATE($$, @5.first_line, @5.first_column);
                                              }
    | IDENTIFIER COMMA BLANKID ASSIGN PARSEINT LPAR error RPAR                      { }
  ;


%%

