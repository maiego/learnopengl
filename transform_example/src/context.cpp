#include "context.h"

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init()
{
    ShaderPtr vertexShader = Shader::CreateFromFile("/texture.vs", GL_VERTEX_SHADER);
    ShaderPtr fragmentShader = Shader::CreateFromFile("/texture.fs", GL_FRAGMENT_SHADER);

    if (vertexShader == nullptr || fragmentShader == nullptr)
    {
        return false;
    }

    SPDLOG_INFO("vertex shader id: {}", vertexShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragmentShader->Get());

    m_program = Program::Create({ vertexShader, fragmentShader});
    if (m_program == nullptr)
    {
        return false;
    }
    
    SPDLOG_INFO("program id: {}", m_program->Get());

    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, 1.0f, 1.0f, 0.0f, 1.0f);

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    //glEnable(GL_MULTISAMPLE);

    // cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        0.5f,  0.5f,  0.5f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 0.0f,
    };

    uint32_t indices[] = {
        0,  2,  1,  2,  0,  3,
        4,  5,  6,  6,  7,  4,
        8,  9, 10, 10, 11,  8,
        12, 14, 13, 14, 12, 15,
        16, 17, 18, 18, 19, 16,
        20, 22, 21, 22, 20, 23,
    };

    m_vertexLayout = VertexLayout::Create();
    
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 120);
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 36);

    // pos(3), color(3), uv(2)
    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, 0);
    //m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, sizeof(float)*3);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, sizeof(float)*3);

    auto image = Image::Load("/container.jpg");

    // 체크 무늬 생성
    // auto image = Image::Create(512, 512);
    // if (!image)
    // {
    //     return false;
    // }
    // image->SetCheckImage(16, 16);
    // SPDLOG_INFO("image: {}x{}, {} channels", image->GetWidth(), image->GetHeight(), image->GetChannelCount());

    // Texture 생성
    m_texture = Texture::CreateFromImage(image.get());

    auto image2 = Image::Load("/awesomeface.png");
    m_texture2 = Texture::CreateFromImage(image2.get());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture->Get());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture2->Get());

    m_program->Use();
    m_program->SetUniform("tex", 0);
    m_program->SetUniform("tex2", 1);

    // transformation 테스트
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);

    // (1,1,0) 평행이동하는 행렬
    auto trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f));

    // z축으로 90도 회전하는 행렬
    auto rot = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // 모든 축에 대해 3배율 확대하는 행렬
    auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(3.0f));

    // 확대->회전->평행이동 순으로 선형변환 적용
    vec = trans * rot * scale * vec;
    SPDLOG_INFO("vec: [{}, {}, {}]", vec.x, vec.y, vec.z);

    auto model = glm::rotate(glm::mat4(1.0f),
        glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    auto view = glm::translate(glm::mat4(1.0f),
        glm::vec3(0.0f, 0.0f, -3.0f));

    auto projection = glm::perspective(glm::radians(45.0f),
        (float) 640 / (float)480, 0.01f, 10.0f);

    auto transform = projection * view * model;

    m_program->SetUniform("transform", transform);

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // static float time = 0;
    // float t = sinf(time) * 0.5f + 0.5f;

    // auto loc = glGetUniformLocation(m_program->Get(), "color");
    // m_program->Use();
    // glUniform4f(loc, t*t, 2*t*(1-t), (1-t)*(1-t), 1.0f);

    // time += 0.016f;

    std::vector<glm::vec3> cubePositions = {
        glm::vec3( 0.0f, 0.0f, 0.0f),
        glm::vec3( 2.0f, 5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f, 2.0f, -2.5f),
        glm::vec3( 1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f),
    };

    m_cameraDir =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDir, m_cameraUp);

    for (int i = 0; i < cubePositions.size(); i++)
    {
        auto model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
        model = glm::rotate(model,
            glm::radians(static_cast<float>(glfwGetTime()) * 120.0f + 20.0f * i), glm::vec3(1.0f, 0.5f, 0.0f));

        auto projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(m_width) / static_cast<float>(m_height), 0.01f, 20.0f);

        auto transform = projection * view * model;

        m_program->SetUniform("transform", transform);
        
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }

    // auto model = glm::rotate(glm::mat4(1.0f),
    //     glm::radians(static_cast<float>(glfwGetTime()) * 120.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    // auto view = glm::translate(glm::mat4(1.0f),
    //     glm::vec3(0.0f, 0.0f, -3.0f));

    // auto projection = glm::perspective(glm::radians(45.0f),
    //     (float) 640 / (float)480, 0.01f, 10.0f);

    // auto transform = projection * view * model;

    // m_program->SetUniform("transform", transform);

    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

void Context::ProcessInput(GLFWwindow* window)
{
    const float cameraSpeed = 0.05f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * m_cameraDir;
    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * m_cameraDir;


    auto cameraRight = glm::normalize(glm::cross(m_cameraUp, -m_cameraDir));
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraRight;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraRight;


    auto cameraUp = glm::normalize(glm::cross(-m_cameraDir, cameraRight));

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        m_cameraPos += cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        m_cameraPos -= cameraSpeed * cameraUp;


    if (!m_cameraControl)
        return;
}

void Context::Reshape(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);
}

void Context::MouseMove(double x, double y)
{
    if (!m_cameraControl)
        return;

    auto pos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
    auto deltaPos = pos - m_prevMousePos;

    const float cameraRotSpeed = 0.5f;

    m_cameraYaw -= deltaPos.x * cameraRotSpeed;
    m_cameraPitch -= deltaPos.y * cameraRotSpeed;

    if (m_cameraYaw < 0.0f)
        m_cameraYaw += 360.0f;
    if (m_cameraYaw > 360.0f)
        m_cameraYaw -= 360.0f;

    if (m_cameraPitch > 89.0f)
        m_cameraPitch = 89.0f;
    if (m_cameraPitch < -89.0f)
        m_cameraPitch = -89.0f;

    m_prevMousePos = pos;
}

void Context::MouseButton(int button, int action, double x, double y)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT)
    {
        if (action == GLFW_PRESS)
        {
            m_cameraControl = true;
            m_prevMousePos = glm::vec2(static_cast<float>(x), static_cast<float>(y));
        }
        else if (action == GLFW_RELEASE)
        {
            m_cameraControl = false;
        }
    }
}