#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

int main(int argc, const char** argv)
{
    SPDLOG_INFO("Start program");
    // window 환경변수 출력
    SPDLOG_INFO("Window Name: {}", WINDOW_NAME);
    SPDLOG_INFO("Window Width: {}", WINDOW_WIDTH);
    SPDLOG_INFO("Window Height: {}", WINDOW_HEIGHT);

    // glfw 초기화
    SPDLOG_INFO("Initialize GLFW");
    if (!glfwInit())
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("Failed to initialize GLFW: {}", description);
        return -1;
    }

    // GLFW window 생성
    SPDLOG_INFO("Create GLFW window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
    if (!window)
    {
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("Failed to create GLFW window: {}", description);
        glfwTerminate();
        return -1;
    }

    // GLFW loop 실행
    SPDLOG_INFO("Start GLFW loop");
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // GLFW 종료
    SPDLOG_INFO("Terminate GLFW");
    glfwTerminate();

    return 0;
}