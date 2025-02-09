add_library(libartcpp STATIC)

file(GLOB libart_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libart/src/*.c")

target_sources(libartcpp PRIVATE "${libart_SOURCES}")
target_compile_options(libartcpp PUBLIC -Wno-pedantic)
target_include_directories(libartcpp PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/libart/src")
