add_library(SettingsStorageLib STATIC)

add_subdirectory(deps/art)

file(GLOB SettingsStorageLib_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(SettingsStorageLib PRIVATE ${SettingsStorageLib_SOURCES})
target_include_directories(SettingsStorageLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Link the sub-libraries to the combined library
target_link_libraries(SettingsStorageLib PUBLIC libartcpp)