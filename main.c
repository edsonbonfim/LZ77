/**
 * Programacao com Arquivos
 *
 * Compactador de Arquivos (Algoritimo LZ77)
 *
 * Alline Ribeiro
 * Edson Onildo
 * Isabela Carvalho
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lz77.h"
#define N 256

/**
 * Funcao responsavel por deslizar o buffer/janela,
 * encontrar a tupla, e salva-la no arquivo comprimido
 *
 * @param temp Fonte de dados
 * @param comp Arquivo comprimido
 */
void comprimir(FILE *temp, char *nomeOriginal, char *nomeComprimido)
{
    // Cria arquivo comprimido (ainda vazio)
    FILE *comp = abrir(nomeComprimido, "wb");

    // Salva no arquivo comprimido o nome do arquivo original
    // que sera utilizado na hora de descomprimir
    fwrite(nomeOriginal, 1, N, comp);

    int cont = 1;

    // Conta a quantidade de caracteres no arquivo temporario
    while(fgetc(temp) != EOF)
        cont++;

    // Salva no arquivo comprimido o tamanho do arquivo temporario
    // que sera utilizado na hora de descomprimir
    fwrite(&cont, 1, sizeof(int), comp);

    // Rebobina o arquivo temporario
    rewind(temp);

    // Aloca um vetor para armazenar a string do arquivo temporario
    char *str = (char*) malloc(cont * sizeof(char));

    cont = 0;

    // Preenche o vetor com as strings do arquivo temporario
    while((str[cont] = (char) fgetc(temp)) != EOF)
        cont++;

    str[cont] = '\0';

    char *buffer = (char *) malloc((TAM_BUF + 1) * sizeof(char));
    char *janela = (char *) malloc((TAM_JAN + 1) * sizeof(char));

    buffer[TAM_BUF] = '\0';
    janela[0] = '\0';

    Tupla tupla = (Tupla) malloc(sizeof(struct tupla));

    // Preenche o buffer com os primeiros simbolos
    // (Para que tudo funcione bem, o buffer deve comecar preenchido)
    strncpy(buffer, str, TAM_BUF);
    str += TAM_BUF;

    while(strlen(str) != 0)
    {
        // Encontrar a tupla, salva no arquivo e desliza a janela
        encontrar(janela, buffer, tupla);
        salvarTupla(comp, tupla);
        deslizar(&str, &janela, &buffer, tupla->tam+1);

        // Debug
        printf("(%d %d %c)\n", tupla->pos, tupla->tam, tupla->car);
    }

    // Quando sair do while, ainda faltara uma execucao para concluir
    encontrar(janela, buffer, tupla);
    salvarTupla(comp, tupla);
    printf("(%d %d %c)\n", tupla->pos, tupla->tam, tupla->car);

    // Esvaziar o buffer
    if (strlen(buffer) != TAM_BUF)
    {
        deslizar(&str, &janela, &buffer, tupla->tam+1);
        encontrar(janela, buffer, tupla);
        salvarTupla(comp, tupla);
    }

    free(buffer);
    free(janela);

    fclose(comp);
 }

 /**
  * Funcao responsavel por ler os dados do arquivo comprimido,
  * descomprimi-los e coloca-los em um arquivo temporario
  *
  * @param comp
  * @param temp
  */
 void descomprimir(FILE *comp)
 {
     int cont;
     char nomeOriginal[256], *aux, ch[2];

     // Le o nome do arquivo original que foi salvo no arquivo comprimido
     fread(nomeOriginal, 1, N, comp);

     // Cria o arquivo original
     FILE *saida = abrir(nomeOriginal, "w");

     // Le o tamanho da string de binarios que foi salvo no arquivo temporario
     fread(&cont, 1, sizeof(int), comp);

     // Aloca o vetor que contera a string de binarios
     char *vet = (char *) malloc(cont * sizeof(char));

     // Aloca vetor de tuplas
     Tupla tupla = (Tupla) malloc(sizeof(struct tupla));

     //fechamento da cadeia de caracteres da janela
     vet[0] = '\0';

     //fechamento da string que contera o ultimo caracter lido do arquivo
     ch[1] = '\0';

     //loop para ler os dados do arquivo comprimido
     while (!feof(comp))
     {
         //Le a tupla do arquivo
         if (fread(tupla, 1, sizeof(struct tupla), comp))
         {
             //Se a tupla for lida, uma copia do vetor que contem os dados da janela
             //e feita para o vetor auxiliar
             aux = vet;

             //O ponteiro apontara para a posicao da janela onde esta a substring
             aux += strlen(vet) - tupla->pos;

             //A janela e atualizada atraves da concatenacao do caracter copiado da substring
             for (int i = 0; i < tupla->tam; i++)
             {
                 ch[0] = aux[i];
                 strcat(vet, ch);
             }

             //O caracter de quebra e colocado na janela
             ch[0] = (char) tupla->car;
             strcat(vet, ch);
         }
     }

     //Vetor que contera os bytes a serem convertidos em caracteres para a descompressao
     char novoVet[BIT+1];

     //fechamento da cadeia de caracteres do byte que sera convertido
     novoVet[BIT] = '\0';

     int i = 0; //indice para percorrer o novo vetor
     char elem; //caracter contera os bits que formarao os bytes a serem convertidos

     //loop para converter os bytes e escreve-los no arquivo
     while((elem = vet[0]) != '\0')
     {
         novoVet[i] = elem; //cada bit e colocado no vetor que conterao os bytes
         vet = vet+1; //o ponteiro do vetor com os dados da janela e colocado na proxima posicao
         i++;

         /* Quando o vetor com os bits atingir o tamanho 8, e porque
          *  o byte esta completo e podera ser convertido
          */
         if (i == 8)
         {
             i = 0;
             //Apos a conversao, o caracter e impresso no arquivo temporario
             fprintf(saida, "%c", (char) converteDecimal(novoVet));
         }
     }

     fclose(saida);
 }

int main(void)
{
    char entrada[N], nomeComp[N];

    int opcao;

    //Menu de opcoes
    printf("1 - Comprimir\n");
    printf("2 - Descomprimir\n");
    printf("3 - Sair\n");
    scanf("%d", &opcao);

    FILE *temp; //ponteiro para o arquivo temporario
    FILE *comp; //ponteiro para o arquvivo comprimido
    FILE *arq; //ponteiro para o arquivo de entrada

    switch(opcao)
    {
        case 1:
            printf("Digite o arquivo de entrada: ");
            scanf("%s", entrada);

            temp = abrir("temp.txt", "w"); //cria o arquivo temporario
            arq = abrir(entrada, "rb"); //abre o arquivo de entrada

            preencherArquivo(arq, temp); //preenhce o arquivo temporario

            fclose(arq);
            fclose(temp);
            temp = abrir("temp.txt", "r"); //abre o arquivo temporario para leitura

            printf("Digite o nome do arquivo a ser comprimido: ");
            scanf("%s", nomeComp); //le o nome do arquivo comprimido

            if (!strstr(nomeComp, ".comp"))
                strcat(nomeComp, ".comp");

            comprimir(temp, entrada, nomeComp); //chamada da funcao para comprimir o arquivo

            printf("\nArquivo comprimido com sucesso!\n");

            fclose(temp); //fecha o arquivo temporario

            remove("temp.txt");
            break;

        case 2:
            printf("Digite o nome do arquivo a ser descomprimido: ");
            scanf("%s", nomeComp); //le o nome do arquivo comprimido

            comp = abrir(nomeComp, "rb"); //abre o arquivo comprimido
            descomprimir(comp); //chamada da funcao para descomprimir o arquivo
            break;

        default:
            return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
