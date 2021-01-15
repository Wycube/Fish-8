project(glad C)

include_directories(include)
add_library(glad src/glad.c include/glad/glad.h include/KHR/khrplatform.h)