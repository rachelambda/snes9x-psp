#include <stdlib.h>
#include <string.h>
#include <pspuser.h>
#include <pspgu.h>
#include <pspdisplay.h>

#include "snes9x.h"

PSP_MODULE_INFO("snes9x-psp", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_VFPU | THREAD_ATTR_USER);

#define BUFFER_WIDTH 512
#define BUFFER_HEIGHT 272
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT BUFFER_HEIGHT
#define END_OFFSET 0x4000000

char list[0x20000] __attribute__((aligned(64)));
int running;

int exit_callback(int arg1, int arg2, void *common) {
    running = 0;
    return 0;
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
    sceGuClearColor(0xFFFFFFFF);
    // sceGuClear(GU_COLOR_BUFFER_BIT);
}

void endFrame(){
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_DONE);
    sceDisplayWaitVblankStart();
    drawbuffer = sceGuSwapBuffers();
}

int main() {
    // Make exiting with the home button possible
    setup_callbacks();

    // Setup the library used for rendering
    initGu();

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

    int resx = 256;
    int bufw = 256;
    int resy = 239;

    // max screen size used by snes9x
    const uint screenSize = 2*256*239;
    void* screen = malloc(screenSize);

    memset(screen, 0xFF, screenSize);

    running = 1;
    while(running){
        startFrame();

        sceGuCopyImage(GU_PSM_5650, 
            0, 0, resx, resy, bufw, screen, 
            (SCREEN_WIDTH - resx) / 2, (SCREEN_HEIGHT - resy) / 2, BUFFER_WIDTH, drawbuffer+END_OFFSET);

        endFrame();
    }

    endGu();

    return 0;
}
