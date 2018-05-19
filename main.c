#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Fila.h"
#include "Tupla.h"

#define TAM_JANELA 8
#define TAM_BUFFER 4

void salvarArq(FILE *arq, int *bin);
void encontrar(Fila jan, Fila buff, int *pos, int *tam, int *car);
void comprimir(FILE *tmp, FILE *comp);

int criarArqTmp(FILE *original);
int *limparBuffer(int *buff);
int *converteBinario(int num, int *buff);

char *rstrstr(char *procurarei, char *procurando);

int main()
{
    FILE *arq, *tmp, *comp;
    char entrada[255], saida[255];

    printf("INFORME O NOME DO ARQUIVO QUE DESEJA COMPRIMIR: ");
    scanf("%s", entrada);

    printf("INFORME O NOME DO ARQUIVO DE SAIDA: ");
    scanf("%s", saida);

    // Arquivo temporario para armazenar string de "zeros e uns"
    if (!(arq = fopen(entrada, "rb")))
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo %s", entrada);
        return EXIT_FAILURE;
    }
    criarArqTmp(arq);
    fclose(arq);

    if (!(tmp = fopen(".lz77.tmp", "r")))
    {
        fprintf(stderr, "Nao foi possivel abrir o arquivo temporario");
        return EXIT_FAILURE;
    }

    if (!(comp = fopen(saida, "w")))
    {
        fprintf(stderr, "Nao foi possivel comprimir o arquivo");
        fclose(tmp);
        return EXIT_FAILURE;
    }

    comprimir(tmp, comp);
    fclose(tmp);
    fclose(comp);

    return EXIT_SUCCESS;
}

/**
 * Funcao responsavel por percorrer o arquivo
 * temporario, deslizar a janela/buffer e
 * salvar as tuplas no arquivo de saida/comprimido
 *
 * @param tmp  Arquivo temporario
 * @param comp Arquivo de saida
 */
void comprimir(FILE *tmp, FILE *comp)
{
    Fila buffer = iniFila();
    Fila janela = iniFila();
    Tupla tupla = iniTupla();

    int pos = 0;
    int tam = 0;
    int car = '0';

    // Preenche o buffer com os primeiros simbolos
    for(int i = 0; i < TAM_BUFFER; i++)
        setFila(buffer, fgetc(tmp));

    int c = fgetc(tmp);

    while (c != EOF)
    {
        // Encontrar pos, tam e car
        encontrar(janela, buffer, &pos, &tam, &car);

        // Deslocar tam + 1 simbolos
        for (int i = 0; i < tam + 1; i++)
        {
            // Adiciona na janela o primeiro simbolo do buffer
            // e descarta esse simbolo do buffer
            setFila(janela, getFila(buffer));

            // Descarta o primeiro simbolo da
            // janela quando ela estiver cheia
            if (lengthFila(janela) > TAM_JANELA)
                getFila(janela);

            // Adiciona no buffer o proximo simbolo da fonte de dados
            setFila(buffer, c);

            // Le o proximo simbolo da fonte de dados
            c = fgetc(tmp);
        }

        // Armazena a tupla encontrada
        setTupla(tupla, pos, tam, car);

        // Salvar tupla no arquivo comprimido
        saveTupla(comp, tupla);

        printf("POS: %d\n", pos);
        printf("TAM: %d\n", tam);
        printf("CAR: %c\n\n", car);
    }

    // Quando sair do while, ainda faltara uma execucao para concluir
    encontrar(janela, buffer, &pos, &tam, &car);
    setTupla(tupla, pos, tam, car);
    saveTupla(comp, tupla);

    // Liberar a memoria
    freeTupla(tupla);
    freeFila(buffer);
    freeFila(janela);

    printf("POS: %d\n", pos);
    printf("TAM: %d\n", tam);
    printf("CAR: %c\n\n", car);
}

/**
 * Funcao responsavel por determinar qual
 * eh a tupla para cada deslizamento da janela/buffer
 *
 * @param jan
 * @param buff
 * @param pos
 * @param tam
 * @param car
 */
void encontrar(Fila jan, Fila buff, int *pos, int *tam, int *car)
{
    // Copia do buffer em vetor de string
    char *buffer = getVetFila(buff);

    // Outra copia do buffer em vetor de string (gambiarra)
    char *cp_buf = getVetFila(buff);

    // Copia da janela em vetor de string
    char *janela = getVetFila(jan);

    // Variavel auxiliar temporaria
    char *tmp;

    for (int i = TAM_BUFFER - 1; i >= 0; i--)
    {
        // Remove o ultimo elemento do buffer para nova comparacao
        buffer[i+1] = '\0';

        // Verifica se ha alguma ocorrencia do buffer na janela
        // Retorna a ultima ocorrencia, caso exista, senao retorna NULL
        if ((tmp = rstrstr(janela, buffer)) != NULL)
        {
            // Atualiza o tamanho da string
            *tam = (int) strlen(buffer);

            // Posicao da janela onde foi encontrado a
            // ocorrencia do buffer (da direita para a esquerda)
            *pos = (int) strlen(tmp);

            // Atualiza o caracter de quebra
            *car = cp_buf[*tam];
            break;
        }
    }

    // Liberar a memoria
    free(cp_buf);
    free(buffer);
    free(janela);
}

/**
 * Converte um inteiro para binario com 8 bits
 *
 * @param num
 * @param buff
 * @return
 */
int *converteBinario(int num, int *buff)
{
    for (int i = 7; num != 0; num /= 2, i--)
        buff[i] = num % 2;
    return buff;
}

/**
 * Preenche todas as posicoes do buffer com 0
 *
 * @param buff
 * @return
 */
int *limparBuffer(int *buff)
{
    for (int i = 0; i < 8; i++)
        buff[i] = 0;
    return buff;
}

/**
 * Salva o conteudo do buffer no arquivo
 *
 * @param arq
 * @param bin
 */
void salvarArq(FILE *arq, int *bin)
{
    for (int i = 0; i < 8; i++)
        fprintf(arq, "%d", bin[i]);
}

/**
 * Le os dados do arquivo original, converte para
 * binario e salva o binario em um arquivo temporario
 *
 * @param original
 * @return
 */
int criarArqTmp(FILE *original)
{
    FILE *tmp;
    int buff[8];
    unsigned char info;

    if (!(tmp = fopen(".lz77.tmp", "w")))
        return 0;

    while(!feof(original))
        if (fread(&info, 1, 1, original))
            salvarArq(tmp, converteBinario(info, limparBuffer(buff)));

    fclose(tmp);
    return 1;
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