cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

include(Sanitizers.cmake)

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

add_executable(main app/main.cpp lib/voronoi_viewable.cpp)

target_include_directories(main PUBLIC ${GLFW_INCLUDE_DIRS}
 	${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${CGAL_INCLUDE_DIRS})
target_link_libraries(main ${GLFW_LIBRARIES} ${GLEW_LIBRARIES}
 	${OPENGL_LIBRARIES} ${CGAL_LIBRARY} ${GMP_LIBRARIES})

add_executable(generate_input app/rand_input.cpp)

install(FILES sample_input.txt DESTINATION bin)
install(FILES demo DESTINATION bin)
install(TARGETS main DESTINATION bin)
install(TARGETS generate_input DESTINATION bin)


