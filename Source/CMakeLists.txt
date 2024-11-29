add_library(SettingsStorageLib STATIC)

add_subdirectory(deps/art)
add_subdirectory(deps/OSShim)

file(GLOB SettingsStorageLib_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(SettingsStorageLib PRIVATE ${SettingsStorageLib_SOURCES} "deps/cJSON/cJSON.c" "deps/cJSON/cJSON_Utils.c")
target_include_directories(SettingsStorageLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" deps/cJSON)

# Link the sub-libraries to the combined library
target_link_libraries(SettingsStorageLib PUBLIC libartcpp OSShim)
