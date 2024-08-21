#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Definições de constantes
#define SHM_SIZE 1024       // Tamanho do segmento de memória compartilhada
#define SHM_PERMISSIONS 0666 // Permissões para o segmento de memória
#define SHM_FILE_PATH "shmfile" // Caminho usado para gerar a chave

// Função para gerar uma chave única baseada em um caminho e um ID de projeto
key_t get_shared_memory_key(const char *path, int proj_id) {
    key_t key = ftok(path, proj_id);
    if (key == -1) {
        perror("Erro ao gerar chave com ftok");
        exit(EXIT_FAILURE);
    }
    return key;
}

// Função para criar ou obter um segmento de memória compartilhada
int create_or_get_shared_memory(key_t key, size_t size, int permissions) {
    int shmid = shmget(key, size, permissions | IPC_CREAT);
    if (shmid == -1) {
        perror("Erro ao criar/obter segmento de memória compartilhada com shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Função para anexar o segmento de memória compartilhada ao espaço de endereçamento do processo
char* attach_shared_memory(int shmid) {
    char *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (char *) -1) {
        perror("Erro ao anexar memória compartilhada com shmat");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

// Função para desanexar o segmento de memória compartilhada do espaço de endereçamento do processo
void detach_shared_memory(const char *shmaddr) {
    if (shmdt(shmaddr) == -1) {
        perror("Erro ao desanexar memória compartilhada com shmdt");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Gerar chave única para memória compartilhada
    key_t key = get_shared_memory_key(SHM_FILE_PATH, 65);

    // Criar ou obter segmento de memória compartilhada
    int shmid = create_or_get_shared_memory(key, SHM_SIZE, SHM_PERMISSIONS);

    // Anexar segmento de memória compartilhada
    char *shared_data = attach_shared_memory(shmid);

    // Escrever dados na memória compartilhada
    printf("Escreva algo na memória compartilhada: ");
    if (fgets(shared_data, SHM_SIZE, stdin) == NULL) {
        perror("Erro ao ler entrada com fgets");
        detach_shared_memory(shared_data);
        exit(EXIT_FAILURE);
    }

    // Desanexar o segmento de memória compartilhada
    detach_shared_memory(shared_data);

    printf("Dados escritos na memória compartilhada.\n");

    return 0;
}
