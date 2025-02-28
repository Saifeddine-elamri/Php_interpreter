#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,
    TOKEN_ECHO,
    TOKEN_STRING,
    TOKEN_VARIABLE,
    TOKEN_SEMICOLON,
    TOKEN_EQUALS,
    TOKEN_NUMBER,
    TOKEN_PLUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MINUS,
    TOKEN_FOR,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FOREACH,
    TOKEN_AS,          // Nouveau token pour "as"
    TOKEN_OPEN_PAREN,  // (
    TOKEN_CLOSE_PAREN, // )
    TOKEN_OPEN_BRACE,  // {
    TOKEN_CLOSE_BRACE, // }
    TOKEN_LESS,        // <
    TOKEN_GREATER,     // >
    TOKEN_ARROW,       // =>
    TOKEN_OPEN_BRACKET,// [
    TOKEN_CLOSE_BRACKET,// ]
    TOKEN_COMMA,       // ,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

typedef struct {
    char* source;
    int position;
    Token* tokens;
    int token_count;
    int token_capacity;
} Lexer;

Lexer* lexer_create(const char* source);
void lexer_free(Lexer* lexer);
void lexer_tokenize(Lexer* lexer);

#endif