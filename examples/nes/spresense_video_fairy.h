#ifndef __SPRESENSE_VIDEO_FAIRY_H__
#define __SPRESENSE_VIDEO_FAIRY_H__

#include <pthread.h>

#include <nuttx/board.h>
#include <nuttx/lcd/lcd.h>
#include "../../emulator/fairy/VideoFairy.h"

#include "spresense_port.h"
#include "video_config.h"

#include "render_request.h"

class SpresenseVideoFairy;

inline void initialize_render_req(int idx, struct RenderRequestContainer *container)
{
  container->lineNo = idx;
  container->paletteMask = 0;
  container->next = NULL;
}

#define REQUEST_ENGINE_QUEUEDEPTH  (2)
class RenderEngine {
public:
  RenderEngine();
  ~RenderEngine() {};
  void startEngine();

  void setVideoFairy(SpresenseVideoFairy *fairy){ vFairy = fairy; };

  static void *render_thread(void *param);

  struct RenderRequestContainer * requestRender(int line_no, uint8_t *linebuff, uint8_t paletteMask);
  uint16_t *getRenderFrameBuff(void);
  uint8_t *getRenderVRAM(void);
  void initializeSHM(void);

  struct RenderRequestContainer * popFromEmpty();
  void pushToEmpty(struct RenderRequestContainer *request);

  struct RenderRequestContainer * popFromRequest();
  void pushToRequest(struct RenderRequestContainer *request);

private:
  struct RenderRequestContainer *req_queue;
  struct RenderRequestContainer *empty_queue;
  struct RenderRequestContainer requests[REQUEST_ENGINE_QUEUEDEPTH];

  struct RenderRequestContainer *current_work;

  pthread_t thd;
  pthread_cond_t empty_cond;
  pthread_mutex_t empty_mutex;

  pthread_cond_t req_cond;
  pthread_mutex_t req_mutex;

  SpresenseVideoFairy *vFairy;
};


class SpresenseVideoFairy : public VideoFairy {
public:
  SpresenseVideoFairy();
  ~SpresenseVideoFairy();
  uint8_t * initialize();
  uint8_t *dispatchRendering(const uint8_t (*nesBuffer)[screenWidth], const uint8_t paletteMask);

private:
  struct lcd_dev_s *lcddev;
  struct lcd_planeinfo_s pinfo;

  int counter;
  RenderEngine render_engine;

  uint32_t total_render_time;
  uint32_t *total_time;
  int line_col;

  friend class RenderEngine;
};


#endif  // __SPRESENSE_VIDEO_FAIRY_H__
