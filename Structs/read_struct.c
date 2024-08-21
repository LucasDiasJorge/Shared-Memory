#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Define the struct
typedef struct {
    char name[50];
    int age;
} Person;

#define SHM_SIZE sizeof(Person)  // Size of the shared memory segment
#define SHM_FILE_PATH "shmfile"  // Path used to generate the key

// Function to generate a unique key based on a path and a project ID
key_t get_shared_memory_key(const char *path, int proj_id) {
    key_t key = ftok(path, proj_id);
    if (key == -1) {
        perror("Error generating key with ftok");
        exit(EXIT_FAILURE);
    }
    return key;
}

// Function to create or obtain a shared memory segment
int create_or_get_shared_memory(key_t key, size_t size, int permissions) {
    int shmid = shmget(key, size, permissions);
    if (shmid == -1) {
        perror("Error creating/obtaining shared memory segment with shmget");
        exit(EXIT_FAILURE);
    }
    return shmid;
}

// Function to attach the shared memory segment to the process's address space
void* attach_shared_memory(int shmid) {
    void *shmaddr = shmat(shmid, NULL, 0);
    if (shmaddr == (void *) -1) {
        perror("Error attaching shared memory with shmat");
        exit(EXIT_FAILURE);
    }
    return shmaddr;
}

// Function to detach the shared memory segment from the process's address space
void detach_shared_memory(const void *shmaddr) {
    if (shmdt(shmaddr) == -1) {
        perror("Error detaching shared memory with shmdt");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Generate key
    key_t key = get_shared_memory_key(SHM_FILE_PATH, 65);

    // Create or get shared memory segment
    int shmid = create_or_get_shared_memory(key, SHM_SIZE, 0666);

    // Attach shared memory
    Person *shared_data = (Person *) attach_shared_memory(shmid);

    // Read data from shared memory
    printf("Name: %s\n", shared_data->name);
    printf("Age: %d\n", shared_data->age);

    // Detach shared memory
    detach_shared_memory(shared_data);

    return 0;
}
