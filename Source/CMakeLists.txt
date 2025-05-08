add_library(SettingsStorageLib STATIC)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
        CRCpp
        GIT_REPOSITORY  https://github.com/d-bahr/CRCpp.git
        GIT_TAG         master
)
FetchContent_Declare(
        OSInterface
        GIT_REPOSITORY  git@github.com:vacmg/OSInterface.git
        GIT_TAG         v1.0.0
)
FetchContent_MakeAvailable(CRCpp)
FetchContent_MakeAvailable(OSInterface)

add_subdirectory(deps/art)

file(GLOB SettingsStorageLib_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
target_sources(SettingsStorageLib PRIVATE ${SettingsStorageLib_SOURCES})
target_include_directories(SettingsStorageLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_options(SettingsStorageLib PRIVATE -Wall -Wextra -Wpedantic -Werror)

# Link the sub-libraries to the combined library
target_link_libraries(SettingsStorageLib PUBLIC libartcpp OSInterface CRCpp)
