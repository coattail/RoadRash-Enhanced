/* Road Rash Mac gameplay additions live in an executable DLL code section.
 * The stock race clock and keyboard polling are never detoured. */
#include <windows.h>
#include <stdint.h>
#include <string.h>
#include "dd.h"
#include "roadrash_audio.h"
#define RD(a) (*(volatile uint32_t *)(uintptr_t)(a))
#define F(p,o) RD((p)+(o))
typedef BYTE (__attribute__((fastcall)) *QUERY)(uint32_t);
typedef BYTE (__attribute__((fastcall)) *ANIM)(uint32_t,uint32_t);
typedef uint32_t (*RNG)(void);
static struct {uint32_t owner, order;} attempts[16];
static uint32_t serial, remembered_player, last_tick;
static uint32_t player_hit_victim,player_hit_tick;
static BOOL ready, pending;
/* The race count excludes slot 15 in short fields. The officer is always
 * registered separately there by 406c5f/406c64, with owner 4642dc. */
static uint32_t rider_at(uint32_t i){
 uint32_t count=RD(0x4642d4);if(count>16)count=16;
 return i<count?RD(0x4c7f70+i*4):i==15?RD(0x4642dc):0;
}
/* Riding/attack poses 0..27 must never replace a fallen rider's pose.
 * Fall, slide, stand-up and remount poses start at 28. Clear-queue (255)
 * and non-rider animation objects retain the original behavior. */
__attribute__((used,noinline)) BOOL rr_rider_animation_allowed(uint32_t anim,uint32_t pose){
 if((pose&255)>=28 || RD(0x4c5d7c)!=1)return TRUE;
 for(uint32_t i=0;i<16;i++){
  uint32_t owner=rider_at(i);if(!owner)continue;
  uint32_t state=F(owner,0x2fc);
  if(state && state+0x70==anim){uint32_t mode=F(owner,0x2f4);return mode!=1 && mode!=2;}
 }
 return TRUE;
}
__attribute__((naked,used)) void rr_primary_animation(void){
 __asm__ volatile(
  "pushl %ecx\npushl %edx\npushl %edx\npushl %ecx\ncall _rr_rider_animation_allowed\naddl $8,%esp\npopl %edx\npopl %ecx\n"
  "testl %eax,%eax\njz 1f\npushl %ebx\npushl %esi\nmovb %dl,%bl\nmovl %ecx,%esi\nmovl $0x40ac76,%eax\njmp *%eax\n"
  "1: xorl %eax,%eax\nret\n");
}
__attribute__((naked,used)) void rr_secondary_animation(void){
 __asm__ volatile(
  "pushl %ecx\npushl %edx\npushl %edx\npushl %ecx\ncall _rr_rider_animation_allowed\naddl $8,%esp\npopl %edx\npopl %ecx\n"
  "testl %eax,%eax\njz 1f\npushl %ebx\npushl %esi\nmovb %dl,%bl\nmovl %ecx,%esi\nmovl $0x40aca6,%eax\njmp *%eax\n"
  "1: xorl %eax,%eax\nret\n");
}
static uint32_t mag(int32_t n){return n<0?-(uint32_t)n:(uint32_t)n;}
static void sync_race(void){
 uint32_t player=RD(0x4642d8),tick=RD(0x4b7ecc);
 if(player!=remembered_player || tick<last_tick || !tick){memset(attempts,0,sizeof(attempts));serial=0;pending=FALSE;remembered_player=player;player_hit_victim=player_hit_tick=0;}
 last_tick=tick;
}
static uint32_t incoming_order(uint32_t other){
 for(uint32_t i=0;i<16;i++)if(attempts[i].owner==other && rider_at(i)==other)return attempts[i].order;
 return 0;
}
static uint32_t weapon_reach(uint32_t style){return (style==1?7:style==2?8:5)<<12;}
/* Retaliation can reach the attacker at the distance of the longer weapon,
 * with one road unit of lateral tolerance and 25% longitudinal tolerance.
 * A rider with no accepted incoming attack keeps the stock contact bounds. */
static BOOL player_contact(uint32_t self,uint32_t other,uint32_t side,uint32_t style){
 if(!self || !other || self==other || F(self,0x2f4) || !F(self,0x2c) || F(other,0x2f4) || !F(other,0x2c) || (F(other,0xc0)&16))return FALSE;
 uint32_t os=F(other,0x2fc);if(!os)return FALSE;
 int32_t x=(int32_t)(F(os,0x28)-F(self,0x28));
 if((x>0)!=(side!=0))return FALSE;
 uint32_t reach=weapon_reach(style),z=64;
 if(incoming_order(other)){
  uint32_t theirs=weapon_reach(F(os,0x1d4));if(theirs>reach)reach=theirs;
  reach+=0x1000;z=80;
 }
 return mag(F(os,0x1c)-F(self,0x1c))<z && mag(x)<reach && mag(F(os,0x20)-F(self,0x20))<0x3000;
}
__attribute__((used,noinline)) void rr_remember_attempt(uint32_t source,uint32_t target){
 if(!target || target!=RD(0x4642d8) || source==target || !source)return;
 sync_race();
 for(uint32_t i=0;i<16;i++)if(rider_at(i)==source){attempts[i].owner=source;attempts[i].order=++serial;return;}
}
static uint32_t select_target(uint32_t self,BOOL lock_side){
 if(!self || self!=RD(0x4642d8) || F(self,0x2f4) || !F(self,0x2c))return 0;
 uint32_t state=F(self,0x2fc);if(!state || !F(state,0x1d8))return 0;
 sync_race();uint32_t style=F(state,0x1d4);
 /* During an existing swing, contact must stay on that swing's side. */
 BOOL swinging=lock_side && ((QUERY)0x409510)(self)!=0;
 BOOL right=swinging?((QUERY)0x4095a0)(self)!=0:FALSE;
 uint32_t best=0,newest=0,score=UINT32_MAX;
 for(uint32_t i=0;i<16;i++){
  uint32_t other=rider_at(i),order=attempts[i].owner==other?attempts[i].order:0;
  if(!other || other==self || F(other,0x2f4) || !F(other,0x2c) || (F(other,0xc0)&16))continue;
  /* Owners are 0x32c bytes; paired states are only 0x208 (405470/405490).
   * Fallen mode belongs to the owner. Reading state+2f4 reads another
   * object's memory and used to silently reject healthy riders. */
  uint32_t os=F(other,0x2fc);if(!os || os==state)continue;
  uint32_t dz=mag(F(os,0x1c)-F(self,0x1c)),dx=mag(F(os,0x28)-F(self,0x28)),dy=mag(F(os,0x20)-F(self,0x20));
  if(!player_contact(self,other,(int32_t)(F(os,0x28)-F(self,0x28))>0,style))continue;
  if(swinging && ((int32_t)(F(os,0x28)-F(self,0x28))>0)!=right)continue;
  uint32_t z=dz<<8,distance=z*z+dx*dx+dy*dy;
  if(!best || order>newest || (order==newest && distance<score)){best=other;newest=order;score=distance;}
 }
 return best;
}
__attribute__((used,noinline)) uint32_t rr_recent_attacker(uint32_t self){
 uint32_t target=select_target(self,TRUE);
 /* Once swinging, an empty correct-side search must not fall back to the
  * stock nearest-Z rider on the other side or beyond weapon reach. */
 if(!target && self && self==RD(0x4642d8) && !F(self,0x2f4) && F(self,0x2fc) && F(F(self,0x2fc),0x1d8) && ((QUERY)0x409510)(self))return UINT32_MAX;
 return target;
}
typedef BYTE (__attribute__((fastcall)) *CONTACT)(uint32_t,uint32_t,uint32_t,uint32_t);
typedef void (__attribute__((fastcall)) *HIT)(uint32_t);
typedef BYTE (__attribute__((fastcall)) *DAMAGE)(uint32_t,int32_t,int32_t,uint32_t);
/* This call site is used only by rider melee damage (409ad0), so crash,
 * traffic and road-object damage never enters the defense adjustment.
 * Fourfold defense rounds fractional positive amounts up: small hits
 * keep at least one damage, without overflow at INT32_MAX. */
__attribute__((used,noinline,fastcall)) BYTE rr_combat_damage(uint32_t target,int32_t amount,int32_t extra,uint32_t kind){
 if(target && target==RD(0x4642d8) && RD(0x4c5d7c)==1 && (kind==4 || kind==5)){
  if(amount>0)amount=(amount>>2)+((amount&3)!=0);
  if(extra>0)extra=(extra>>2)+((extra&3)!=0);
 }
 return ((DAMAGE)0x4052c0)(target,amount,extra,kind);
}
/* Only the rider call site is wrapped; road-object contact remains stock.
 * At simultaneous contact, run the player's real hit first. Suppress the
 * opponent's hit only if that call actually advanced its damage cooldown.
 * Wind-up, a miss, blocking, and cooldown confer no passive immunity. */
__attribute__((used,noinline,fastcall)) BYTE rr_rider_contact(uint32_t self,uint32_t other_state,uint32_t side,uint32_t style){
 uint32_t player=RD(0x4642d8);
 BYTE stock=((CONTACT)0x409c10)(self,other_state,side,style);
 if(RD(0x4c5d7c)==1 && !*(BYTE*)0x4753c8 && player && other_state){
  sync_race();
  uint32_t other=0;for(uint32_t i=0;i<16;i++){uint32_t p=rider_at(i);if(p && F(p,0x2fc)==other_state){other=p;break;}}
  if(other && self==player)return player_contact(self,other,side,style);
  if(stock && other==player && self!=player && !F(player,0x2f4) && !F(self,0x2f4)){
   /* The player may have been processed earlier in this very frame. */
   if(player_hit_victim==self && player_hit_tick==RD(0x4b7ecc))return FALSE;
   uint32_t ps=F(player,0x2fc),os=F(self,0x2fc);
   if(ps && os && F(ps,0x1d8) && ((QUERY)0x409510)(player) && select_target(player,TRUE)==self){
    uint32_t frame=F(ps,0x7c),flags=frame?F(frame,0x10):0;
    BOOL contact_frame=(*(BYTE*)(uintptr_t)(ps+0x81)==2) || (flags && (F(flags,4)&0x600000));
    if(contact_frame && F(os,0x1d0)<RD(0x4b7ecc) && *(BYTE*)(uintptr_t)(os+0x80)!=9){
     uint32_t before=F(os,0x1d0);((HIT)0x4096f0)(player);
     if(F(os,0x1d0)!=before)return FALSE;
    }
   }
  }
 }
 return stock;
}
__attribute__((used,noinline)) uint32_t rr_repeat_pose(uint32_t anim,uint32_t pose){
 uint32_t self=RD(0x4642d8);if(!self || F(self,0x2f4) || RD(0x4c5d7c)!=1)return pose;
 uint32_t state=F(self,0x2fc);if(!state || anim!=state+0x70 || (pose&255)==255)return pose;
 uint32_t style=F(state,0x1d4);if(style>3)return pose;
 uint32_t left=RD(0x4644f8+style*4),right=RD(0x464518+style*4);
 uint32_t alt_left=RD(0x464508+style*4),alt_right=RD(0x464528+style*4);
 if(pose!=left && pose!=right && pose!=alt_left && pose!=alt_right)return pose;
 uint32_t target=select_target(self,FALSE);if(!target)return pose;
 BOOL next_right=(int32_t)(F(F(target,0x2fc),0x28)-F(self,0x28))>0;
 /* Queue the next swing only; never flip the current contact animation. */
 return pose==alt_left || pose==alt_right?(next_right?alt_right:alt_left):(next_right?right:left);
}
__attribute__((naked,used)) void rr_repeat_attack(void){
 __asm__ volatile("pushl %ecx\npushl %edx\npushl %ecx\ncall _rr_repeat_pose\naddl $8,%esp\npopl %ecx\nmovl %eax,%edx\nmovl $0x43ae60,%eax\njmp *%eax\n");
}
__attribute__((used,noinline)) void rr_npc_celebrate(uint32_t anim){
 if(!rr_rider_animation_allowed(anim,6))return;
 uint32_t roll=((RNG)0x441750)();((ANIM)0x40aca0)(anim,(roll&1)?2:6);
}
__attribute__((used,noinline)) void rr_remember_player_hit(uint32_t source,uint32_t target){
 if(source!=RD(0x4642d8) || !target || target==source)return;
 sync_race();player_hit_victim=target;player_hit_tick=RD(0x4b7ecc);
}
__attribute__((naked,used)) void rr_attack_start(void){
 __asm__ volatile(
  "movl $0x40ac70,%eax\ncall *%eax\ntestb %al,%al\njz 1f\n"
  "pushl %eax\npushl %ecx\npushl %edx\npushl %edi\npushl %esi\n"
  "call _rr_remember_attempt\naddl $8,%esp\npopl %edx\npopl %ecx\npopl %eax\n1: ret\n");
}
__attribute__((naked,used)) void rr_target_refresh(void){
 __asm__ volatile(
  "pushl %ecx\npushl %edx\npushl %ecx\ncall _rr_recent_attacker\naddl $4,%esp\npopl %edx\npopl %ecx\n"
  "testl %eax,%eax\njz 1f\ncmpl $-1,%eax\njne 2f\n"
  /* No hittable rider on this swing's side. Retain the stock secondary
   * object query so road-object contact callbacks still receive EAX. */
  "pushl %esi\nmovl 0x2fc(%ecx),%esi\nmovl %esi,%ecx\nmovl $0x38,%edx\npushl $0x1e0\nmovl $0x449c80,%eax\ncall *%eax\n"
  "movl 0x1d8(%esi),%ecx\npopl %esi\nmovl $0,0x168(%ecx)\nret\n"
  "2: movl 0x2fc(%ecx),%ecx\nmovl 0x1d8(%ecx),%ecx\nmovl %eax,0x168(%ecx)\nxorl %eax,%eax\nret\n"
  "1: pushl %esi\nmovl $8,%edx\nmovl $0x409406,%eax\njmp *%eax\n");
}
__attribute__((naked,used)) void rr_hit_gesture(void){
 __asm__ volatile(
  "pushl %eax\npushl %ecx\npushl %edx\npushl %esi\npushl %edi\ncall _rr_remember_player_hit\naddl $8,%esp\npopl %edx\npopl %ecx\npopl %eax\n"
  "movl 0x4642d8,%eax\ncmpl %eax,%edi\nje 1f\ncmpl %eax,%esi\njne 1f\n"
  "pushl 0x10(%esp)\ncall _rr_npc_celebrate\naddl $4,%esp\nmovl $0x40997c,%edx\njmp *%edx\n"
  "1: movl $0x441750,%eax\ncall *%eax\nmovl $0x40993d,%edx\njmp *%edx\n");
}
static BOOL patch(uintptr_t address, const BYTE *original, size_t size, void *target, BYTE opcode){
 if(memcmp((void*)address,original,size))return FALSE;
 DWORD protection; if(!VirtualProtect((void*)address,size,PAGE_EXECUTE_READWRITE,&protection))return FALSE;
 BYTE b[8];memset(b,0x90,size);b[0]=opcode;int32_t delta=(intptr_t)target-address-5;memcpy(b+1,&delta,4);memcpy((void*)address,b,size);
 FlushInstructionCache(GetCurrentProcess(),(void*)address,size);DWORD ignored;VirtualProtect((void*)address,size,protection,&ignored);return TRUE;
}
static BOOL install(void){
 if((uintptr_t)GetModuleHandleA(NULL)!=0x400000)return FALSE;
 const BYTE pause[]={0xe8,0x59,0x30,0xff,0xff},leave[]={0xe8,0x4e,0xc1,0xfd,0xff};
 const BYTE animation[]={0x53,0x56,0x8a,0xda,0x8b,0xf1};
 const BYTE refresh[]={0x56,0xba,8,0,0,0},attack[]={0xe8,0x8f,0x15,0,0},hit[]={0xe8,0x13,0x7e,3,0};
 const BYTE repeat[]={0xe8,0xc0,0x66,0,0},repeat_ai[]={0xe8,0xf8,0x2f,3,0};
 const BYTE contact[]={0xe8,0x58,0x04,0,0};
 const BYTE damage[]={0xe8,0x07,0xb7,0xff,0xff};
 if(memcmp((void*)0x443202,pause,5)||memcmp((void*)0x44331d,leave,5))return FALSE;
 if(memcmp((void*)0x40ac70,animation,6)||memcmp((void*)0x40aca0,animation,6))return FALSE;
 if(memcmp((void*)0x409400,refresh,6)||memcmp((void*)0x4096dc,attack,5)||memcmp((void*)0x409938,hit,5))return FALSE;
 if(memcmp((void*)0x43479b,repeat,5)||memcmp((void*)0x407e63,repeat_ai,5))return FALSE;
 if(memcmp((void*)0x4097b3,contact,5))return FALSE;
 if(memcmp((void*)0x409bb4,damage,5))return FALSE;
 return patch(0x409400,refresh,6,rr_target_refresh,0xe9) && patch(0x4096dc,attack,5,rr_attack_start,0xe8) && patch(0x409938,hit,5,rr_hit_gesture,0xe9) && patch(0x443202,pause,5,rr_pause_enter,0xe8) && patch(0x44331d,leave,5,rr_pause_leave,0xe8) && patch(0x40ac70,animation,6,rr_primary_animation,0xe9) && patch(0x40aca0,animation,6,rr_secondary_animation,0xe9) && patch(0x43479b,repeat,5,rr_repeat_attack,0xe8) && patch(0x407e63,repeat_ai,5,rr_repeat_attack,0xe8) && patch(0x4097b3,contact,5,rr_rider_contact,0xe8) && patch(0x409bb4,damage,5,rr_combat_damage,0xe8);
}
void rr_mac_key(unsigned int key,long flags){
 if(ready && key=='U' && !(flags&(1L<<30)) && RD(0x4c5d7c)==1 && !*(BYTE*)0x4753c8)pending=TRUE;
}
int rr_mac_dashboard(unsigned int key){
 /* The old M alias exists only inside the stock pause message loop.
  * Handle key release in the real race window. Consuming it prevents the
  * original F10 handler from cycling a second time. */
 if(!ready || (key!='M' && key!=VK_F10) || RD(0x4c5d7c)!=1)return FALSE;
 if(!*(BYTE*)0x4753b8){
  BYTE *mode=(BYTE*)0x467a98;*mode=(*mode+1)%3;
 }
 return TRUE;
}
void rr_mac_tick(void){
 static BOOL checked;
 if(!checked){checked=TRUE;ready=install();}
 if(!ready)return;
 sync_race();
 static BOOL u_held;
 if(GetForegroundWindow()==g_ddraw.hwnd){
  SHORT keys=GetAsyncKeyState('U');BOOL down=(keys&0x8000)!=0;
  if(!u_held && (down || (keys&1)))rr_mac_key('U',0);
  u_held=down;
 }else{u_held=FALSE;pending=FALSE;}
 if(!pending)return;
 uint32_t player=RD(0x4642d8);
 if(RD(0x4c5d7c)!=1 || *(BYTE*)0x4753c8 || !player || F(player,0x2f4) || !F(player,0x2c) || (F(player,0xc0)&16)){pending=FALSE;return;}
 uint32_t state=F(player,0x2fc);if(!state || (F(state,0xc0)&16)){pending=FALSE;return;}
 if((RD(0x4b7ed0)&0xe0) || ((QUERY)0x409510)(player))return;
 pending=FALSE;((ANIM)0x40aca0)(state+0x70,2);((ANIM)0x40ac70)(state+0x70,6);
}
