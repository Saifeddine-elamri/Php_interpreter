#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

typedef struct {
    char* key;   // Peut être NULL pour les tableaux indexés
    char* value;
} ArrayItem;

typedef struct {
    ArrayItem* items;
    int count;
    int capacity;
} Array;

typedef struct {
    char* name;
    char* value;  // Pour les variables scalaires
    Array* array; // Pour les tableaux, NULL si ce n'est pas un tableau
} Variable;

typedef struct {
    Lexer* lexer;
    int position;
    Variable* variables;
    int var_count;
    int var_capacity;
} Parser;

Parser* parser_create(Lexer* lexer);
void parser_free(Parser* parser);
void parser_run(Parser* parser);

#endif