#include <stdio.h>
#include <stdlib.h>
#include "Lista.h"

struct lista
{
    int info;
    Lista prox;
};

Lista ini_lista(void)
{
    return NULL;
}

int vazia_lista(Lista l)
{
    return l == NULL;
}

int conta_lista(Lista lista)
{
    if (vazia_lista(lista))
        return 0;

    return 1 + conta_lista(lista->prox);
}

Lista add_fim_lista(Lista l, int elem)
{
    if (vazia_lista(l))
    {
        l = (Lista) malloc(sizeof(struct lista));
        l->info = elem;
        l->prox = ini_lista();
    }
    else
    {
        l->prox = add_fim_lista(l->prox, elem);
    }
    return l;
}

Lista del_inicio_lista(Lista lista, int *ret)
{
    if (vazia_lista(lista))
        return lista;

    *ret = lista->info;
    Lista tmp = lista->prox;
    free(lista);
    return tmp;
}

Lista freeLista(Lista lista)
{
    if (!vazia_lista(lista))
    {
        lista->prox = freeLista(lista->prox);
        free(lista);
    }
    return NULL;
}
