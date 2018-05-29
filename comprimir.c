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
    FILE *temp, *arq;

    if (argc != 3)
    {
        printf("Usage: %s compfile file\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!(temp = fopen("temp.txt", "w")))
    {
        fprintf(stderr, "Error opening file 'temp.txt': ");
        perror("");
        return EXIT_FAILURE;
    }

    if (!(arq = fopen(argv[2], "rb")))
    {
        fprintf(stderr, "Error opening file '%s': ", argv[2]);
        perror("");
        fclose(temp);
        remove("temp.txt");
        return EXIT_FAILURE;
    }

    preencherArquivo(arq, temp);

    fclose(arq);
    fclose(temp);

    temp = fopen("temp.txt", "r");

    LZComprimir(temp, argv[2], argv[1]);

    fclose(temp);
    remove("temp.txt");

    return EXIT_SUCCESS;
}