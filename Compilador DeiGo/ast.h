#ifndef _AST_H
#define _AST_H

enum type {integer_type, double_type,bool_type, float32_type, string_type, undef, no_type};
#define type_name(type) \
    (type == integer_type ? "int" : \
    (type == double_type ? "double" : \
    (type == float32_type ? "float32" : \
    (type == bool_type ? "bool" : \
    (type == string_type ? "string" : \
    (type == undef ? "undef" : "none"))))))

// Atualiza a macro para mapear categorias para os tipos
#define category_type(category) \
    (category == Integer ? integer_type : \
    (category == Double ? double_type : \
    (category == Float_32 ? float32_type : \
    (category == Bool ? bool_type :\
    (category == String ? string_type : no_type)))))



// Temos categorias que nunca usamos - Duarte
enum category { Program,           
    Package,
    Function,
    Declarations,      
    VarDeclaration,    
    VarSpec,           
    Var,
    Type,              
    Integer,
    Double,
    FuncHeader,
    Float_32,
    Assign,
    FuncParams,
    ParamDecl,
    Bool,
    Call,
    String,
    StrLit,
    FuncDeclaration,   
    Parameters,        
    FuncBody,         
    VarsAndStatements, 
    Statement,         
    ParseArgs,         
    FuncInvocation,    
    Expression,
    Print,             
    Return,           
    Natural,           
    Decimal,           
    Identifier,        
    ComaIdentifier,
    Ignora,
    
    Block,
    Or,                
    And,               
    LessThan,          
    GreaterThan,       
    Equal,             
    NotEqual,          
    LessEqual,         
    GreaterEqual,      
    Add,               
    Sub,               
    Mul,               
    Div,               
    Mod,               
    Not,               
    UnaryMinus,        
    UnaryPlus,          
    If,
    For,
};

struct node {
    enum category category;
    char *token;
    int token_line, token_column;
    int error;
    struct node_list *children;
    enum type type;
    struct node *next; 

};

struct node_list {
    struct node *node;
    struct node_list *next;
    struct node_list *head;
};

struct node *newnode(enum category category, char *token);
int childcount(struct node *parent);
struct node* removeblock(struct node* block);
struct node* get_parent(struct node* root, struct node* target);
void addchild(struct node *parent, struct node *child);
void show_s(struct node *node, int depth);
void show_t(struct node *node, int depth);
void clear_node_list(struct node_list *list);
void init_global_list(struct node_list *list);
void add_to_node_list(struct node_list *list, struct node *new_node);
struct node *getchild(struct node *parent, int position);
int countchildren(struct node *node);

#endif