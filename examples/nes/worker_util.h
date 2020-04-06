#ifndef __WORKER_UTIL_H__
#define __WORKER_UTIL_H__

#include "render_request.h"

extern "C" int worker_exec(void);
extern "C" int send_render_request(struct RenderRequestContainer *req);
extern "C" struct RenderRequestContainer *recv_render_request(void);
extern "C" void *get_shm_buffer(int idx);
extern "C" void set_shm_ptr(struct RenderRequestContainer *req);
extern "C" uintptr_t vbuf_v2p(struct RenderRequestContainer *req);

#endif  // __WORKER_UTIL_H__
