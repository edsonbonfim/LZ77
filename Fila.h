#ifndef PAQ_FILA_H
#define PAQ_FILA_H

typedef struct Fila *Fila;

void setFila(Fila fila, int elem);

int getFila(Fila fila);
int lengthFila(Fila fila);
int isEmptyFila(Fila fila);

char *getVetFila(Fila fila);

Fila iniFila(void);
Fila freeFila(Fila fila);
Fila cloneFila(Fila fila);

#endif //PAQ_FILA_H