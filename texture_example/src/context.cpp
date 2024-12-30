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

    float vertices[] = { // x,y,z, r,g,b, s,t
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    };

    uint32_t indices[] = { // note that we start from 0!
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };

    m_vertexLayout = VertexLayout::Create();
    
    m_vertexBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, vertices, sizeof(float) * 32);
    m_indexBuffer = Buffer::CreateWithData(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, indices, sizeof(uint32_t) * 6);

    m_vertexLayout->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, 0);
    m_vertexLayout->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, sizeof(float)*3);
    m_vertexLayout->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(float)*8, sizeof(float)*6);

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
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex"), 0);
    glUniform1i(glGetUniformLocation(m_program->Get(), "tex2"), 1);

    return true;
}

void Context::Render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float time = 0;
    float t = sinf(time) * 0.5f + 0.5f;

    auto loc = glGetUniformLocation(m_program->Get(), "color");
    m_program->Use();
    glUniform4f(loc, t*t, 2*t*(1-t), (1-t)*(1-t), 1.0f);

    time += 0.016f;

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}