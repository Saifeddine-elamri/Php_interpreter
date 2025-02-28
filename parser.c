#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

Parser* parser_create(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->position = 0;
    parser->var_count = 0;
    parser->var_capacity = 10;
    parser->variables = malloc(sizeof(Variable) * parser->var_capacity);
    for (int i = 0; i < parser->var_capacity; i++) {
        parser->variables[i].array = NULL;
    }
    return parser;
}

void parser_free(Parser* parser) {
    for (int i = 0; i < parser->var_count; i++) {
        free(parser->variables[i].name);
        free(parser->variables[i].value);
        if (parser->variables[i].array) {
            for (int j = 0; j < parser->variables[i].array->count; j++) {
                free(parser->variables[i].array->items[j].key);
                free(parser->variables[i].array->items[j].value);
            }
            free(parser->variables[i].array->items);
            free(parser->variables[i].array);
        }
    }
    free(parser->variables);
    free(parser);
}

static Token current_token(Parser* parser) {
    return parser->lexer->tokens[parser->position];
}

static void advance(Parser* parser) {
    parser->position++;
}

static char* get_variable_value(Parser* parser, const char* name) {
    for (int i = 0; i < parser->var_count; i++) {
        if (strcmp(parser->variables[i].name, name) == 0) {
            return parser->variables[i].value;
        }
    }
    return NULL;
}

static Array* get_variable_array(Parser* parser, const char* name) {
    for (int i = 0; i < parser->var_count; i++) {
        if (strcmp(parser->variables[i].name, name) == 0) {
            return parser->variables[i].array;
        }
    }
    return NULL;
}

static void set_variable(Parser* parser, const char* name, const char* value, Array* array) {
    for (int i = 0; i < parser->var_count; i++) {
        if (strcmp(parser->variables[i].name, name) == 0) {
            free(parser->variables[i].value);
            if (parser->variables[i].array) {
                for (int j = 0; j < parser->variables[i].array->count; j++) {
                    free(parser->variables[i].array->items[j].key);
                    free(parser->variables[i].array->items[j].value);
                }
                free(parser->variables[i].array->items);
                free(parser->variables[i].array);
            }
            parser->variables[i].value = value ? strdup(value) : NULL;
            parser->variables[i].array = array;
            return;
        }
    }
    
    if (parser->var_count >= parser->var_capacity) {
        parser->var_capacity *= 2;
        parser->variables = realloc(parser->variables, 
                                  sizeof(Variable) * parser->var_capacity);
        for (int i = parser->var_count; i < parser->var_capacity; i++) {
            parser->variables[i].array = NULL;
        }
    }
    
    parser->variables[parser->var_count].name = strdup(name);
    parser->variables[parser->var_count].value = value ? strdup(value) : NULL;
    parser->variables[parser->var_count].array = array;
    parser->var_count++;
}

static char* operate_values(const char* val1, const char* val2, TokenType operator) {
    double num1 = atof(val1);
    double num2 = atof(val2);
    double result;
    
    switch (operator) {
        case TOKEN_PLUS:
            result = num1 + num2;
            break;
        case TOKEN_MULTIPLY:
            result = num1 * num2;
            break;
        case TOKEN_DIVIDE:
            if (num2 == 0) {
                fprintf(stderr, "Erreur: Division par zéro\n");
                return strdup("0");
            }
            result = num1 / num2;
            break;
        case TOKEN_MINUS:
            result = num1 - num2;
            break;
        default:
            return strdup("0");
    }
    
    char* buffer = malloc(32);
    snprintf(buffer, 32, "%.2f", result);
    return buffer;
}

static int evaluate_condition(Parser* parser, char* left, TokenType operator, char* right) {
    double val1 = atof(left);
    double val2 = atof(right);
    
    switch (operator) {
        case TOKEN_LESS:
            return val1 < val2;
        case TOKEN_GREATER:
            return val1 > val2;
        default:
            return 0;
    }
}

static void parse_expression(Parser* parser);

static void parse_block(Parser* parser) {
    while (current_token(parser).type != TOKEN_CLOSE_BRACE && 
           current_token(parser).type != TOKEN_EOF) {
        parse_expression(parser);
    }
    if (current_token(parser).type == TOKEN_CLOSE_BRACE) {
        advance(parser);
    }
}

static Array* parse_array(Parser* parser) {
    Array* array = malloc(sizeof(Array));
    array->count = 0;
    array->capacity = 10;
    array->items = malloc(sizeof(ArrayItem) * array->capacity);
    
    advance(parser); // [
    
    while (current_token(parser).type != TOKEN_CLOSE_BRACKET && 
           current_token(parser).type != TOKEN_EOF) {
        if (array->count >= array->capacity) {
            array->capacity *= 2;
            array->items = realloc(array->items, sizeof(ArrayItem) * array->capacity);
        }
        
        char* key = NULL;
        char* value = current_token(parser).type == TOKEN_VARIABLE ? 
                    get_variable_value(parser, current_token(parser).value) : 
                    current_token(parser).value;
        advance(parser);
        
        if (current_token(parser).type == TOKEN_ARROW) {
            key = strdup(value);
            advance(parser); // =>
            value = current_token(parser).type == TOKEN_VARIABLE ? 
                   get_variable_value(parser, current_token(parser).value) : 
                   current_token(parser).value;
            advance(parser);
        }
        
        array->items[array->count].key = key ? strdup(key) : NULL;
        array->items[array->count].value = strdup(value);
        array->count++;
        
        if (current_token(parser).type == TOKEN_COMMA) {
            advance(parser); // ,
        }
    }
    
    if (current_token(parser).type == TOKEN_CLOSE_BRACKET) {
        advance(parser); // ]
    }
    
    return array;
}

static void parse_expression(Parser* parser) {
    Token token = current_token(parser);
    
    if (token.type == TOKEN_ECHO) {
        advance(parser);
        token = current_token(parser);
        
        if (token.type == TOKEN_STRING) {
            printf("%s", token.value);
            advance(parser);
        } else if (token.type == TOKEN_VARIABLE) {
            char* value = get_variable_value(parser, token.value);
            if (value) {
                printf("%s", value);
            }
            advance(parser);
        }
        
        if (current_token(parser).type == TOKEN_SEMICOLON) {
            advance(parser);
        }
    } else if (token.type == TOKEN_VARIABLE) {
        char* var_name = strdup(token.value);
        advance(parser);
        
        if (current_token(parser).type == TOKEN_EQUALS) {
            advance(parser);
            token = current_token(parser);
            
            char* value = NULL;
            Array* array = NULL;
            
            if (token.type == TOKEN_STRING || token.type == TOKEN_NUMBER) {
                value = strdup(token.value);
                advance(parser);
            } else if (token.type == TOKEN_VARIABLE) {
                value = strdup(get_variable_value(parser, token.value));
                advance(parser);
            } else if (token.type == TOKEN_OPEN_BRACKET) {
                array = parse_array(parser);
            }
            
            TokenType operator = current_token(parser).type;
            if (operator == TOKEN_PLUS || operator == TOKEN_MULTIPLY || 
                operator == TOKEN_DIVIDE || operator == TOKEN_MINUS) {
                advance(parser);
                token = current_token(parser);
                char* val2 = (token.type == TOKEN_NUMBER) ? token.value : 
                            get_variable_value(parser, token.value);
                char* result = operate_values(value, val2, operator);
                free(value);
                value = result;
                advance(parser);
            }
            
            set_variable(parser, var_name, value, array);
            free(var_name);
            free(value);
            
            if (current_token(parser).type == TOKEN_SEMICOLON) {
                advance(parser);
            }
        }
    } else if (token.type == TOKEN_IF) {
        advance(parser);
        if (current_token(parser).type == TOKEN_OPEN_PAREN) {
            advance(parser);
            
            char* left = current_token(parser).type == TOKEN_VARIABLE ? 
                        get_variable_value(parser, current_token(parser).value) : 
                        current_token(parser).value;
            advance(parser);
            
            TokenType operator = current_token(parser).type;
            advance(parser);
            
            char* right = current_token(parser).type == TOKEN_VARIABLE ? 
                         get_variable_value(parser, current_token(parser).value) : 
                         current_token(parser).value;
            advance(parser);
            
            if (current_token(parser).type == TOKEN_CLOSE_PAREN) {
                advance(parser);
                
                int condition = evaluate_condition(parser, left, operator, right);
                
                if (current_token(parser).type == TOKEN_OPEN_BRACE) {
                    advance(parser);
                    
                    if (condition) {
                        parse_block(parser);
                    } else {
                        int depth = 1;
                        while (depth > 0 && current_token(parser).type != TOKEN_EOF) {
                            if (current_token(parser).type == TOKEN_OPEN_BRACE) depth++;
                            if (current_token(parser).type == TOKEN_CLOSE_BRACE) depth--;
                            advance(parser);
                        }
                    }
                    
                    if (current_token(parser).type == TOKEN_ELSE) {
                        advance(parser);
                        if (current_token(parser).type == TOKEN_OPEN_BRACE) {
                            advance(parser);
                            if (!condition) {
                                parse_block(parser);
                            } else {
                                int depth = 1;
                                while (depth > 0 && current_token(parser).type != TOKEN_EOF) {
                                    if (current_token(parser).type == TOKEN_OPEN_BRACE) depth++;
                                    if (current_token(parser).type == TOKEN_CLOSE_BRACE) depth--;
                                    advance(parser);
                                }
                            }
                        }
                    }
                }
            }
        }
    } else if (token.type == TOKEN_FOR) {
        advance(parser);
        if (current_token(parser).type == TOKEN_OPEN_PAREN) {
            advance(parser);
            
            char* var_name = current_token(parser).value;
            advance(parser);
            if (current_token(parser).type == TOKEN_EQUALS) {
                advance(parser);
                char* init_value = current_token(parser).value;
                set_variable(parser, var_name, init_value, NULL);
                advance(parser);
                if (current_token(parser).type == TOKEN_SEMICOLON) {
                    advance(parser);
                }
            }
            
            char* cond_var = current_token(parser).value;
            advance(parser);
            TokenType cond_op = current_token(parser).type;
            advance(parser);
            char* cond_limit = current_token(parser).value;
            advance(parser);
            if (current_token(parser).type == TOKEN_SEMICOLON) {
                advance(parser);
            }
            
            char* inc_var = current_token(parser).value;
            advance(parser);
            if (current_token(parser).type == TOKEN_EQUALS) {
                advance(parser);
                advance(parser);
                TokenType inc_op = current_token(parser).type;
                advance(parser);
                char* inc_value = current_token(parser).value;
                advance(parser);
                if (current_token(parser).type == TOKEN_CLOSE_PAREN) {
                    advance(parser);
                    
                    if (current_token(parser).type == TOKEN_OPEN_BRACE) {
                        advance(parser);
                        
                        int start_pos = parser->position;
                        while (evaluate_condition(parser, 
                                               get_variable_value(parser, cond_var), 
                                               cond_op, 
                                               cond_limit)) {
                            parse_block(parser);
                            char* current_val = get_variable_value(parser, inc_var);
                            char* new_val = operate_values(current_val, inc_value, inc_op);
                            set_variable(parser, inc_var, new_val, NULL);
                            free(new_val);
                            parser->position = start_pos;
                        }
                        int depth = 1;
                        while (depth > 0 && current_token(parser).type != TOKEN_EOF) {
                            if (current_token(parser).type == TOKEN_OPEN_BRACE) depth++;
                            if (current_token(parser).type == TOKEN_CLOSE_BRACE) depth--;
                            advance(parser);
                        }
                    }
                }
            }
        }
    } else if (token.type == TOKEN_FOREACH) {
        advance(parser);
        if (current_token(parser).type == TOKEN_OPEN_PAREN) {
            advance(parser);
            
            char* array_var = current_token(parser).value;
            advance(parser);
            Array* array = get_variable_array(parser, array_var);
            
            if (current_token(parser).type == TOKEN_AS) {
                advance(parser); // Passer "as"
                
                char* value_var = current_token(parser).value;
                advance(parser);
                
                char* key_var = NULL;
                if (current_token(parser).type == TOKEN_ARROW) {
                    advance(parser); // =>
                    key_var = value_var;
                    value_var = current_token(parser).value;
                    advance(parser);
                }
                
                if (current_token(parser).type == TOKEN_CLOSE_PAREN) {
                    advance(parser);
                    
                    if (current_token(parser).type == TOKEN_OPEN_BRACE) {
                        advance(parser);
                        
                        if (array) {
                            int start_pos = parser->position;
                            for (int i = 0; i < array->count; i++) {
                                if (key_var) {
                                    set_variable(parser, key_var, array->items[i].key, NULL);
                                }
                                set_variable(parser, value_var, array->items[i].value, NULL);
                                parse_block(parser);
                                parser->position = start_pos;
                            }
                        }
                        int depth = 1;
                        while (depth > 0 && current_token(parser).type != TOKEN_EOF) {
                            if (current_token(parser).type == TOKEN_OPEN_BRACE) depth++;
                            if (current_token(parser).type == TOKEN_CLOSE_BRACE) depth--;
                            advance(parser);
                        }
                    }
                }
            }
        }
    }
}

void parser_run(Parser* parser) {
    while (current_token(parser).type != TOKEN_EOF) {
        parse_expression(parser);
    }
}