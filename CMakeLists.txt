cmake_minimum_required(VERSION 3.27)
project(prac-polynom-operator)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -std=c++20")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


set(CORE
        src/cartesian_tree.cpp
        src/tree_drawer.cpp
        src/vector_operations.cpp
        src/avl_tree.cpp
        core/trees/splay_tree.h
        src/rb_tree.cpp
)

add_executable(${PROJECT_NAME}
        main.cpp
        src/table_drawer.cpp
        src/window.cpp
        src/button.cpp
        src/center_positioned_string.cpp
        src/general_data.cpp
        src/window_manager.cpp
        app/colors.h
        src/colors.cpp
        src/inoutput_field.cpp
        src/table_drawer.cpp
        src/rounded_rectangle.cpp
        src/tabs_drawer.cpp
        ${CORE}
)

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics)


if(WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM)
endif()

install(TARGETS ${PROJECT_NAME})
