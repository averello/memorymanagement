cmake_minimum_required(VERSION 3.10.2)

project(memorymanagement VERSION 1.1.4)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(memorymanagement ${PROJECT_SOURCE_DIR}/src/memory_management.c)
target_compile_options(memorymanagement PRIVATE -W -Wall -Wextra -pedantic
    -std=c99 -I${INC} -D_XOPEN_SOURCE=700)
set_target_properties(memorymanagement PROPERTIES PUBLIC_HEADER
    ${PROJECT_SOURCE_DIR}/include/memory_management/memory_management.h)
target_include_directories(memorymanagement PUBLIC
	"${PROJECT_SOURCE_DIR}/include"
)
list(APPEND EXTRA_INCLUDES "${PROJECT_SOURCE_DIR}/include")

install(TARGETS memorymanagement
	LIBRARY DESTINATION lib ${CMAKE_INSTALL_LIBDIR}
	PUBLIC_HEADER DESTINATION include/memory_management ${CMAKE_INSTALL_INCLUDEDIR}
)
