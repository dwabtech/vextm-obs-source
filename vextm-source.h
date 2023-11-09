#pragma once

#include <obs-module.h>
#include <pthread.h>
#include "platform.h"

struct vextm_source_data {
	obs_source_t* source;

    pthread_t thread;
    pthread_mutex_t mutex;
    int run_thread;
    char shmem[10];
    plat_pid_t pid;
};
