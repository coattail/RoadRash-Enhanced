/* Pause every live game sound, including buffers omitted by the original
 * active-voice flag and the MIDI synth's already sounding notes. */
#define COBJMACROS
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdint.h>
#include <stdio.h>
#include "roadrash_audio.h"
#define RD(a) (*(volatile uint32_t *)(uintptr_t)(a))
static struct {IDirectSoundBuffer *buffer; uintptr_t slot; DWORD flags;} stopped[72];
static unsigned count;
static BOOL paused;

static void stop_slot(uintptr_t slot){
 IDirectSoundBuffer *buffer=(void*)(uintptr_t)RD(slot);
 if(!buffer || count==72)return;
 for(unsigned i=0;i<count;i++)if(stopped[i].buffer==buffer)return;
 DWORD status=0;
 if(FAILED(IDirectSoundBuffer_GetStatus(buffer,&status)) || !(status&DSBSTATUS_PLAYING))return;
 IDirectSoundBuffer_AddRef(buffer);
 if(FAILED(IDirectSoundBuffer_Stop(buffer))){IDirectSoundBuffer_Release(buffer);return;}
 stopped[count].buffer=buffer;stopped[count].slot=slot;
 stopped[count++].flags=(status&DSBSTATUS_LOOPING)?DSBPLAY_LOOPING:0;
}
__attribute__((used,noinline)) void rr_audio_pause(void){
 if(paused)return;
 paused=TRUE;count=0;
 /* 69 original sound objects. Some looping voices have active==0, so use
  * the actual DirectSound playback status rather than that game flag. */
 for(uintptr_t slot=0x4c4060;slot<0x4c4174;slot+=4){
  uint32_t voice=RD(slot);if(voice)stop_slot(voice+0x24);
 }
 stop_slot(0x47ea38); /* streamed PCM music/video */
 stop_slot(0x4c8060);stop_slot(0x4c80a4); /* original primary buffers */
 uint32_t song=RD(0x46d904),stream=song?RD(song):0;
 HMIDIOUT midi=stream && RD(stream)==0x4953444d?(void*)(uintptr_t)RD(stream+0x14):NULL;
 unsigned quiet=0;
 /* midiStreamPause only stops future events. Explicitly end all currently
  * sounding notes. Wine's CoreAudio volume API reports success even when
  * the underlying AudioUnit volume operation has no effect. Do not alter
  * the user's volume or reset/discard the queued stream buffers. */
 if(midi)for(unsigned channel=0;channel<16;channel++)
  if(midiOutShortMsg(midi,0x78b0|channel)==MMSYSERR_NOERROR)quiet++;
 FILE *log=fopen("pause-audio.log","a");
 if(log){fprintf(log,"pause: stopped=%u midi_channels_quiet=%u\n",count,quiet);fclose(log);}
}
__attribute__((used,noinline)) void rr_audio_finish(unsigned result){
 if(!paused)return;
 unsigned resumed=0,failed=0;
 for(unsigned i=0;i<count;i++){
  IDirectSoundBuffer *buffer=stopped[i].buffer;
  /* Continue (2) resumes only buffers still owned by the game. Quit (1)
   * releases retained references without replaying the previous race. */
  if(result==2 && RD(stopped[i].slot)==(uintptr_t)buffer){
   DWORD status=0;
   if(SUCCEEDED(IDirectSoundBuffer_GetStatus(buffer,&status)) && !(status&DSBSTATUS_PLAYING)){
    if(SUCCEEDED(IDirectSoundBuffer_Play(buffer,0,0,stopped[i].flags)))resumed++;else failed++;
   }
  }
  IDirectSoundBuffer_Release(buffer);
 }
 count=0;paused=FALSE;
 FILE *log=fopen("pause-audio.log","a");
 if(log){fprintf(log,"leave: result=%u resumed=%u failed=%u\n",result,resumed,failed);fclose(log);}
}
__attribute__((naked,used)) void rr_pause_enter(void){
 __asm__ volatile("pushl %eax\npushl %ecx\npushl %edx\ncall _rr_audio_pause\npopl %edx\npopl %ecx\npopl %eax\nmovl $0x436260,%eax\njmp *%eax\n");
}
__attribute__((naked,used)) void rr_pause_leave(void){
 __asm__ volatile("pushl %eax\npushl %ecx\npushl %edx\npushl %esi\ncall _rr_audio_finish\naddl $4,%esp\npopl %edx\npopl %ecx\npopl %eax\nmovl $0x41f470,%eax\njmp *%eax\n");
}
