cmake_minimum_required(VERSION 3.0.0)
project(srted VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(
  "argparse/"
  "utf8/src"
  "Nuklear"
  "cconf/include"
  "src"
)
link_directories(
  "argparse/"
  "utf8/build/"
  "cconf/build"
)

add_executable(
  srted
  src/srted.c
  src/srt.c
  src/sub_list.c
  src/sub.c
  src/time.c
  argparse/argparse.c
)

target_link_libraries(srted utf8)

add_executable(
  srted-gui
  src/srted-gui.c
  src/srt.c
  src/sub_list.c
  src/sub.c
  src/time.c
  argparse/argparse.c
  src/gui/editor_ctx.c
)

target_link_libraries(srted-gui utf8 m GLEW GL glfw cconf)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
