#include <switch.h>

#include "routes.hpp"

// libnx fake heap initialization
extern "C" {
#define HEAP_SIZE 0x000340000

// we aren't an applet
u32 __nx_applet_type = AppletType_None;

// setup a fake heap (we don't need the heap anyway)
char fake_heap[HEAP_SIZE];

// we override libnx internals to do a minimal init
void __libnx_initheap(void) {
  extern char *fake_heap_start;
  extern char *fake_heap_end;

  // setup newlib fake heap
  fake_heap_start = fake_heap;
  fake_heap_end = fake_heap + HEAP_SIZE;
}

void __appInit(void) {
	/* Initialize services */
  smInitialize();
  ldrDmntInitialize();
	pmdmntInitialize();
  fsInitialize();
  fsdevMountSdmc();
  timeInitialize();
  socketInitializeDefault();
}

void __appExit(void) {
  fsdevUnmountAll();
  fsExit();
  timeExit();
  socketExit();
  pmdmntExit();
  ldrDmntExit();
  smExit();
}
}

int main() {
  startServer();
  return 0;
}
