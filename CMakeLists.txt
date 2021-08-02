cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(cpp_fortunes_project LANGUAGES CXX)

set(OpenGL_GL_PREFERENCE "GLVND")

find_package(CGAL REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -frounding-math")


include_directories(include)
include_directories(external)




add_executable(devmain app/devmain.cpp)

target_include_directories(devmain PUBLIC ${GLFW_INCLUDE_DIRS}
	${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})
target_link_libraries(devmain ${GLFW_LIBRARIES} ${GLEW_LIBRARIES}
	${OPENGL_LIBRARIES})


