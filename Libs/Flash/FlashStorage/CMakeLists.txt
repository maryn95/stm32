add_library(FlashStorage FlashStorage.cpp)

target_include_directories(FlashStorage INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FlashStorage PUBLIC project_compiler_flags)
