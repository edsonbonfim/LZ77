#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lz77.h"

#define N 256
#define BIT 8
#define TAM_BUF 8
#define TAM_JAN 32

typedef struct tupla
{
    unsigned char pos;
    unsigned char tam;
    unsigned char car;
} *Tupla;

void LZComprimir(FILE *temp, char *nomeOriginal, char *nomeComprimido);
void LZDescomprimir(FILE *comp);
void preencherArquivo(FILE *origem, FILE *destino);
void deslizar(char **str, char **janela, char **buffer, int tam);
void encontrar(char *janela, char *buffer, Tupla tupla);
void salvarTupla(FILE *arq, Tupla t);

FILE *abrir(char *entrada, char *modo);

int *converte(int num);
int converteDecimal(char *num);

char *rstrstr(char *procurarei, char *procurando);

/**
 * Funcao responsavel por deslizar o buffer/janela,
 * encontrar a tupla, e salva-la no arquivo comprimido
 *
 * @param temp           Fonte de dados
 * @param nomeComprimido Nome do arquivo comprimido
 * @param nomeOriginal   Nome do arquivo original
 */
void LZComprimir(FILE *temp, char *nomeOriginal, char *nomeComprimido)
{
    // Cria arquivo comprimido (ainda vazio)
    FILE *comp = abrir(nomeComprimido, "wb");

    // Salva no arquivo comprimido o nome do arquivo original
    // que sera utilizado na hora de LZDescomprimir
    fwrite(nomeOriginal, 1, N, comp);

    int cont = 1;

    // Conta a quantidade de caracteres no arquivo temporario
    while(fgetc(temp) != EOF)
        cont++;

    printf("%d\n\n", cont);

    // Salva no arquivo comprimido o tamanho do arquivo temporario
    // que sera utilizado na hora de LZDescomprimir
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
 * @param comp Arquivo comprimido
 */
void LZDescomprimir(FILE *comp)
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
