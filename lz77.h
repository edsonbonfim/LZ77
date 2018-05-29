#ifndef LZ77_LZ77_H
#define LZ77_LZ77_H

void LZComprimir(FILE *temp, char *nomeOriginal, char *nomeComprimido);
void preencherArquivo(FILE *origem, FILE *destino);
void LZDescomprimir(FILE *comp);

#endif // LZ77_LZ77_H