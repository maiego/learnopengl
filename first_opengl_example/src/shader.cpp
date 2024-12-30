#include "shader.h"

ShaderUPtr Shader::CreateFromFile(const std::string& filename, GLenum shaderType)
{
    std::string filepath = std::string(SHADER_PATH) + filename;
    ShaderUPtr shader = ShaderUPtr(new Shader());
    if (!shader->LoadFile(filepath, shaderType))
        return nullptr;
    return std::move(shader);
}

bool Shader::LoadFile(const std::string &filename, GLenum shaderType)
{
    auto result = LoadTextFile(filename);
    if (!result.has_value())
        return false;

    auto& code = result.value();
    const char* codePtr = code.c_str();
    int32_t codeLength = (int32_t)code.length();
    
    m_shader = glCreateShader(shaderType);
    glShaderSource(m_shader, 1, &codePtr, &codeLength);
    glCompileShader(m_shader);

    int sucess = 0;
    glGetShaderiv(m_shader, GL_COMPILE_STATUS, &sucess);
    if (!sucess)
    {
        char infoLog[512];
        glGetShaderInfoLog(m_shader, 512, nullptr, infoLog);
        SPDLOG_ERROR("Failed to compile shader: \"{}\"", filename);
        SPDLOG_ERROR("reason: {}", infoLog);
        return false;
    }
    return true;
}

Shader::~Shader()
{
    if (m_shader)
    {
        glDeleteShader(m_shader);
    }
}

