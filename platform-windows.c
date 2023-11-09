#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <windows.h>
#include "vextm-log.h"
#include "platform.h"

// Static helper methods defined at the end of the file
static char* get_dirname(char* path);

shm_file_t shm_fd_open(char* name, size_t len) {
    // Open memory-mapped file.  A file mapping with INVALID_HANDLE_VALUE uses
    // the system paging file without writing to disk.
    HANDLE fd = CreateFileMapping(
            INVALID_HANDLE_VALUE,
            NULL,
            PAGE_READWRITE,
            0,
            len,
            name);
    if (fd == NULL) {
        warn("CreateFileMapping failed: %ld", GetLastError());
        return NULL;
    }

    return fd;
}

void shm_fd_close(shm_file_t fd, char* name) {
    CloseHandle(fd);
}

uint8_t* shm_mmap(shm_file_t fd, size_t len) {
    // Obtain a pointer to memory-mapped file data
    uint8_t* imgbuf = (uint8_t*) MapViewOfFile(fd,
            FILE_MAP_ALL_ACCESS,
            0,
            0,
            len);
    if(imgbuf == NULL) {
        warn("MapViewOfFile failed: %ld", GetLastError());
        return NULL;
    }

    return imgbuf;
}

void shm_mmap_close(uint8_t* mmap, size_t len) {
    UnmapViewOfFile(mmap);
}

shm_sem_t shm_sem_create(char* name) {
    HANDLE sem = CreateSemaphoreA(
            NULL,
            0,
            5,
            name);
    if(sem == NULL) {
        warn("CreateSemaphore failed: %ld", GetLastError());
        return NULL;
    }

    return sem;
}

int shm_sem_wait(shm_sem_t sem) {
    DWORD wait = WaitForSingleObject(sem, 100);
    if(wait == WAIT_OBJECT_0) {
        return 0;
    }
    return 1;
}

void shm_sem_close(shm_sem_t sem, char* name) {
    CloseHandle(sem);
}

plat_pid_t plat_spawn(const char* file, char* const args[]) {
    char cmd[1024];

    snprintf(cmd, 1024, "%s", file);
    for(int i = 0; args[i] != NULL; i++) {
        snprintf(cmd, 1024, "%s %s", cmd, args[i]);
    }

    info("Windows display command: %s", cmd);

    char* dir;
    char path[1024];
    strncpy(path, cmd, 1024);
    dir = get_dirname(path);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    if(CreateProcessA(NULL,
                cmd,
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                dir,
                &si,
                &pi) == 0) {
        warn("CreateProcess failed: %ld", GetLastError());
    }
    return pi.hProcess;
}

void plat_kill(plat_pid_t pid) {
    TerminateProcess(pid, 0);
}

// Function get_dirname strips the last component off the path in order to
// provide the directory name for the specified resource. This is a basic
// implementation that fails to handle a number of edge cases.
static char* get_dirname(char* path) {
    for(int i = strlen(path) - 1; i >= 0; i--) {
        if((path[i] == '\\') || (path[i] == '/')) {
            path[i] = '\0';
            return path;
        }
    }

    snprintf(path, strlen(path), ".");
    return path;
}
