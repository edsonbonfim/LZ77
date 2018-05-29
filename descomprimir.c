/**
 * Compactador de Arquivos (Algoritimo LZ77)
 *
 * @author Alline Ribeiro
 * @author Edson Onildo
 * @author Isabela Carvalho
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz77.h"

int main(int argc, char **argv)
{
    FILE *comp;

    if (argc != 2)
    {
        printf("Usage: %s compfile\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!(comp = fopen(argv[1], "rb")))
    {
        fprintf(stderr, "Error opening file '%s': ", argv[1]);
        perror("");
        return EXIT_FAILURE;
    }

    LZDescomprimir(comp);

    return EXIT_SUCCESS;
}