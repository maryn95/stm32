add_library(crc crc.c)

target_include_directories(crc INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(crc PUBLIC project_compiler_flags)
