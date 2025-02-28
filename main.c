#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <fichier.php>\n", argv[0]);
        printf("Exemple: %s script.php\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    char* php_code = read_file(filename);
    
    if (!php_code) {
        return 1;
    }
    
    Lexer* lexer = lexer_create(php_code);
    lexer_tokenize(lexer);
    
    Parser* parser = parser_create(lexer);
    parser_run(parser);
    
    parser_free(parser);
    lexer_free(lexer);
    free(php_code);
    
    return 0;
}