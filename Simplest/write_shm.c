#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    // Chave para identificação da memória compartilhada
    key_t key = ftok("shmfile", 65);

    // Criar o segmento de memória compartilhada
    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget failed");
        exit(1);
    }

    // Anexar o segmento ao espaço de endereçamento do processo
    char *data = (char*) shmat(shmid, NULL, 0);
    if (data == (char*)-1) {
        perror("shmat failed");
        exit(1);
    }

    // Escrever dados na memória compartilhada
    printf("Escreva algo na memória compartilhada: ");
    fgets(data, 1024, stdin);

    // Desanexar o segmento de memória
    if (shmdt(data) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    printf("Dados escritos na memória compartilhada.\n");

    return 0;
}
