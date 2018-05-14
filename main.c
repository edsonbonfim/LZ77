#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Fila.h"
#include "Tupla.h"

#define TAM_JANELA 8
#define TAM_BUFFER 4

void salvar_arq(FILE *arq, int *bin);
void encontrar(Fila jan, Fila buff, int *pos, int *tam, int *car);
void comprimir(FILE *tmp, FILE *comp);

int criar_arq_tmp(FILE *original);
int *clear_buffer(int *buff);
int *converte_binario(int num, int *buff);

int main()
{
    FILE *arq, *tmp, *comp;
    char entrada[255], saida[255];

    printf("INFORME O NOME DO ARQUIVO QUE DESEJA COMPRIMIR: ");
    scanf("%s", entrada);

    printf("INFORME O NOME DO ARQUIVO DE SAIDA: ");
    scanf("%s", saida);

    // Arquivo temporario para armazenar string de "zeros e uns"
    arq = fopen(entrada, "rb");
    criar_arq_tmp(arq);
    fclose(arq);

    // Comprimir
    tmp = fopen(".lz77.tmp", "r");
    comp = fopen(saida, "w");
    comprimir(tmp, comp);
    fclose(tmp);
    fclose(comp);

    return EXIT_SUCCESS;
}

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
    }

    // Quando sair do while, ainda faltara uma execucao para concluir
    encontrar(janela, buffer, &pos, &tam, &car);
    setTupla(tupla, pos, TAM_BUFFER, '\0');
    saveTupla(comp, tupla);

    // Liberar a memoria
    freeTupla(tupla);
    freeFila(buffer);
    freeFila(janela);
}

void encontrar(Fila jan, Fila buff, int *pos, int *tam, int *car)
{
    char *buffer = getVetFila(buff);
    char *cp_buf = getVetFila(buff);
    char *janela = getVetFila(jan);
    char *tmp, *tmp2;

    for (int i = TAM_BUFFER - 1; i >= 0; i--)
    {
        buffer[i+1] = '\0';
        tmp = strstr(janela, buffer);
        if (tmp != NULL)
        {
            *tam = (int) strlen(buffer);

            tmp2 = strstr(tmp + *tam, buffer);

            if (tmp2 != NULL)
                *pos = (int) strlen(tmp2);
            else
                *pos = (int) strlen(tmp);

            *car = cp_buf[*tam];
            break;
        }
    }

    // Liberar a memoria
    free(cp_buf);
    free(buffer);
    free(janela);
}

int *converte_binario(int num, int *buff)
{
    for (int i = 7; num != 0; num /= 2, i--)
        buff[i] = num % 2;
    return buff;
}

int *clear_buffer(int *buff)
{
    for (int i = 0; i < 8; i++)
        buff[i] = 0;
    return buff;
}

void salvar_arq(FILE *arq, int *bin)
{
    for (int i = 0; i < 8; i++)
        fprintf(arq, "%d", bin[i]);
}

int criar_arq_tmp(FILE *original)
{
    FILE *tmp;
    int buff[8];
    unsigned char info;

    if (!(tmp = fopen(".lz77.tmp", "w")))
        return 0;

    while(!feof(original))
        if (fread(&info, 1, 1, original))
            salvar_arq(tmp, converte_binario(info, clear_buffer(buff)));

    fclose(tmp);
    return 1;
}