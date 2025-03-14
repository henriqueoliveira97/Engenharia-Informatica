#ifndef _SEMANTICS_H
#define _SEMANTICS_H

#include "ast.h"

int check_program(struct node *program);

struct symbol_list {
	char *identifier;
	enum type type;
	struct node *node;
	int used;
	struct symbol_list *next;
};

struct scope_list {
	struct scope_list *head;
	char *identifier;
	struct symbol_list *list;	// cada scope é uma funçao 
	struct scope_list *next;
	struct param_list *params;
};

struct param_list {
	enum type type;
	struct param_list *next;
};

struct symbol_list *insert_symbol(struct symbol_list *symbol_table, char *identifier, enum type type, struct node *node);
struct param_list *insert_param_list(struct param_list *table, enum type type);
struct symbol_list *search_symbol(struct symbol_list *symbol_table, char *identifier);
void show_symbol_table();

#endif
