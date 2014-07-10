add_library(rsd buffer.cpp drawable.cpp glfwapplication.cpp program.cpp shader.cpp vertexarray.cpp
    vertexformat.cpp)
target_link_libraries(rsd glfw ${GLFW_LIBRARIES})
