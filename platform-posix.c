#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "vextm-log.h"
#include "platform.h"

shm_file_t shm_fd_open(char* name, size_t len) {
    shm_file_t fd = shm_open(name, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        warn("shm_open failed: %d", errno);
        return SHM_FD_INVALID;
    }

    return fd;
}

void shm_fd_close(shm_file_t fd, char* name) {
    shm_unlink(name);
}

uint8_t* shm_mmap(shm_file_t fd, size_t len) {
    // Set size of shared memory file
    ftruncate(fd, len);

    // Obtain a pointer to memory-mapped file data
    uint8_t* imgbuf = (uint8_t*) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(imgbuf == NULL) {
        warn("mmap failed: %d", errno);
        return NULL;
    }

    return imgbuf;
}

void shm_mmap_close(uint8_t* mmap, size_t len) {
    munmap(mmap, len);
}

shm_sem_t shm_sem_create(char* name) {
    shm_sem_t sem = sem_open(name, O_CREAT, S_IRUSR | S_IWUSR, 0);
    if(sem == SEM_FAILED) {
        warn("sem_open failed: %d", errno);
        return NULL;
    }

    return sem;
}

int shm_sem_wait(shm_sem_t sem) {
    int ret;
    for(int i = 0; i < 20; i++) {
        ret = sem_trywait(sem);
        if (ret == 0) {
            break;
        }

        // Semaphore not ready, sleep for 5ms
        usleep(5 * 1000);
    }
    if(ret == 0) {
        return 0;
    }
    return 1;
}

void shm_sem_close(shm_sem_t sem, char* name) {
    sem_unlink(name);
}

plat_pid_t plat_spawn(const char* file, char* const args[]) {
    pid_t pid;

    pid = fork();
    if(pid == 0) {
        // Child process
        execvp(file, args);
    }

    return pid;
}

void plat_kill(plat_pid_t pid) {
    kill(pid, SIGTERM);
}
