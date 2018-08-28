cmake_minimum_required(VERSION 2.8)
project(MySfM C)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS_DEBUG "-g3")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_SOURCE_DIR}/../bin")

set(files
  ${CMAKE_SOURCE_DIR}/main.c
  ${CMAKE_SOURCE_DIR}/detect_directory.c
  ${CMAKE_SOURCE_DIR}/output_image_list.c
  ${CMAKE_SOURCE_DIR}/SIFT/SIFT_main.c
  ${CMAKE_SOURCE_DIR}/SIFT/DoG.c
  ${CMAKE_SOURCE_DIR}/SIFT/grayscale.c
  ${CMAKE_SOURCE_DIR}/SIFT/allocate_image.c
  ${CMAKE_SOURCE_DIR}/SIFT/load_jpeg.c
  ${CMAKE_SOURCE_DIR}/SIFT/write_jpeg.c
)

add_executable(MySfM ${files})
target_link_libraries(MySfM m jpeg)
