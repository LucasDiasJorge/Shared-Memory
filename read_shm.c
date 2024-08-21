#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Definições de constantes
#define SHM_SIZE 1024       // Tamanho do segmento de memória compartilhada
#define SHM_PERMISSIONS 0666 // Permissões para o segmento de memória
#define SHM_FILE_PATH "shmfile" // Caminho usado para gerar a chave

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

// Função para remover o segmento de memória compartilhada
void remove_shared_memory(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erro ao remover memória compartilhada com shmctl");
        exit(EXIT_FAILURE);
    }
}

int main() {

    // Gerar chave única para memória compartilhada
    key_t key = ftok(SHM_FILE_PATH, 65);

    // Criar ou obter segmento de memória compartilhada
    int shmid = create_or_get_shared_memory(key, SHM_SIZE, SHM_PERMISSIONS);

    // Anexar segmento de memória compartilhada
    char *shared_data = attach_shared_memory(shmid);

    // Verificar se há dados na memória compartilhada
    if (strlen(shared_data) == 0) {
        printf("Nenhum dado encontrado na memória compartilhada.\n");
    } else {
        printf("Dados na memória compartilhada: %s\n", shared_data);
    }

    // Desanexar segmento de memória compartilhada
    detach_shared_memory(shared_data);

    // Remover segmento de memória compartilhada
    remove_shared_memory(shmid);

    printf("Segmento de memória compartilhada removido com sucesso.\n");

    return 0;
}
