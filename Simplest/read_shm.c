#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    // Chave para identificação da memória compartilhada
    key_t key = ftok("shmfile", 65);

    // Localizar o segmento de memória compartilhada
    int shmid = shmget(key, 1024, 0666);
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

    // Ler dados da memória compartilhada
    printf("Dados na memória compartilhada: %s\n", data);

    // Desanexar o segmento de memória
    if (shmdt(data) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    // Remover o segmento de memória compartilhada
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }

    printf("Segmento de memória compartilhada removido.\n");

    return 0;
}
