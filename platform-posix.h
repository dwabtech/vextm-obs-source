#pragma once

#include <semaphore.h>

#define SHM_FD_INVALID -1

typedef int shm_file_t;
typedef sem_t* shm_sem_t;
typedef pid_t plat_pid_t;
