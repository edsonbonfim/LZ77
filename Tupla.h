#ifndef PAQ_TUPLAS_H
#define PAQ_TUPLAS_H

typedef struct tupla *Tupla;

void saveTupla(FILE *arq, Tupla t);
void setTupla(Tupla t, int pos, int tam, int car);
void getTupla(Tupla t, int *pos, int *tam, int *car);
void readTupla(FILE *arq, Tupla tupla);

Tupla iniTupla(void);
Tupla freeTupla(Tupla tupla);

#endif //PAQ_TUPLAS_H
