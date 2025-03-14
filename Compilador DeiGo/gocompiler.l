%{
    /*
    * Autores:
    * - Duarte Dinis Pereira (nº 2022236193)
    * - Henrique Oliveira (nº 2022211169)
    *
    * Descrição: Compilador da linguagem de programação Go
    */
    
    /* place here any C variables and definitions */
    #include "ast.h"
    #include "y.tab.h"
    #include "semantics.h"
    #define YY_USER_ACTION { syn_line = yylloc.first_line = l; if (YY_START == STRING_LITERAL) { syn_column = yylloc.first_column = coluna_inicial; } else { syn_column = yylloc.first_column = c; c += yyleng; }}

    #define PASS_TOKEN() yylval.lexeme = strdup(yytext);
    int yyparse(void);
    void yyerror(char *);
    int l = 1;  // linha
    int c = 1;  // coluna
    char str_buffer[119]; 
    char buffer2[2048];
    int str_index = 0;      
    int cancel = 0;
    int endline = 0;
    int coluna_inicial = 0; 
    int coluna_inicialescape = 0;
    int linha_inicial=0;
    int show_tokens = 0; 
    int syn_line = -1, syn_column = -1;
    int errors = 0; 
    int syntax_error = 0;
    extern struct node *program;
    int generate_ast = 0;
    int semantic_analysis = 0;

%}


digit   [0-9]
char    [a-zA-Z_]
hex_digit [0-9a-fA-F]
oct_digit [0-7]
whitespace [\t ]
reserved     "break"|"default"|"interface"|"select"|"case"|"defer"|"go"|"map"|"struct"|"chan"|"goto"|"switch"|"const"|"fallthrough"|"range"|"type"|"continue"|"import"|"++"|"--"

%X COMMENT
%X COMMENT_LINE
%X STRING_LITERAL

%%

"if"             { if (show_tokens) printf("IF\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return IF; }
"else"           { if (show_tokens) printf("ELSE\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return ELSE; }
"package"        { if (show_tokens) printf("PACKAGE\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return PACKAGE; }
"return"         { if (show_tokens) printf("RETURN\n");  endline = 1;PASS_TOKEN();if (generate_ast) return RETURN; }
"for"            { if (show_tokens) printf("FOR\n");  endline = 0; PASS_TOKEN();if (generate_ast)return FOR; }
"var"            { if (show_tokens) printf("VAR\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return VAR; }
"int"            { if (show_tokens) printf("INT\n");  endline = 0; PASS_TOKEN();if (generate_ast)return INTEGER; }
"float32"        { if (show_tokens) printf("FLOAT32\n");  endline = 0; PASS_TOKEN();if (generate_ast)return FLOAT32; }
"bool"           { if (show_tokens) printf("BOOL\n");  endline = 0;PASS_TOKEN();if (generate_ast) return BOOL; }
"string"         { if (show_tokens) printf("STRING\n");  endline = 0;PASS_TOKEN();if (generate_ast) return STRING; }
"fmt.Println"    { if (show_tokens) printf("PRINT\n");  endline = 0; PASS_TOKEN();if (generate_ast)return PRINT; }
"strconv.Atoi"   { if (show_tokens) printf("PARSEINT\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return PARSEINT; }
"func"           { if (show_tokens) printf("FUNC\n");  endline = 0; PASS_TOKEN();if (generate_ast)return FUNC; }
"os.Args"        { if (show_tokens) printf("CMDARGS\n");  endline = 0; PASS_TOKEN();if (generate_ast)return CMDARGS; }

{reserved}       { if (show_tokens) printf("RESERVED(%s)\n", yytext);  endline = 0; if (generate_ast)return RESERVED;}

"_"             { if (show_tokens) printf("BLANKID\n");  endline = 0; if (generate_ast)return BLANKID; }

"0"("x"|"X"){hex_digit}+  { if (show_tokens) printf("NATURAL(%s)\n", yytext);  endline = 1;PASS_TOKEN(); if (generate_ast)return NATURAL; }
"0"{oct_digit}+            { if (show_tokens) printf("NATURAL(%s)\n", yytext);  endline = 1;PASS_TOKEN(); if (generate_ast)return NATURAL; }
"0"{oct_digit}*[89]+{digit}* {
    coluna_inicial = c; 
    printf("Line %d, column %d: invalid octal constant (%s)\n", l, coluna_inicial, yytext);
  
    endline = 1;
}
{digit}+                   { if (show_tokens) printf("NATURAL(%s)\n", yytext);  endline = 1;PASS_TOKEN(); if (generate_ast)return NATURAL; }

{digit}+"."{digit}*(("e"|"E")("+"|"-")?{digit}+)? { if (show_tokens) printf("DECIMAL(%s)\n", yytext);  endline = 1;PASS_TOKEN();if (generate_ast) return DECIMAL; }
"."{digit}+(("e"|"E")("+"|"-")?{digit}+)?        { if (show_tokens) printf("DECIMAL(%s)\n", yytext);  endline = 1;PASS_TOKEN(); if (generate_ast)return DECIMAL; }
{digit}+(("e"|"E")("+"|"-")?{digit}+)?           { if (show_tokens) printf("DECIMAL(%s)\n", yytext);  endline = 1;PASS_TOKEN(); if (generate_ast)return DECIMAL; }

{char}+(({char})|{digit})* { if (show_tokens) printf("IDENTIFIER(%s)\n", yytext);  endline = 1;PASS_TOKEN(); return IDENTIFIER; }

{whitespace}               { ; }

";"                      { if (show_tokens) printf("SEMICOLON\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return SEMICOLON; }
"=="                     { if (show_tokens) printf("EQ\n"); endline = 0;PASS_TOKEN(); if (generate_ast)return EQ; }
","                      { if (show_tokens) printf("COMMA\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return COMMA; }
"="                      { if (show_tokens) printf("ASSIGN\n");  endline = 0;PASS_TOKEN();PASS_TOKEN(); if (generate_ast)return ASSIGN; }
"*"                      { if (show_tokens) printf("STAR\n");  endline = 0;PASS_TOKEN();if (generate_ast) return STAR; }
"/"                      { if (show_tokens) printf("DIV\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return DIV; }
"-"                      { if (show_tokens) printf("MINUS\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return MINUS; }
"+"                      { if (show_tokens) printf("PLUS\n"); endline = 0; PASS_TOKEN();if (generate_ast)return PLUS; }
">="                     { if (show_tokens) printf("GE\n"); endline = 0; PASS_TOKEN();if (generate_ast)return GE; }
">"                      { if (show_tokens) printf("GT\n");  endline = 0; PASS_TOKEN();if (generate_ast)return GT; }
"{"                      { if (show_tokens) printf("LBRACE\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return LBRACE; }
"<="                     { if (show_tokens) printf("LE\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return LE; }
"("                      { if (show_tokens) printf("LPAR\n"); endline = 0;PASS_TOKEN();if (generate_ast) return LPAR; }
"["                      { if (show_tokens) printf("LSQ\n"); endline = 0;PASS_TOKEN(); if (generate_ast)return LSQ; }
"<"                      { if (show_tokens) printf("LT\n");  endline = 0;PASS_TOKEN(); if (generate_ast)return LT; }
"%"                      { if (show_tokens) printf("MOD\n");  endline = 0;PASS_TOKEN();if (generate_ast) return MOD; }
"!="                     { if (show_tokens) printf("NE\n");  endline = 0; PASS_TOKEN();if (generate_ast)return NE; }
"!"                      { if (show_tokens) printf("NOT\n");  endline = 0; PASS_TOKEN();if (generate_ast)return NOT; }
"&&"                     { if (show_tokens) printf("AND\n"); endline = 0; PASS_TOKEN();if (generate_ast)return AND; }
"||"                     { if (show_tokens) printf("OR\n");  endline = 0; PASS_TOKEN();if (generate_ast)return OR; }
"}"                      { if (show_tokens) printf("RBRACE\n");; endline = 1;PASS_TOKEN(); if (generate_ast)return RBRACE; }
")"                      { if (show_tokens) printf("RPAR\n"); endline = 1;PASS_TOKEN();if (generate_ast) return RPAR; }
"]"                      { if (show_tokens) printf("RSQ\n"); endline = 1; PASS_TOKEN();if (generate_ast)return RSQ; }


"/*" { 
    linha_inicial=l;
    coluna_inicial = c;  
    BEGIN(COMMENT); 
}

<COMMENT>"*/" { 
    BEGIN(INITIAL); 
}

<COMMENT><<EOF>> { 
    printf("Line %d, column %d: unterminated comment\n", linha_inicial, coluna_inicial); 
    BEGIN(INITIAL);
    endline = 0;
}

<COMMENT>\n { 
    l++; 
    c = 1;  
}

<COMMENT>. { 
   
}


"//"                     { BEGIN(COMMENT_LINE); }
<COMMENT_LINE>\n          { 
                             BEGIN(INITIAL);
                             l++; c = 1;
                             if (endline) {
                                if (show_tokens) printf("SEMICOLON\n");
                                endline = 0;
                                return SEMICOLON;
                             }
                          }
<COMMENT_LINE>.           {  }

\"                        { BEGIN(STRING_LITERAL); 
                            str_index = 0; 
                            str_buffer[0] = '\0';
                            str_buffer[str_index++] = yytext[0];  
                            coluna_inicial = c-1;  
                            cancel = 0; }

<STRING_LITERAL>\"                { BEGIN(INITIAL);
                             str_buffer[str_index++] = yytext[0];
                             str_buffer[str_index] = '\0';
                             // Limpa o buffer2 antes de retornar
                                buffer2[0] = '\0';
                             snprintf(buffer2, sizeof(buffer2), "%s", str_buffer);
                             
                             c += yyleng;
                             if (cancel == 0) {
                                if(show_tokens){
                                printf("STRLIT(%s)\n", str_buffer); 
                                }  
                                yylval.lexeme = strdup(buffer2);
                                 
                                endline = 1;
                                
                                cancel = 0;  
                                if (generate_ast)return STRLIT;
                                
                             }
                             
                             
                          }

<STRING_LITERAL><<EOF>>           { 
                             printf("Line %d, column %d: unterminated string literal\n", l, coluna_inicial);
                             cancel = 0; 
                             BEGIN(INITIAL);
                             endline = 0; 
                             c += yyleng;
                          }

<STRING_LITERAL>\\"\n"            { 
                             coluna_inicialescape = c; 
                             yytext[yyleng-1] = '\0';
                             printf("Line %d, column %d: invalid escape sequence (%s)\n", l, coluna_inicialescape, yytext); 
                             printf("Line %d, column %d: unterminated string literal\n", l, coluna_inicial);
                             cancel = 0;
                             BEGIN(INITIAL); 
                             
                             l++;c=1;
                          }

<STRING_LITERAL>[\n]              { 
                             printf("Line %d, column %d: unterminated string literal\n", l, coluna_inicial);
                             BEGIN(INITIAL); 
                             cancel = 1;
                             
                             l++;c=1;
                          }

<STRING_LITERAL>\\[ntfr\"\\]      { 
                             str_buffer[str_index++] = yytext[0];  
                             str_buffer[str_index++] = yytext[1];
                             c += yyleng;
                               
                          }

<STRING_LITERAL>\\[^ntfr\"\\]     { 
                             
                             coluna_inicialescape = c; 
                             printf("Line %d, column %d: invalid escape sequence (%s)\n", l, coluna_inicialescape, yytext); 
                             cancel = 1;
c += yyleng;
                          }

<STRING_LITERAL>.                 { 
                             str_buffer[str_index++] = yytext[0];
                             c += yyleng;
                          }


"\n"                      { 
                             l++; c = 1;
                             if (endline) {
                                if (show_tokens) printf("SEMICOLON\n");
                                endline = 0;
                                if (generate_ast) return SEMICOLON;
                             }
                          } 

<<EOF>>                   { 
                             if (endline) {
                                if (show_tokens) printf("SEMICOLON\n");
                                endline = 0;
                                if (generate_ast) return SEMICOLON;
                             }
                             YY_USER_ACTION;
                             return 0;
                          }

.                         { 
                             coluna_inicial = c; 
                             printf("Line %d, column %d: illegal character (%s)\n", l, coluna_inicial, yytext);
                          }

%%

extern int yylex();

int main(int argc, char **argv) {

    // Configura os modos de operação
    if (argc > 1) {
        if (strcmp(argv[1], "-l") == 0) {
            show_tokens = 1;  // Mostra os tokens
        } else if (strcmp(argv[1], "-t") == 0) {
            generate_ast = 1; // Gera a AST
        } else if (strcmp(argv[1], "-s") == 0) {
            generate_ast = 1; // Gera a AST
            semantic_analysis = 1; // Realiza análise semântica
        }
    }

    // Executa o parser
    if (yyparse() == 0 && syntax_error == 0) {

        if (generate_ast) {
            if (semantic_analysis && errors ==0) {
                // Realiza a análise semântica
                errors+=check_program(program);

                show_symbol_table(); // Exibe as tabelas de símbolos

                show_s(program, 0); // Mostra a AST (com ou sem semântica, dependendo do caso)
            } else {
               show_t(program, 0); // Mostra a AST (com ou sem semântica, dependendo do caso)
            }

            
        }
    }
    return 0;
}



void yyerror(char *error) {
    errors++;
    if (strcmp(error, "syntax error") == 0) syntax_error = 1;

     if(yylval.lexeme != NULL && strlen(yylval.lexeme) > 0 && strlen(buffer2) > 2){
        printf("Line %d, column %d: %s: %s\n",syn_line,syn_column,error,yylval.lexeme);
     }else{
        printf("Line %d, column %d: %s: %s\n",syn_line,syn_column,error,yytext);
     }

}


int yywrap() {
    return 1;
}