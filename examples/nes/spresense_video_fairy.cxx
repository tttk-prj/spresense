#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>

#include <nuttx/board.h>
#include <nuttx/lcd/ili9340.h>

#include "spresense_port.h"
#include "spresense_video_fairy.h"

#include "worker_util.h"


extern "C" int ili9340_putall(int devno, fb_coord_t y0,     fb_coord_t x0,
                                         fb_coord_t height, fb_coord_t width,
                                         FAR const uint8_t * buffer);

#define MEASURE_60_FRAMES


/**********************************************/
/** RenderEngine Class method implementation **/
/**********************************************/
/** Constructor on RenderEngine **/
RenderEngine::RenderEngine() : req_queue(NULL), empty_queue(NULL) 
{

  initialize_render_req(0, &requests[0]);
  current_work = &requests[0];

  initialize_render_req(1, &requests[1]);
  empty_queue = &requests[1];

  for(int i=2; i<REQUEST_ENGINE_QUEUEDEPTH; i++)
    {
      initialize_render_req(i, &requests[i]);
      requests[i-1].next = &requests[i];
    }

  pthread_mutex_init(&empty_mutex, NULL);
  pthread_cond_init(&empty_cond, NULL);
  pthread_mutex_init(&req_mutex, NULL);
  pthread_cond_init(&req_cond, NULL);
}

extern "C" void ili9340_setcolor(int devno, const uint8_t *color_set);

void RenderEngine::initializeSHM(void)
{
  for(int i=0; i<REQUEST_ENGINE_QUEUEDEPTH; i++)
    {
      set_shm_ptr(&requests[i]);
    }
  for(int i=0; i<REQUEST_ENGINE_QUEUEDEPTH; i++)
  {
    dump_container(&requests[i]);
  }

  // Clear the display.
  ili9340_putall(0, 0, 0, 240, 320, (uint8_t *)vbuf_v2p(&requests[0]));

  { // Set color table on LCD.
    uint8_t i;
    uint8_t *data, *p;
    data = p = (uint8_t *)requests[0].vbuf;
    // Red color table
    for(i=0; i<32; i++)
      {
        *p++ = i<<1;
      }
    // Green color table
    for(i=0; i<64; i++)
      {
        *p++ = i;
      }
    // Blue color table
    for(i=0; i<32; i++)
      {
        *p++ = i<<1;
      }
    ili9340_setcolor(0, data);
  }
}

/** startEngine() on RenderEngine **/
void RenderEngine::startEngine()
{
  pthread_attr_t attr;
  struct sched_param sparam;

  pthread_attr_init(&attr);
  sparam.sched_priority = 110;
  pthread_attr_setschedparam(&attr,&sparam);
  pthread_create(&thd, &attr, RenderEngine::render_thread, (void *)this);
}

/** popFrameEmpty() on RenderEngine **/
struct RenderRequestContainer * RenderEngine::popFromEmpty()
{
  struct RenderRequestContainer *ret;

  pthread_mutex_lock(&empty_mutex);
    while(empty_queue == NULL)
      {
        pthread_cond_wait(&empty_cond, &empty_mutex);
      }
    ret = empty_queue;
    empty_queue = ret->next;
    ret->next = NULL;
  pthread_mutex_unlock(&empty_mutex);

  return ret;
}

/** popFrameRequest() on RenderEngine **/
struct RenderRequestContainer * RenderEngine::popFromRequest()
{
  struct RenderRequestContainer *ret;

  pthread_mutex_lock(&req_mutex);
    while(req_queue == NULL)
      {
        pthread_cond_wait(&req_cond, &req_mutex);
      }
    ret = req_queue;
    req_queue = ret->next;
    ret->next = NULL;
  pthread_mutex_unlock(&req_mutex);

  return ret;
}

/** pushToRequest() on RenderEngine **/
void RenderEngine::pushToRequest(struct RenderRequestContainer *request)
{
  pthread_mutex_lock(&req_mutex);
  if (req_queue == NULL)
    {
      req_queue = request;
    }
  else
    {
      struct RenderRequestContainer *tmp = req_queue;
      while(tmp->next != NULL)
        {
          tmp = tmp->next;
        }
      tmp->next = request;
    }
  pthread_cond_signal(&req_cond);
  pthread_mutex_unlock(&req_mutex);
}

/** pushToRequest() on RenderEngine **/
void RenderEngine::pushToEmpty(struct RenderRequestContainer *container)
{
  pthread_mutex_lock(&empty_mutex);
  if (empty_queue == NULL)
    {
      empty_queue = container;
    }
  else
    {
      struct RenderRequestContainer *tmp = empty_queue;
      while(tmp->next != NULL)
        {
          tmp = tmp->next;
        }
      tmp->next = container;
    }
  pthread_cond_signal(&empty_cond);
  pthread_mutex_unlock(&empty_mutex);
}

/** requestRender() on RenderEngine **/
struct RenderRequestContainer *RenderEngine::requestRender(int line_no, uint8_t *linebuff, uint8_t paletteMask)
{
  int no = current_work->lineNo;
  current_work->paletteMask = paletteMask;

  pushToRequest(current_work);
  current_work = popFromEmpty();

  if (no == current_work->lineNo) printf("This is not Good...\n");

  return current_work;
}

uint16_t * RenderEngine::getRenderFrameBuff(void)
{
  return current_work->vbuf;
}

uint8_t * RenderEngine::getRenderVRAM(void)
{
  return current_work->lineBuf;
}

/** render_thread() on RenderEngine **/
void *RenderEngine::render_thread(void *param)
{
  RenderEngine *engine = (RenderEngine *)param;
  RenderRequestContainer *request;

  while(1)
    {
      request = engine->popFromRequest();

      send_render_request(request);
      recv_render_request();

      ili9340_putall(0, 0, engine->vFairy->line_col, 240, 256, (uint8_t *)vbuf_v2p(request));

      engine->pushToEmpty(request);
    }
}


/**********************************************/
/** RenderEngine Class method implementation **/
/**********************************************/
/** Constructor on SpresenseVideoFairy **/
SpresenseVideoFairy::SpresenseVideoFairy() :
  counter(0), total_render_time(0)
{
  line_col = (320 - screenWidth) / 2;
}

#ifdef MEASURE_60_FRAMES
static struct timeval lastTime;
#endif

/** initialize() on SpresenseVideoFairy **/
uint8_t * SpresenseVideoFairy::initialize()
{
  counter = 0;

  board_lcd_initialize();
  lcddev = board_lcd_getdev(0);
  lcddev->getplaneinfo(lcddev, 0, &pinfo);
  lcddev->setpower(lcddev, 1);
  // ili9340_clear(lcddev, RGB565_BLACK);  // This clear implementation is very slow...

#ifdef MEASURE_60_FRAMES
  gettimeofday(&lastTime, NULL);
#endif

  render_engine.setVideoFairy(this);
  render_engine.initializeSHM();
  render_engine.startEngine();
  return render_engine.getRenderVRAM();
}

/********************************************/
/**   Destructor of SpresenseVideoFairy    **/
/********************************************/
SpresenseVideoFairy::~SpresenseVideoFairy()
{
}

/********************************************/
/**          dispatchRendering             **/
/***** Rendering dispatch on V-Blank. *******/
uint8_t * SpresenseVideoFairy::dispatchRendering(
        const uint8_t (*nesBuffer)[screenWidth],
        const uint8_t paletteMask)
{
  render_engine.requestRender(0/*dummy*/, NULL/*dummy*/, (uint8_t)paletteMask);

#ifdef MEASURE_60_FRAMES
  counter++;
  if (counter >= 60)
    {
      struct timeval curTime;
      uint32_t deltaUS;
      gettimeofday(&curTime, NULL);

      deltaUS = (curTime.tv_sec  - lastTime.tv_sec ) * 1000000L + 
                (curTime.tv_usec - lastTime.tv_usec);

      printf("60 frames.. %dus AVG:%dus/f  rtime:%dus/f\n",
                    deltaUS,
                    deltaUS / counter,
                    total_render_time/counter);

      counter = 0;
      total_render_time = 0;
      lastTime = curTime;
    }
#endif  //MEASURE_60_FRAMES

  return render_engine.getRenderVRAM();
}


