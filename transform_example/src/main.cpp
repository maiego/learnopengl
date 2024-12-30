#include "common.h"
#include "context.h"

void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->Reshape(width, height);
}

void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow* window, double x, double y) {
  auto context = (Context*)glfwGetWindowUserPointer(window);
  context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int mods) {
  auto context = (Context*)glfwGetWindowUserPointer(window);
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  context->MouseButton(button, action, x, y);
}

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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 16);

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

    glfwMakeContextCurrent(window);
    // glad를 활용한 OpenGL 함수 로딩
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
    auto glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    SPDLOG_INFO("OpenGL context version: {}", glVersion);

    auto context = Context::Create();
    if (!context)
    {
        SPDLOG_ERROR("Failed to create context");
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get());

    // 이벤트 콜백 설정
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    
    // GLFW loop 실행
    SPDLOG_INFO("Start GLFW loop");
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        context->ProcessInput(window);
        context->Render();
        glfwSwapBuffers(window);
    }

    context.reset();

    // GLFW 종료
    SPDLOG_INFO("Terminate GLFW");
    glfwTerminate();

    return 0;
}