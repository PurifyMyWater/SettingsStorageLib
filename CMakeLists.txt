idf_component_register(SRCS "SettingsStorage.cpp" "SettingsParser.cpp"
                    INCLUDE_DIRS "include"
	            REQUIRES spiffs json fmt)

add_subdirectory(deps/art)
target_link_libraries(${COMPONENT_LIB} PUBLIC libartcpp)
