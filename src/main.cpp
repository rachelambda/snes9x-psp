#include <stdlib.h>
#include <string.h>

#include <pspuser.h>
#include <pspgu.h>
#include <pspdebug.h>
#include <pspdisplay.h>

#include "snes9x.h"
#include "memmap.h"
#include "controls.h"
#include "gfx.h"
#include "apu/apu.h"

PSP_MODULE_INFO("snes9x-psp", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT BUFFER_HEIGHT
#define END_OFFSET 0x4000000

#define MAX_BUF 256
#define MAX_X 256
#define MAX_Y 239

char list[0x20000] __attribute__((aligned(64)));
int running;

void* framebuffer = (void*)0x88000;
void* drawbuffer = (void*)0x0;
void* depthbuffer = (void*)0x110000;

void initGu(){
    sceGuInit();

    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_5650,drawbuffer,BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,framebuffer,BUFFER_WIDTH);

    sceGuDepthBuffer(framebuffer, 0); // Set depth buffer to a length of 0
    sceGuDisable(GU_DEPTH_TEST); // Disable depth testing

    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuFinish();
    sceGuDisplay(GU_TRUE);
}

void endGu(){
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
}

void startFrame(){
    sceGuStart(GU_DIRECT, list);
}

void endFrame(){
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_DONE);
    sceDisplayWaitVblankStart();
    drawbuffer = sceGuSwapBuffers();
}

int exit_callback(int arg1, int arg2, void *common) {
    running = 0;
    endGu();
    Memory.Deinit();
    S9xGraphicsDeinit();
    S9xDeinitAPU();
    return 0;
}

void S9xExit(void) {
  exit_callback(0, 0, NULL);
}

bool8 S9xOpenSoundDevice() {
  // TODO
  return FALSE;
}

bool8 S9xOpenSnapshotFile (const char* path, bool8 readonly, STREAM* stream) {
  return FALSE;
}

std::string S9xGetFilenameInc (std::string, enum s9x_getdirtype) {
  return "TODO";
}

std::string S9xGetDirectory (enum s9x_getdirtype type) {
  return "TODO";
}

bool8 S9xInitUpdate() {
  return TRUE;
}

bool8 S9xContinueUpdate(int resx, int resy) {
  return TRUE;
}

bool8 S9xDeinitUpdate(int resx, int resy) {
  sceGuCopyImage(GU_PSM_5650, 
      0, 0, resx, resy, MAX_BUF, GFX.Screen, 
      (SCREEN_WIDTH - resx) / 2, (SCREEN_HEIGHT - resy) / 2, BUFFER_WIDTH, (void*)((pint)drawbuffer+END_OFFSET));
  return TRUE;
}

void S9xMessage(int type, int number, const char* message) {
  pspDebugScreenPrintf("S9xMessage: %s\n", message);
}

const char* S9xStringInput(const char *msg) {
  return NULL;
}

bool8 S9xOpenSnapshotFile(const char *filepath, bool8 read_only, STREAM file) {
  // TODO
  return FALSE;
}

void S9xCloseSnapshotFile(STREAM file) {
  // TODO
}

void S9xAutoSaveSRAM(void) {
  // TODO
}

void S9xSyncSpeed(void) {
  // TODO
}

void S9xToggleSoundChannel(int c) {
  // TODO
}

bool S9xPollButton (uint32 id, bool* pressed) {
  // TODO
  return false;
}

bool S9xPollPointer (uint32 id, int16 *x, int16 *y) {
  // TODO
  return false;
}

bool S9xPollAxis (uint32 id, int16 *value) {
  // TODO
  return false;
}

void S9xHandlePortCommand (s9xcommand_t cmd, int16 data1, int16 data2) {
  // TODO
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}


int main() {
    // Make exiting with the home button possible
    setup_callbacks();

    // Setup the library used for rendering
    initGu();
    
    pspDebugScreenInitEx(framebuffer, GU_PSM_5650, 0);

    Memory.Init();

    memset(&Settings, 0, sizeof(Settings));
    Settings.MouseMaster = true;
    Settings.SuperScopeMaster = true;
    Settings.JustifierMaster = true;
    Settings.MultiPlayer5Master = true;
    Settings.FrameTimePAL = 20000;
    Settings.FrameTimeNTSC = 16667;
    Settings.SoundPlaybackRate = 32040;
    Settings.SoundInputRate = 31947;
    Settings.Transparency = true;
    Settings.AutoDisplayMessages = true;
    Settings.InitialInfoStringTimeout = 120;
    Settings.HDMATimingHack = 100;
    Settings.BlockInvalidVRAMAccessMaster = true;


    // max screen size used by snes9x
    const uint screenSize = 2*MAX_BUF*MAX_Y;
    GFX.Screen = (uint16*)malloc(screenSize);

    memset(GFX.Screen, 0xFF, screenSize);

    S9xGraphicsInit();
    S9xInitAPU();
    S9xInitSound(0);

    running = 1;
    while(running){
        startFrame();

        // TODO: update controls

        // main loop
        S9xMainLoop();

        endFrame();
    }

    return 0;
}
