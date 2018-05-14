#include <stdio.h>
#include <stdlib.h>
#include "Lista.h"
#include "Fila.h"

struct Fila
{
    Lista lista;
};

int isEmptyFila(Fila fila)
{
    return vazia_lista(fila->lista);
}

Fila iniFila(void)
{
    Fila fila = (Fila) malloc(sizeof(struct Fila));
    fila->lista = ini_lista();
    return fila;
}

void setFila(Fila fila, int elem)
{
    fila->lista = add_fim_lista(fila->lista, elem);
}

int getFila(Fila fila)
{
    int ret;
    fila->lista = del_inicio_lista(fila->lista, &ret);
    return ret;
}

Fila cloneFila(Fila fila)
{
    int elem;
    Fila tmp = iniFila();
    Fila copia = iniFila();

    while(!isEmptyFila(fila))
        setFila(tmp, getFila(fila));

    while (!isEmptyFila(tmp))
    {
        elem = getFila(tmp);
        setFila(fila, elem);
        setFila(copia, elem);
    }

    return copia;
}

int lengthFila(Fila fila)
{
    return conta_lista(fila->lista);
}

char *getVetFila(Fila fila)
{
    Fila copia = cloneFila(fila);
    int tam = lengthFila(copia);
    char *vet = (char*) malloc((tam + 1) * sizeof(char));
    for (int i = 0; i < tam; i++)
        vet[i] = (char) getFila(copia);
    vet[tam+1] = '\0';
    return vet;
}

Fila freeFila(Fila fila) {
    fila->lista = freeLista(fila->lista);
    free(fila);
    return NULL;
}
