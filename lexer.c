#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

Lexer* lexer_create(const char* source) {
    Lexer* lexer = malloc(sizeof(Lexer));
    lexer->source = strdup(source);
    lexer->position = 0;
    lexer->token_count = 0;
    lexer->token_capacity = 10;
    lexer->tokens = malloc(sizeof(Token) * lexer->token_capacity);
    return lexer;
}

void lexer_free(Lexer* lexer) {
    for (int i = 0; i < lexer->token_count; i++) {
        free(lexer->tokens[i].value);
    }
    free(lexer->tokens);
    free(lexer->source);
    free(lexer);
}

static void add_token(Lexer* lexer, TokenType type, char* value) {
    if (lexer->token_count >= lexer->token_capacity) {
        lexer->token_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->token_capacity);
    }
    lexer->tokens[lexer->token_count].type = type;
    lexer->tokens[lexer->token_count].value = value ? strdup(value) : NULL;
    lexer->token_count++;
}

void lexer_tokenize(Lexer* lexer) {
    while (lexer->source[lexer->position] != '\0') {
        char c = lexer->source[lexer->position];

        if (isspace(c)) {
            lexer->position++;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "<?php", 5) == 0) {
            lexer->position += 5;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "echo", 4) == 0 && 
            !isalnum(lexer->source[lexer->position + 4])) {
            add_token(lexer, TOKEN_ECHO, NULL);
            lexer->position += 4;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "for", 3) == 0 && 
            !isalnum(lexer->source[lexer->position + 3])) {
            add_token(lexer, TOKEN_FOR, NULL);
            lexer->position += 3;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "if", 2) == 0 && 
            !isalnum(lexer->source[lexer->position + 2])) {
            add_token(lexer, TOKEN_IF, NULL);
            lexer->position += 2;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "else", 4) == 0 && 
            !isalnum(lexer->source[lexer->position + 4])) {
            add_token(lexer, TOKEN_ELSE, NULL);
            lexer->position += 4;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "foreach", 7) == 0 && 
            !isalnum(lexer->source[lexer->position + 7])) {
            add_token(lexer, TOKEN_FOREACH, NULL);
            lexer->position += 7;
            continue;
        }

        if (strncmp(&lexer->source[lexer->position], "as", 2) == 0 && 
            !isalnum(lexer->source[lexer->position + 2])) {
            add_token(lexer, TOKEN_AS, NULL);
            lexer->position += 2;
            continue;
        }

        if (c == '$') {
            int start = lexer->position + 1;
            while (isalnum(lexer->source[lexer->position + 1])) {
                lexer->position++;
            }
            int length = lexer->position - start + 1;
            char* value = malloc(length + 1);
            strncpy(value, &lexer->source[start], length);
            value[length] = '\0';
            add_token(lexer, TOKEN_VARIABLE, value);
            lexer->position++;
            free(value);
            continue;
        }

        if (isdigit(c)) {
            int start = lexer->position;
            while (isdigit(lexer->source[lexer->position])) {
                lexer->position++;
            }
            int length = lexer->position - start;
            char* value = malloc(length + 1);
            strncpy(value, &lexer->source[start], length);
            value[length] = '\0';
            add_token(lexer, TOKEN_NUMBER, value);
            free(value);
            continue;
        }

        if (c == ';') {
            add_token(lexer, TOKEN_SEMICOLON, NULL);
            lexer->position++;
            continue;
        }

        if (c == '=') {
            if (lexer->source[lexer->position + 1] == '>') {
                add_token(lexer, TOKEN_ARROW, NULL);
                lexer->position += 2;
            } else {
                add_token(lexer, TOKEN_EQUALS, NULL);
                lexer->position++;
            }
            continue;
        }

        if (c == '+') {
            add_token(lexer, TOKEN_PLUS, NULL);
            lexer->position++;
            continue;
        }

        if (c == '*') {
            add_token(lexer, TOKEN_MULTIPLY, NULL);
            lexer->position++;
            continue;
        }

        if (c == '/') {
            add_token(lexer, TOKEN_DIVIDE, NULL);
            lexer->position++;
            continue;
        }

        if (c == '-') {
            add_token(lexer, TOKEN_MINUS, NULL);
            lexer->position++;
            continue;
        }

        if (c == '(') {
            add_token(lexer, TOKEN_OPEN_PAREN, NULL);
            lexer->position++;
            continue;
        }

        if (c == ')') {
            add_token(lexer, TOKEN_CLOSE_PAREN, NULL);
            lexer->position++;
            continue;
        }

        if (c == '{') {
            add_token(lexer, TOKEN_OPEN_BRACE, NULL);
            lexer->position++;
            continue;
        }

        if (c == '}') {
            add_token(lexer, TOKEN_CLOSE_BRACE, NULL);
            lexer->position++;
            continue;
        }

        if (c == '<') {
            add_token(lexer, TOKEN_LESS, NULL);
            lexer->position++;
            continue;
        }

        if (c == '>') {
            add_token(lexer, TOKEN_GREATER, NULL);
            lexer->position++;
            continue;
        }

        if (c == '[') {
            add_token(lexer, TOKEN_OPEN_BRACKET, NULL);
            lexer->position++;
            continue;
        }

        if (c == ']') {
            add_token(lexer, TOKEN_CLOSE_BRACKET, NULL);
            lexer->position++;
            continue;
        }

        if (c == ',') {
            add_token(lexer, TOKEN_COMMA, NULL);
            lexer->position++;
            continue;
        }

        if (c == '"') {
            int start = ++lexer->position;
            while (lexer->source[lexer->position] != '"' && 
                   lexer->source[lexer->position] != '\0') {
                lexer->position++;
            }
            int length = lexer->position - start;
            char* value = malloc(length + 1);
            strncpy(value, &lexer->source[start], length);
            value[length] = '\0';
            add_token(lexer, TOKEN_STRING, value);
            lexer->position++;
            free(value);
            continue;
        }

        add_token(lexer, TOKEN_UNKNOWN, NULL);
        lexer->position++;
    }
    add_token(lexer, TOKEN_EOF, NULL);
}