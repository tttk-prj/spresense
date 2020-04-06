#ifndef __WORKER_COMMON_H__
#define __WORKER_COMMON_H__

/* MP object keys. Must be synchronized with worker. */

#define KEY_SHM1   1
#define KEY_SHM2   2
#define KEY_SHM3   3
#define KEY_SHM4   4
#define KEY_MQ     5
#define KEY_MUTEX  6

#define MSG_ID_SAYHELLO 1

#define SHM_TILE_SIZE (128*1024)

/* Enable USE_SHARED_MEM if you want to use shared memory */
#define USE_SHARED_MEM

#endif  // __WORKER_COMMON_H__
