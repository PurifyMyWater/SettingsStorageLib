add_library(SettingsStorageLib STATIC)

add_subdirectory(deps/art)
add_subdirectory(deps/OSShim)
add_subdirectory(deps/CRCpp)

file(GLOB SettingsStorageLib_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(SettingsStorageLib PRIVATE ${SettingsStorageLib_SOURCES})
target_include_directories(SettingsStorageLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(SettingsStorageLib PRIVATE -Wall -Wextra -Wpedantic -Werror)

# Link the sub-libraries to the combined library
target_link_libraries(SettingsStorageLib PUBLIC libartcpp OSShim CRCpp)
