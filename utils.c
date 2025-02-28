#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = malloc(size + 1);
    if (!content) {
        fclose(file);
        perror("Erreur d'allocation mémoire");
        return NULL;
    }

    size_t read_size = fread(content, 1, size, file);
    content[read_size] = '\0';

    fclose(file);
    return content;
}