if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory.")
endif()

cmake_minimum_required(VERSION 3.10)

project(fish8 CXX)

cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add GLFW
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glfw-3.3.2)
include_directories(${PROJECT_SOURCE_DIR}/lib/glfw-3.3.2/include)

# Add glad
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/glad)
include_directories(${PROJECT_SOURCE_DIR}/lib/glad/include)

# Add miniaudio
include_directories(${PROJECT_SOURCE_DIR}/lib/miniaudio)

# Add ImGUI and imgui_club headers
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/imgui-1.79)
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui-1.79/)
include_directories(${PROJECT_SOURCE_DIR}/lib/imgui_club)

# Add glad
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/tinyfiledialog)
include_directories(${PROJECT_SOURCE_DIR}/lib/tinyfiledialog)

# Add fmt
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/fmt-7.1.3)
include_directories(${PROJECT_SOURCE_DIR}/lib/fmt-7.1.3/include)

# Add general include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add emulator as library
add_subdirectory(src/emulator)

add_subdirectory(src/frontend)