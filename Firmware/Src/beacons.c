#include "beacons.h"
#include "iprintf.h"
#include "board_id.h"

#include "ir_encode.h"
#include "ir_decode.h"

#include <stdint.h>
#include <string.h>

struct beacon_State {
   // systime timestamp from the last time we got a packet
   uint32_t    lastReceived;
};

static struct beacon_State state;

void beacon_Init(void) {
   memset(&state, 0, sizeof(state));

   iprintf("Setting up RC5 encode/decode...");
   ir_InitEncode();
   ir_InitDecode();
   iprintf("ok\r\n");
}

// sends 14 bites of data
void beacon_Send(uint16_t rawData) {
   ir_SendRaw(rawData);

   // We actually WANT to see our own beacon. If we turn off
   // RX, we don't get the incoming ones!
}

void beacon_SendId(void) {
   beacon_Send(bid_GetIDCrc());
}

bool beacon_Receive(uint16_t *rawBeacon) {
   RC5_Frame_TypeDef rcf;

   if(ir_GetDecoded(rawBeacon, &rcf)) {
      //FIXME rm
      iprintf("Raw  0x%x\r\n", *rawBeacon);
      iprintf("Addr   %d\r\n", rcf.Address);
      iprintf("Comd   %d\r\n", rcf.Command);
      iprintf("Field  %d\r\n", rcf.FieldBit);
      iprintf("Toggle %d\r\n", rcf.ToggleBit);
      iprintf("\r\n");

      state.lastReceived = HAL_GetTick();

      return true;
   }
   return false;
}

uint32_t beacon_LastReceived(void) {
   return state.lastReceived;
}
