cmake_minimum_required(VERSION 3.29)

set(PROJECT_NAME cmake_project_example)
set(CMAKE_CXX_STANDARD 17)

# spdlog 라이브러리가 MSVC 특정 버전에서 명시적으로 /utf-8 컴파일 옵션 주지 않으면 에러 발생하기 때문에 추가
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

# 프로젝트 환경변수 설정
set(WINDOW_NAME "Hello, OpenGL!")
set(WINDOW_WIDTH 640)
set(WINDOW_HEIGHT 480)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} src/main.cpp)

include(Dependency.cmake)

# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep-spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)


# include / lib 관련 옵션 추가
target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_compile_definitions(${PROJECT_NAME} PUBLIC
    WINDOW_NAME="${WINDOW_NAME}"
    WINDOW_WIDTH=${WINDOW_WIDTH}
    WINDOW_HEIGHT=${WINDOW_HEIGHT}
    )

# Dependency 들이 먼저 build 되도록 관계 설정
add_dependencies(${PROJECT_NAME} ${DEP_LIST})

