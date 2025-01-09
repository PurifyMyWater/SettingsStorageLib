add_library(barrcrc STATIC)

file(GLOB barrcrc_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

target_sources(barrcrc PRIVATE "${barrcrc_SOURCES}")
target_include_directories(barrcrc PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
