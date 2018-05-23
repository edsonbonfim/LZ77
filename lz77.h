#ifndef LZ77_LZ77_H
#define LZ77_LZ77_H

#define BIT 8
#define TAM_BUF 8
#define TAM_JAN 32

struct tupla
{
    unsigned char pos;
    unsigned char tam;
    unsigned char car;
};

typedef struct tupla *Tupla;

FILE *abrir(char *entrada, char *modo);

void deslizar(char **str, char **janela, char **buffer, int tam);

int *converte(int num);

int converteDecimal(char *num);

void preencherArquivo(FILE *origem, FILE *destino);

char *rstrstr(char *procurarei, char *procurando);

void encontrar(char *janela, char *buffer, Tupla tupla);

void salvarTupla(FILE *arq, Tupla t);

#endif