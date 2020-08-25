cmake_minimum_required(VERSION 3.0.0)
project(srted VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(
  "argparse/"
  "src"
)
link_directories(
  "argparse/"
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

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
