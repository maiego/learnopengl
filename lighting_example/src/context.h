#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);
    
private:
    Context() {}
    bool Init();
    
    ProgramUPtr m_program;
    VertexLayoutUPtr m_vertexLayout;
    BufferUPtr m_vertexBuffer;
    BufferUPtr m_indexBuffer;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;

    // camera parameter
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 0.0f, 3.0f) };
    glm::vec3 m_cameraDir { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
    float m_cameraYaw { 0.0f };
    float m_cameraPitch { 0.0f };

    bool m_cameraControl { false };
    glm::vec2 m_prevMousePos { glm::vec2(0.0f) };

    // window size
    int m_width { 1280 };
    int m_height { 720 };

    // animation
    bool m_animation { true };

    // ImGui Config Value
    glm::vec4 m_clearColor { glm::vec4(0.1f, 0.2f, 0.3f, 1.0f) };

    // light parameter
    struct Light {
        glm::vec3 position { glm::vec3(3.0f, 3.0f, 3.0f) };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    Light m_light;

    // material parameter
    struct Material {
        glm::vec3 ambient { glm::vec3(1.0f, 0.5f, 0.3f) };
        glm::vec3 diffuse { glm::vec3(1.0f, 0.5f, 0.3f) };
        glm::vec3 specular { glm::vec3(0.5f, 0.5f, 0.5f) };
        float shininess { 32.0f };
    };
    Material m_material;
    
};

#endif // __CONTEXT_H__