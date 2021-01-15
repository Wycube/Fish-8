# Adding the imgui implementation source files to this target
add_executable(fish main.cpp Window.cpp Gui.cpp Application.cpp ${PROJECT_SOURCE_DIR}/lib/imgui-1.79/imgui_impl_opengl3.cpp ${PROJECT_SOURCE_DIR}/lib/imgui-1.79/imgui_impl_glfw.cpp )

target_link_libraries(fish chip8-emu glfw glad imgui tinyfiledialog fmt)