/* Local Road Rash PC combat admission policy. Fixed addresses are verified
 * against the user's unpacked executable; no saves or physics changes. */
#include <stdint.h>
#define RD(a) (*(volatile uint32_t *)(uintptr_t)(a))
#define FIELD(p,o) RD((p)+(o))
static __inline__ uint32_t magnitude(int32_t n){return n<0?-(uint32_t)n:(uint32_t)n;}
__attribute__((fastcall)) uint8_t combat_admitted(uint32_t self){
    uint32_t player=RD(0x4642d8);
    if(RD(0x4b7ecc)<300 || !player || !self || self==player) return 0;
    if(FIELD(player,0x2f4) || !FIELD(player,0x2c) || FIELD(self,0x2f4) || !FIELD(self,0x2c))return 0;
    /* Keep v3's departure protection even when waiting at the grid >5sec. */
    if((int32_t)FIELD(player,0x1c)<=0x3220 && (int32_t)FIELD(player,0xec)<=1500)return 0;
    uint32_t pz=FIELD(player,0x1c),px=FIELD(player,0x28);
    uint32_t dz=magnitude(FIELD(self,0x1c)-pz),dx=magnitude(FIELD(self,0x28)-px);
    if(dz>3072 || dx>128000)return 0;
    uint32_t side=(int32_t)(FIELD(self,0x28)-px)>=0;
    uint32_t score=(dz<<8)+dx;
    uint32_t count=RD(0x4642d4);
    if(count>16)count=16;
    for(uint32_t i=0;i<count;i++){
        uint32_t other=RD(0x4c7f70+4*i);
        if(!other || other==self || other==player)continue;
        if(FIELD(other,0x2f4) || !FIELD(other,0x2c) || !FIELD(other,0x2fc))continue;
        uint32_t oz=magnitude(FIELD(other,0x1c)-pz),ox=magnitude(FIELD(other,0x28)-px);
        if(oz>3072 || ox>128000)continue;
        if(((int32_t)(FIELD(other,0x28)-px)>=0)!=side)continue;
        uint32_t other_score=(oz<<8)+ox;
        /* Address tie break is deterministic for the whole race and avoids
         * every rider selecting the same slot when positions coincide. */
        if(other_score<score || (other_score==score && other<self))return 0;
    }
    return 1;
}
