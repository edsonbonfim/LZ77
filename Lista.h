#ifndef PAQ_LISTA_H
#define PAQ_LISTA_H

typedef struct lista *Lista;

int vazia_lista(Lista l);
int conta_lista(Lista lista);

Lista ini_lista(void);
Lista freeLista(Lista lista);
Lista add_fim_lista(Lista l, int elem);
Lista del_inicio_lista(Lista lista, int *ret);

#endif //PAQ_LISTA_H