#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lz77.h"

FILE *abrir(char *entrada, char *modo)
{
    FILE *arq = fopen(entrada, modo);
    if (arq == NULL)
    {
        printf("Nao foi possivel abrir o arquivo!");
        exit(EXIT_FAILURE);
    }

    return arq;
}

int *converte(int num)
{
    int i=BIT-1;

    int *r = (int*) malloc(BIT*sizeof(int));
    if (r == NULL)
    {
        printf("Nao foi possivel alocar a memoria!\n");
        exit(EXIT_FAILURE);
    }

    while (num != 0)
    {
        r[i] = num % 2;
        num = num / 2;
        i--;
    }

    return r;
}

int converteDecimal(char *num)
{
    int ret = 0, j = 0, i = (int) strlen(num);

    while(i--)
        ret += (int) pow(2, j++) * (num[i] - 48);

    return ret;
}

void preencherArquivo(FILE *origem, FILE *destino)
{
    char letra;

    while (!feof(origem))
    {
        if (fread(&letra, 1, 1, origem))
        {
            int *vet = converte((int) letra);

            for (int i=0; i<BIT; i++)
                fprintf(destino, "%d", vet[i]);
        }
    }
}

/**
 * Retorna um ponteiro para a ultima
 * ocorrencia de procurando em procurarei.
 *
 * Caso nenhuma ocorrencia seja encontrada, retorna NULL
 *
 * @param procurarei
 * @param procurando
 * @return
 */
char *rstrstr(char *procurarei, char *procurando)
{
    char *tmp, *tmp2 = NULL;

    while ((tmp = strstr(procurarei, procurando)) != NULL)
    {
        tmp2 = tmp;
        procurarei = tmp2 + strlen(procurando);
    }

    return tmp2;
}

void deslizar(char **str, char **janela, char **buffer, int tam)
{
    for (int i = 0; i < tam; i++)
    {
        // Adiciona na janela o primeiro simbolo do buffer
        // e descarta esse simbolo do buffer
        strncat(*janela, *buffer, 1);
        strcpy(*buffer, *buffer+1);

        // Descarta o primeiro simbolo da
        // janela quando ela estiver cheia
        if (strlen(*janela) > TAM_JAN)
            strcpy(*janela, *janela+1);

        // Adiciona no buffer o proximo simbolo da fonte de dados
        strncat(*buffer, *str, 1);
        *str += 1;
    }
}

/**
 * Funcao responsavel por determinar qual
 * eh a tupla para cada deslizamento da janela/buffer
 *
 * @param janela
 * @param buffuer
 * @param cpBuff
 * @param tupla
 */
void encontrar(char *janela, char *buf, Tupla tupla)
{
    // Variavel auxiliar temporaria
    char *tmp;

    char buffer[TAM_BUF];
    strcpy(buffer, buf);

    char aux[TAM_BUF];
    strcpy(aux, buf);

    for (int i = ((int) strlen(buffer)) - 1; i >= 0; i--)
    {
        // Remove o ultimo elemento do buffer para nova comparacao
        buffer[i+1] = '\0';

        // Verifica se ha alguma ocorrencia do buffer na janela
        // Retorna a ultima ocorrencia, caso exista, senao retorna NULL
        if ((tmp = rstrstr(janela, buffer)) != NULL && strlen(buffer) < TAM_BUF)
        {
            // Posicao da janela onde foi encontrado a
            // ocorrencia do buffer (da direita para a esquerda)
            tupla->pos = (unsigned char) strlen(tmp);

            // Atualiza o tamanho da string
            tupla->tam = (unsigned char) strlen(buffer);

            // Atualiza o caracter de quebra
            tupla->car = (unsigned char) aux[tupla->tam];

            return;
        }
    }

    tupla->pos = 0;
    tupla->tam = 0;
    tupla->car = (unsigned char) buffer[0];
}

/**
 * Salva a tupla em um arquivo
 *
 * @param arq Arquivo
 * @param t   Tupla
 */
void salvarTupla(FILE *arq, Tupla t)
{
    fwrite(t, 1, sizeof(struct tupla), arq);
}
