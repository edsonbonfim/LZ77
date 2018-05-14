#include <stdlib.h>
#include <stdio.h>
#include "Tupla.h"
#include "Fila.h"

struct tupla
{
    int cont;
    Fila pos, tam, car;
};

Tupla iniTupla(void)
{
    Tupla t = (Tupla) malloc(sizeof(struct tupla));
    t->pos = iniFila();
    t->tam = iniFila();
    t->car = iniFila();
    t->cont = 0;
    return t;
}

Tupla freeTupla(Tupla tupla)
{
    tupla->pos = freeFila(tupla->pos);
    tupla->tam = freeFila(tupla->tam);
    tupla->car = freeFila(tupla->car);
    free(tupla);
    return NULL;
}

void setTupla(Tupla t, int pos, int tam, int car)
{
    t->cont++;
    setFila(t->pos, pos);
    setFila(t->tam, tam);
    setFila(t->car, car);
}

int getPosTupla(Tupla t)
{
    return getFila(t->pos);
}

int getTamTupla(Tupla t)
{
    return getFila(t->tam);
}

int getCarTupla(Tupla t)
{
    return getFila(t->car);
}

void getTupla(Tupla t, int *pos, int *tam, int *car)
{
    *pos = getPosTupla(t);
    *tam = getTamTupla(t);
    *car = getCarTupla(t);
    t->cont--;
}

void saveTupla(FILE *arq, Tupla t)
{
    int pos, tam, car;
    getTupla(t, &pos, &tam, &car);
    fprintf(arq, "%d%d%c", pos, tam, car);
}