#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"

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
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_textureProgram;
    ProgramUPtr m_postProgram;
    ProgramUPtr m_skyboxProgram;
    ProgramUPtr m_envMapProgram;

    MeshUPtr m_box;
    MeshUPtr m_plane;
    //ModelUPtr m_model;

    MaterialUPtr m_planeMaterial;
    MaterialUPtr m_box1Material;
    MaterialUPtr m_box2Material;
    
    TexturePtr m_windowTexture;
    TextureUPtr m_texture;
    TextureUPtr m_texture2;
    CubeTextureUPtr m_cubeTexture;

    FramebufferUPtr m_framebuffer;
    float m_gamma = { 1.0f };

    // camera parameter
    glm::vec3 m_cameraPos { glm::vec3(0.0f, 2.5f, 8.0f) };
    glm::vec3 m_cameraDir { glm::vec3(0.0f, 0.0f, -1.0f) };
    glm::vec3 m_cameraUp { glm::vec3(0.0f, 1.0f, 0.0f) };
    float m_cameraYaw { 0.0f };
    float m_cameraPitch { -20.0f };

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
    struct DirectionalLight {
        glm::vec3 direction { glm::vec3(-2.0f, -1.0f, -3.0f) };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };

    struct PointLight {
        glm::vec3 position { glm::vec3(3.0f, 3.0f, 3.0f) };
        float distance { 32.0f };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };

    struct SpotLight {
        glm::vec3 position { glm::vec3(1.0f, 4.0f, 4.0f) };
        glm::vec3 direction { glm::vec3(-1.0f, -1.0f, -1.0f) };
        glm::vec2 cutoff { glm::vec2(120.0f, 5.0f) };
        float distance { 128.0f };
        glm::vec3 ambient { glm::vec3(0.1f, 0.1f, 0.1f) };
        glm::vec3 diffuse { glm::vec3(0.5f, 0.5f, 0.5f) };
        glm::vec3 specular { glm::vec3(1.0f, 1.0f, 1.0f) };
    };
    SpotLight m_light;
};

#endif // __CONTEXT_H__