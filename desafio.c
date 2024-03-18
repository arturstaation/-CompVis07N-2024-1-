#include <stdio.h>
#include <stdlib.h>

#define CHUNK_SIZE 4096

void printSignature(unsigned char *signature) {
    printf("Assinatura PNG: ");
    for (int i = 0; i < 8; i++) {
        printf("%02X ", signature[i]);
    }
    printf("\n");
}

void printChunk(unsigned char *type, unsigned int size) {
    printf("Chunk: %c%c%c%c, Tamanho: %u\n", type[0], type[1], type[2], type[3], size);
}

int main() {
    char filename[256];
    printf("Digite o nome do arquivo PNG: ");
    scanf("%255s", filename);

    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    unsigned char signature[8];
    fread(signature, sizeof(unsigned char), 8, file);
    printSignature(signature);

    while (1) {
        unsigned int size;
        unsigned char type[4];

        if (fread(&size, sizeof(unsigned int), 1, file) != 1) {
            if (feof(file)) {
                printf("Fim do arquivo atingido.\n");
                break;
            } else {
                perror("Erro ao ler o tamanho do chunk");
                fclose(file);
                return 1;
            }
        }
        size = __builtin_bswap32(size);

        if (fread(type, sizeof(unsigned char), 4, file) != 4) {
            perror("Erro ao ler o tipo do chunk");
            fclose(file);
            return 1;
        }

        printChunk(type, size);

        unsigned char *data = malloc(size * sizeof(unsigned char));
        if (data == NULL) {
            perror("Erro ao alocar memória para dados do chunk");
            fclose(file);
            return 1;
        }
        if (fread(data, sizeof(unsigned char), size, file) != size) {
            perror("Erro ao ler os dados do chunk");
            fclose(file);
            free(data);
            return 1;
        }

        printf("Dados do Chunk: ");
        for (int i = 0; i < size; i++) {
            printf("%02X ", data[i]);
        }
        printf("\n");

        free(data);

        unsigned int crc;
        if (fread(&crc, sizeof(unsigned int), 1, file) != 1) {
            perror("Erro ao ler o CRC do chunk");
            fclose(file);
            return 1;
        }
        crc = __builtin_bswap32(crc);
        printf("CRC: %08X\n\n", crc);
    }

    fclose(file);

    return 0;
}
