#include <stdio.h>
#include <stdint.h>
#include <obs-module.h>
#include <util/platform.h>
#include <pthread.h>
#include "platform.h"
#include "colorbars.h"
#include "vextm-log.h"
#include "vextm-source.h"
#include "vextm-thread.h"

#define FRAME_WIDTH 1920
#define FRAME_HEIGHT 1080
#define IMG_BUF_SIZE (FRAME_WIDTH * FRAME_HEIGHT * 4)

// Function vextm_source_thread is the management function executed in a
// background thread to communicate with the external TM Display process. It
// opens the shared memory segment that the TM Display process will use as a
// framebuffer and also opens a corresponding semaphore that will be triggered
// whenever a new display frame is written to shared memory.
void* vextm_source_thread(void* arg) {
	struct vextm_source_data* context = (struct vextm_source_data*) arg;

	info("Thread start");

    struct obs_source_frame obs_frame = {0};
    obs_frame.width = FRAME_WIDTH;
    obs_frame.height = FRAME_HEIGHT;
    obs_frame.format = VIDEO_FORMAT_BGRA;
    obs_frame.data[0] = (uint8_t*) get_color_bar_data();
    obs_frame.linesize[0] = (FRAME_WIDTH * 4);

    // Open memory-mapped file
    char fb_name[16];
    snprintf(fb_name, 16, "/%s-fb", context->shmem);
    shm_file_t fd = shm_fd_open(fb_name, IMG_BUF_SIZE);
    if(fd == SHM_FD_INVALID) {
        pthread_exit(NULL);
    }

    // Obtain a pointer to memory-mapped file data
    uint8_t* imgbuf = (uint8_t*) shm_mmap(fd, IMG_BUF_SIZE);
    if(imgbuf == NULL) {
        shm_fd_close(fd, fb_name);
        pthread_exit(NULL);
    }

    // Open semaphore that will be signalled after a new frame is written to
    // shared memory
    char sem_name[16];
    snprintf(sem_name, 16, "/%s-sem", context->shmem);
    shm_sem_t frame_semaphore = shm_sem_create(sem_name);
    if(frame_semaphore == NULL) {
        shm_mmap_close(imgbuf, IMG_BUF_SIZE);
        shm_fd_close(fd, fb_name);
        pthread_exit(NULL);
    }

    obs_frame.data[0] = imgbuf;

    int idle_count = 0;
    int frame_count = 0;
    while(true) {
        // Wait for semaphore to be singalled indicating a new frame is
        // available in shared memory.  Set maximum wait time so the OBS frame
        // is updated at least that often, even if we're not signalled from
        // the shared memory source.
        int wait = shm_sem_wait(frame_semaphore);
        if(wait == 0) {
            frame_count++;
            idle_count = 0;
        } else {
            idle_count++;
        }

        // If we haven't received a frame in a long time, go to color bars to
        // indicate loss of signal
        if(idle_count >= 450) {
            generate_color_bars(obs_frame.data[0], obs_frame.width, obs_frame.height);
            idle_count = 0;
        }

        // If semaphore was signalled or we've been idle for a while, deliver
        // the frame to OBS.  The obs_frame structure already points to the
        // shared memory so no need to fill it in.
        if((wait == 0) || ((idle_count % 8) == 0)) {
            obs_frame.timestamp = 0;
            obs_source_output_video(context->source, &obs_frame);
        }

        // If the semaphore wasn't signalled or if several frames have been
        // drawn, then check the run_thread variable to determine if this
        // thread should terminate.  Avoid locking/unlocking every loop during
        // high frame rates.
        if((wait != 0) || (frame_count >= 10)) {
            // Check for a signal to end the thread and if so break out of the
            // while loop
            int keep_running = 0;
            pthread_mutex_lock(&(context->mutex));
            keep_running = context->run_thread;
            pthread_mutex_unlock(&(context->mutex));
            if(keep_running == 0) {
                break;
            }

            frame_count = 0;
        }
    }

	info("Thread ending");

    shm_sem_close(frame_semaphore, sem_name);
    shm_mmap_close(imgbuf, IMG_BUF_SIZE);
    shm_fd_close(fd, fb_name);

    pthread_exit(NULL);

    return NULL;
}
