cmake_minimum_required(VERSION 3.16)

Project(SettingsStorageLib)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

# Include the subdirectories
add_subdirectory(Source)
add_subdirectory(Tests)

# Add coverage flags if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Adding coverage flags")
    set(CMAKE_CXX_FLAGS " ${CMAKE_CXX_FLAGS} --coverage")
endif()

#include(CodeCoverage)
#append_coverage_compiler_flags()
#
#setup_target_for_coverage_gcovr_xml(NAME coverage
#        EXECUTABLE SettingsStorage_GoogleTestsExe
#)