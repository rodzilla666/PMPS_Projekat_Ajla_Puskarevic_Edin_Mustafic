#include "waveplayer.h"
#include "fatfs.h"
#include "File_Handling.h"
#include "AUDIO.h"

static uint32_t uwVolume = 75;

#define OUTPUT_DEVICE_SPEAKER         1
#define OUTPUT_DEVICE_HEADPHONE       2
#define OUTPUT_DEVICE_BOTH            3
#define OUTPUT_DEVICE_AUTO            4


static AUDIO_OUT_BufferTypeDef  BufferCtl;

AUDIO_PLAYBACK_StateTypeDef AudioState;

static int16_t FilePos = 0;

FILELIST_FileTypeDef FileList;

WAVE_FormatTypeDef WaveFormat;

FIL WavFile;

uint8_t PlayerInit(uint32_t AudioFreq)
{
	if(AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint32_t GetCurrentVolume()
{
	return uwVolume;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx)
{
  uint bytesread;

  f_close(&WavFile);
  if(AUDIO_GetWavObjectNumber() > idx)
  {
    f_open(&WavFile, (char *)FileList.file[idx].name, FA_READ);
    f_read(&WavFile, &WaveFormat, sizeof(WaveFormat), &bytesread);
    
    PlayerInit(WaveFormat.SampleRate);
    
    BufferCtl.state = BUFFER_OFFSET_NONE;
    
    f_lseek(&WavFile, 0);
    
    if(f_read(&WavFile,&BufferCtl.buff[0],AUDIO_OUT_BUFFER_SIZE,(void *)&bytesread) == FR_OK)
    {
      AudioState = AUDIO_STATE_PLAY;
        if(bytesread != 0)
        {
          AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE);
          BufferCtl.fptr = bytesread;
          return AUDIO_ERROR_NONE;
        }
      }
  }
  return AUDIO_ERROR_IO;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(bool isLoop)
{
  uint32_t bytesread;
  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
  
  switch(AudioState)
  {
  case AUDIO_STATE_PLAY:
    if(BufferCtl.fptr >= WaveFormat.FileSize)
    {
      AUDIO_OUT_Stop(CODEC_PDWN_SW);
      AudioState = AUDIO_STATE_NEXT;
    }
    
    if(BufferCtl.state == BUFFER_OFFSET_HALF)
    {
      if(f_read(&WavFile, &BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE/2, (void *)&bytesread) != FR_OK)
      { 
        AUDIO_OUT_Stop(CODEC_PDWN_SW);
        return AUDIO_ERROR_IO;       
      } 
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread; 
    }
    
    if(BufferCtl.state == BUFFER_OFFSET_FULL)
    {
      if(f_read(&WavFile, &BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2], AUDIO_OUT_BUFFER_SIZE/2, (void *)&bytesread) != FR_OK)
      { 
        AUDIO_OUT_Stop(CODEC_PDWN_SW);
        return AUDIO_ERROR_IO;       
      } 
 
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread; 
    }
    break;
    
  case AUDIO_STATE_STOP:
    AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AudioState = AUDIO_STATE_IDLE; 
    audio_error = AUDIO_ERROR_IO;
    break;
    
  case AUDIO_STATE_NEXT:
    if(++FilePos >= AUDIO_GetWavObjectNumber())
    {
    	if (isLoop)
    	{
    		FilePos = 0;
    	}
    	else
    	{
    		AudioState =AUDIO_STATE_STOP;
    	}
    }
    AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;    
    
  case AUDIO_STATE_PREVIOUS:
    if(--FilePos < 0)
    {
      FilePos = AUDIO_GetWavObjectNumber() - 1;
    }
    AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;   
    
  case AUDIO_STATE_PAUSE:
    AUDIO_OUT_Pause();
    AudioState = AUDIO_STATE_WAIT;
    break;
    
  case AUDIO_STATE_RESUME:
    AUDIO_OUT_Resume();
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_UP: 
    if( uwVolume <= 90)
    {
      uwVolume += 10;
    }
    AUDIO_OUT_SetVolume(uwVolume);
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_DOWN:    
    if( uwVolume >= 10)
    {
      uwVolume -= 10;
    }
    AUDIO_OUT_SetVolume(uwVolume);
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_WAIT:
  case AUDIO_STATE_IDLE:
  case AUDIO_STATE_INIT:    
  default:
    break;
  }
  return audio_error;
}

AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void)
{
  AudioState = AUDIO_STATE_STOP;
  FilePos = 0;
  
  AUDIO_OUT_Stop(CODEC_PDWN_SW);
  f_close(&WavFile);
  return AUDIO_ERROR_NONE;
}

void AUDIO_OUT_TransferComplete_CallBack(void)
{
  if(AudioState == AUDIO_STATE_PLAY)
  {
    BufferCtl.state = BUFFER_OFFSET_FULL;
  }
}

void AUDIO_OUT_HalfTransfer_CallBack(void)
{ 
  if(AudioState == AUDIO_STATE_PLAY)
  {
    BufferCtl.state = BUFFER_OFFSET_HALF;
  }
}

