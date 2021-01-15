file(GLOB emu_src ${PROJECT_SOURCE_DIR}/src/emulator/*.cpp)

add_library(chip8-emu ${emu_src})