cmake_minimum_required(VERSION 3.22)

set(PROJECT_NAME first_opengl_project)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

# 프로젝트 환경변수 설정
set(WINDOW_NAME "Hello, OpenGL!")
set(WINDOW_WIDTH 640)
set(WINDOW_HEIGHT 480)
set(SHADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/shader)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/common.cpp src/common.h
    src/shader.cpp src/shader.h
    src/program.cpp src/program.h
    src/context.cpp src/context.h
    src/buffer.cpp src/buffer.h
    src/vertex_layout.cpp src/vertex_layout.h
)

include(Dependency.cmake)

# include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    SHADER_PATH="${SHADER_PATH}"
    )

# Dependency 들이 먼저 build 되도록 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})
