if (NOT ESP_PLATFORM) # Only configure tests if we are building in a computer.

project(SettingsStorage_GoogleTests)
if (NOT TARGET gtest)
    add_subdirectory(googletest)
endif ()
add_subdirectory(LinuxOSShim)

include_directories("${gtest_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/TestUtils")

file(GLOB_RECURSE TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Tests/*.cpp")
file(GLOB_RECURSE TEST_UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/TestUtils/*.cpp")

# adding the Google_Tests_run target
add_executable(SettingsStorage_GoogleTestsExe ${TEST_SOURCES} ${TEST_UTILS_SOURCES})
target_include_directories(SettingsStorage_GoogleTestsExe PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/TestUtils")

# linking Google_Tests_run with SettingsStorageLib which will be tested
target_link_libraries(SettingsStorage_GoogleTestsExe SettingsStorageLib LinuxOSShim)

target_link_libraries(SettingsStorage_GoogleTestsExe gtest gtest_main)

else ()
    message(STATUS "Skipping Google Test configuration for target '${PROJECT_NAME}' because we are building for an embedded target")
endif ()
