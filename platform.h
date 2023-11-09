#pragma once

#include <stdint.h>

#ifdef _WIN32
#include "platform-windows.h"
#else
#include "platform-posix.h"
#endif

shm_file_t shm_fd_open(char* name, size_t len);
void shm_fd_close(shm_file_t fd, char* name);
uint8_t* shm_mmap(shm_file_t fd, size_t len);
void shm_mmap_close(uint8_t* mmap, size_t len);
shm_sem_t shm_sem_create(char* name);
int shm_sem_wait(shm_sem_t sem);
void shm_sem_close(shm_sem_t sem, char* name);
plat_pid_t plat_spawn(const char* file, char* const args[]);
void plat_kill(plat_pid_t pid);
