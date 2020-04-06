#ifndef __RENDER_REQUEST_H__
#define __RENDER_REQUEST_H__

#ifndef ARGB2RGB565
#define ARGB2RGB565(x) ( ((x)&(0xF80000)>>8) | ((x)&(0x00FC00)>>5) | ((x)&(0x0000F8)>>3) )
#endif

#ifndef RGB565
#define RGB565(r,g,b) ( (((uint16_t)(r)&(0xF8))<<8) | (((uint16_t)(g)&(0xFC))<<3) | (((uint16_t)(b)&(0xF8))>>3) )
#endif

struct RenderRequestContainer {
  int lineNo;
  uint8_t *  lineBuf;
  uint8_t    paletteMask;
  uint16_t * vbuf;
  uint8_t *  lineBuf_sub;
  uint16_t * vbuf_sub;

  struct RenderRequestContainer *next;
};

inline void dump_container(struct RenderRequestContainer *req)
{
  printf("[%d] lineBuf:%08x, mask:%02x, vbuf:%08x, lineBuf_sub:%08x, vbuf_sub:%08x\n",
            req->lineNo,
            req->lineBuf,
            req->paletteMask,
            req->vbuf,
            req->lineBuf_sub,
            req->vbuf_sub );

}

#endif  // __RENDER_REQUEST_H__

