cmake_minimum_required(VERSION 3.11)

project(snes9x-psp)

include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2)
pkg_search_module(SDL2_MIXER REQUIRED SDL2_mixer)

include_directories(
  src/
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} 
  src/main.cpp
  src/fxinst.cpp
  src/controls.cpp
  src/stream.cpp
  src/fxemu.cpp
  src/netplay.cpp
  src/movie.cpp
  src/conffile.cpp
  src/sa1.cpp
  src/tileimpl-h2x1.cpp
  src/bsx.cpp
  src/c4.cpp
  src/dsp.cpp
  src/cpuops.cpp
  src/gfx.cpp
  src/seta010.cpp
  src/fscompat.cpp
  src/sdd1.cpp
  src/cheats2.cpp
  src/srtc.cpp
  src/seta011.cpp
  src/main.cpp
  src/jma/lzma.cpp
  src/jma/crc32.cpp
  src/jma/7zlzma.cpp
  src/jma/lzmadec.cpp
  src/jma/winout.cpp
  src/jma/jma.cpp
  src/jma/iiostrm.cpp
  src/jma/inbyte.cpp
  src/jma/s9x-jma.cpp
  src/msu1.cpp
  src/memmap.cpp
  src/snapshot.cpp
  src/statemanager.cpp
  src/dsp2.cpp
  src/server.cpp
  src/fxdbg.cpp
  src/ppu.cpp
  src/crosshairs.cpp
  src/cheats.cpp
  src/dsp3.cpp
  src/common/audio/s9x_sound_driver_sdl.cpp
  src/common/video/std_chrono_throttle.cpp
  src/dma.cpp
  src/cpu.cpp
  src/obc1.cpp
  src/cpuexec.cpp
  src/tileimpl-n1x1.cpp
  src/sha256.cpp
  src/c4emu.cpp
  src/sdd1emu.cpp
  src/loadzip.cpp
  src/spc7110dec.cpp
  src/bml.cpp
  src/dsp1.cpp
  src/sa1cpu.cpp
  src/seta.cpp
  src/globals.cpp
  src/debug.cpp
  src/clip.cpp
  src/tile.cpp
  src/external/imgui/imgui_draw.cpp
  src/external/imgui/imgui_tables.cpp
  src/external/imgui/imgui_widgets.cpp
  src/external/imgui/imgui.cpp
  src/external/imgui/snes9x_imgui.cpp
  src/external/imgui/imgui_demo.cpp
  src/external/stb/stb_image_implementation.cpp
  src/seta018.cpp
  src/spc7110.cpp
  src/apu/bapu/smp/iplrom.cpp
  src/apu/bapu/smp/smp_state.cpp
  src/apu/bapu/smp/smp.cpp
  src/apu/bapu/dsp/sdsp.cpp
  src/apu/apu.cpp
  src/tileimpl-n2x1.cpp
  src/dsp4.cpp
  src/screenshot.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    pspgu
    pspge
    pspdisplay
    pspdebug
    ${SDL2_LIBRARIES}
    ${SDL2_MIXER_LIBRARIES}
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH NULL
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)
