#ifndef __WAVEPLAYER_H
#define __WAVEPLAYER_H

#include "main.h"
#include "stdbool.h"

#define AUDIO_OUT_BUFFER_SIZE                      4096


typedef enum {
  AUDIO_ERROR_NONE = 0,
  AUDIO_ERROR_IO,
  AUDIO_ERROR_EOF,
  AUDIO_ERROR_INVALID_VALUE,
}AUDIO_ErrorTypeDef;


typedef enum {
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

typedef struct {
  uint8_t buff[AUDIO_OUT_BUFFER_SIZE];
  BUFFER_StateTypeDef state;
  uint32_t fptr;
}AUDIO_OUT_BufferTypeDef;

typedef enum {
  AUDIO_STATE_IDLE = 0,
  AUDIO_STATE_WAIT,
  AUDIO_STATE_INIT,
  AUDIO_STATE_PLAY,
  AUDIO_STATE_RECORD,
  AUDIO_STATE_NEXT,
  AUDIO_STATE_PREVIOUS,
  AUDIO_STATE_FORWARD,
  AUDIO_STATE_BACKWARD,
  AUDIO_STATE_STOP,
  AUDIO_STATE_PAUSE,
  AUDIO_STATE_RESUME,
  AUDIO_STATE_VOLUME_UP,
  AUDIO_STATE_VOLUME_DOWN,
  AUDIO_STATE_ERROR,
}AUDIO_PLAYBACK_StateTypeDef;

typedef struct {
  uint32_t ChunkID;       /* 0 */
  uint32_t FileSize;      /* 4 */
  uint32_t FileFormat;    /* 8 */
  uint32_t SubChunk1ID;   /* 12 */
  uint32_t SubChunk1Size; /* 16*/
  uint16_t AudioFormat;   /* 20 */
  uint16_t NbrChannels;   /* 22 */
  uint32_t SampleRate;    /* 24 */

  uint32_t ByteRate;      /* 28 */
  uint16_t BlockAlign;    /* 32 */
  uint16_t BitPerSample;  /* 34 */
  uint32_t SubChunk2ID;   /* 36 */
  uint32_t SubChunk2Size; /* 40 */
}WAVE_FormatTypeDef;




AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(bool isLoop);
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void);

#endif
