set(TARGET_NAME gl)
setup_hifi_library(OpenGL Qml Quick)
link_hifi_libraries(shared)

target_glew()
target_opengl()
target_oglplus()