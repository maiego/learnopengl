#include "context.h"
#include "imgui.h"
#include <glm/gtc/random.hpp>

ContextUPtr Context::Create()
{
    auto context = ContextUPtr(new Context());
    if (!context->Init())
        return nullptr;
    return std::move(context);
}

bool Context::Init()
{
    m_framebuffer = Framebuffer::Create(Texture::Create(m_width, m_height, GL_RGBA));
    if (m_framebuffer == nullptr)
    {
        return false;
    }

    m_box = Mesh::CreateBox();

    m_plane = Mesh::CreatePlane();
    m_windowTexture = Texture::CreateFromImage(
        Image::Load("/blending_transparent_window.png").get());

    TexturePtr darkGrayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)).get());

    TexturePtr grayTexture = Texture::CreateFromImage(
        Image::CreateSingleColorImage(4, 4, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)).get());

    m_planeMaterial = Material::Create();
    m_planeMaterial->diffuse = Texture::CreateFromImage(
        Image::Load("/marble.jpg").get());
    m_planeMaterial->specular = grayTexture;
    m_planeMaterial->shininess = 128.0f;

    m_box1Material = Material::Create();
    m_box1Material->diffuse = Texture::CreateFromImage(
        Image::Load("/container.jpg").get());
    m_box1Material->specular = darkGrayTexture;
    m_box1Material->shininess = 16.0f;

    m_box2Material = Material::Create();
    m_box2Material->diffuse = Texture::CreateFromImage(
        Image::Load("/container2.png").get());
    m_box2Material->specular = Texture::CreateFromImage(
        Image::Load("/container2_specular.png").get());
    m_box2Material->shininess = 64.0f;

    m_simpleProgram = Program::Create("/simple.vs", "/simple.fs");
    if (m_simpleProgram == nullptr)
    {
        return false;
    }
    SPDLOG_INFO("simpleProgram id: {}", m_simpleProgram->Get());

    m_program = Program::Create("/lighting.vs", "/spot_lighting.fs");
    if (m_program == nullptr)
    {
        return false;
    }
    SPDLOG_INFO("program id: {}", m_program->Get());

    m_textureProgram = Program::Create("/texture.vs", "/texture.fs");
    if (m_textureProgram == nullptr)
        return false;

    m_postProgram = Program::Create("/texture.vs", "/gamma.fs");

    auto cubeRight = Image::Load("/skybox/right.jpg", false);
    auto cubeLeft = Image::Load("/skybox/left.jpg", false);
    auto cubeTop = Image::Load("/skybox/top.jpg", false);
    auto cubeBottom = Image::Load("/skybox/bottom.jpg", false);
    auto cubeFront = Image::Load("/skybox/front.jpg", false);
    auto cubeBack = Image::Load("/skybox/back.jpg", false);
    
    std::vector<Image *> images;
    images.push_back(cubeRight.get());
    images.push_back(cubeLeft.get());
    images.push_back(cubeTop.get());
    images.push_back(cubeBottom.get());
    images.push_back(cubeFront.get());
    images.push_back(cubeBack.get());
    m_cubeTexture = CubeTexture::CreateFromImages(images);

    m_skyboxProgram = Program::Create("/skybox.vs", "/skybox.fs");
    m_envMapProgram = Program::Create("/env_map.vs", "/env_map.fs");

    m_grassTexture = Texture::CreateFromImage(
        Image::Load("/grass.png").get());
    m_grassProgram = Program::Create("/grass.vs", "/grass.fs");

    
    m_grassPos.resize(10000);
    for (auto& pos : m_grassPos)
    {
        pos.x = glm::linearRand(-5.0f, 5.0f);
        pos.z = glm::linearRand(-5.0f, 5.0f);
        pos.y = glm::radians(glm::linearRand(0.0f, 360.0f));
    }

    m_grassInstance = VertexLayout::Create();
    m_grassInstance->Bind();

    m_plane->GetVertexBuffer()->Bind();
    m_grassInstance->SetAttrib(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    m_grassInstance->SetAttrib(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal));
    m_grassInstance->SetAttrib(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, texCoord));
    
    m_grassPosBuffer = Buffer::CreateWithData(GL_ARRAY_BUFFER, GL_STATIC_DRAW, m_grassPos.data(), sizeof(glm::vec3), m_grassPos.size());
    m_grassPosBuffer->Bind();
    m_grassInstance->SetAttrib(3, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
    glVertexAttribDivisor(3, 1);
    m_plane->GetIndexBuffer()->Bind();

    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glEnable(GL_MULTISAMPLE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    return true;
}

void Context::Render()
{
    if (ImGui::Begin("UI window"))
    {
        if (ImGui::ColorEdit4("clear color", glm::value_ptr(m_clearColor)))
        {
            glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
        }
        ImGui::Separator();

        ImGui::DragFloat3("camera pos", glm::value_ptr(m_cameraPos), 0.01f);
        ImGui::DragFloat("camera yaw", &m_cameraYaw, 0.5f);
        ImGui::DragFloat("camera pitch", &m_cameraPitch, 0.5f, -89, 89);
        ImGui::Separator();

        if (ImGui::Button("Reset Camera"))
        {
            m_cameraPos = glm::vec3(0.0f, 3.4f, 11.436f);
            m_cameraYaw = 358.5f;
            m_cameraPitch = -24.5;
        }

        if (ImGui::CollapsingHeader("light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("l.position", glm::value_ptr(m_light.position), 0.01f);
            ImGui::DragFloat3("l.direction", glm::value_ptr(m_light.direction), 0.01f);
            ImGui::DragFloat2("l.cutoff", glm::value_ptr(m_light.cutoff), 0.01f);
            ImGui::DragFloat("l.distance", &m_light.distance, 0.05f, 0.0f, 3000.0f);
            ImGui::ColorEdit3("l.ambient", glm::value_ptr(m_light.ambient), 0.01f);
            ImGui::ColorEdit3("l.diffuse", glm::value_ptr(m_light.diffuse), 0.01f);
            ImGui::ColorEdit3("l.specular", glm::value_ptr(m_light.specular), 0.01f);
        }
        
        ImGui::Checkbox("animation", &m_animation);
        ImGui::Separator();

        if (ImGui::CollapsingHeader("framebuffer", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("gamma", &m_gamma, 0.01f, 0.0f, 2.0f);
            float aspectRatio = 1.5f;
            if (m_framebuffer->GetColorAttachment()->GetHeight() > 0)
            {
                aspectRatio = m_framebuffer->GetColorAttachment()->GetWidth() / m_framebuffer->GetColorAttachment()->GetHeight();
            }
            ImGui::Image(m_framebuffer->GetColorAttachment()->Get(), ImVec2(300*aspectRatio, 300));
        }

        std::string fps = "FPS : " + std::to_string((int)ImGui::GetIO().Framerate);
        ImGui::Text(fps.c_str());
    }
    ImGui::End();
    
    m_cameraDir =
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraYaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(m_cameraPitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
        glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    auto view = glm::lookAt(m_cameraPos, m_cameraPos + m_cameraDir, m_cameraUp);
    auto projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100.0f);
    
    m_framebuffer->Bind();
    glClearColor(m_clearColor.r, m_clearColor.g, m_clearColor.b, m_clearColor.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    auto skyboxModelTransform =
        glm::translate(glm::mat4(1.0f), m_cameraPos) *
        glm::scale(glm::mat4(1.0f), glm::vec3(50.0f));
    m_skyboxProgram->Use();
    m_cubeTexture->Bind();
    m_skyboxProgram->SetUniform("skybox", 0);
    m_skyboxProgram->SetUniform("transform", projection * view * skyboxModelTransform);
    m_box->Draw(m_skyboxProgram.get());

    // light 에 cube 그리기
    {
        auto lightModelTransform = 
            glm::translate(glm::mat4(1.0f), m_light.position) *
            glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        
        m_simpleProgram->Use();
        m_simpleProgram->SetUniform("color", glm::vec4(m_light.ambient + m_light.diffuse, 1.0f));
        m_simpleProgram->SetUniform("transform", projection * view * lightModelTransform);
        
        m_box->Draw(m_simpleProgram.get());
    }

    m_program->Use();
    m_program->SetUniform("viewPos", m_cameraPos);
    m_program->SetUniform("light.position", m_light.position);
    m_program->SetUniform("light.direction", m_light.direction);
    m_program->SetUniform("light.cutoff", glm::vec2(
        cosf(glm::radians(m_light.cutoff[0])),
        cosf(glm::radians(m_light.cutoff[0] + m_light.cutoff[1]))));
    m_program->SetUniform("light.attenuation", GetAttenuationCoeff(m_light.distance));
    //m_program->SetUniform("light.direction", m_light.direction);
    m_program->SetUniform("light.ambient", m_light.ambient);
    m_program->SetUniform("light.diffuse", m_light.diffuse);
    m_program->SetUniform("light.specular", m_light.specular);

    // 판 그리기    
    auto modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.7f, 2.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(10.0f, 1.0f, 10.0f));

    auto transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_planeMaterial->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // 상자 1 그리기
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 2.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));

    transform = projection * view * modelTransform;
    m_program->SetUniform("transform", transform);
    m_program->SetUniform("modelTransform", modelTransform);
    m_box2Material->SetToProgram(m_program.get());
    m_box->Draw(m_program.get());

    // 상자 2 그리기
    modelTransform =
        glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 0.0f)) *
        glm::rotate(glm::mat4(1.0f), glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));

    transform = projection * view * modelTransform;
    m_program->Use();
    m_program->SetUniform("color", glm::vec4(1.0f, 1.0f, 0.5f, 1.0f));
    m_program->SetUniform("transform", transform *
        glm::scale(glm::mat4(1.0f), glm::vec3(1.05f, 1.05f, 1.05f)));
    m_box->Draw(m_program.get());

    // env mappping 박스 그리기
    modelTransform =
    glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.75f, 3.0f)) *
    glm::rotate(glm::mat4(1.0f), glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f)) *
    glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));

    m_envMapProgram->Use();
    m_envMapProgram->SetUniform("model", modelTransform);
    m_envMapProgram->SetUniform("view", view);
    m_envMapProgram->SetUniform("projection", projection);
    m_envMapProgram->SetUniform("cameraPos", m_cameraPos);

    m_cubeTexture->Bind();
    m_envMapProgram->SetUniform("skybox", 0);
    m_box->Draw(m_envMapProgram.get());

    // 투명빨간 창문 그리기
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_textureProgram->Use();
    m_windowTexture->Bind();
    m_textureProgram->SetUniform("tex", 0);

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 4.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.5f, 5.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.5f, 6.0f));
    transform = projection * view * modelTransform;
    m_textureProgram->SetUniform("transform", transform);
    m_plane->Draw(m_textureProgram.get());

    glDisable(GL_CULL_FACE);
    m_grassProgram->Use();
    m_grassTexture->Bind();
    m_grassProgram->SetUniform("tex", 0);
    m_grassInstance->Bind();

    modelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 2.0f));
    transform = projection * view * modelTransform;
    m_grassProgram->SetUniform("transform", transform);

    glDrawElementsInstanced(GL_TRIANGLES,
        m_plane->GetIndexBuffer()->GetCount(),
        GL_UNSIGNED_INT, 0,
        m_grassPosBuffer->GetCount());
    
    // instancing 없이 그리기
    // for (size_t i=0; i<m_grassPos.size(); ++i)
    // {
    //     modelTransform =
    //         glm::translate(glm::mat4(1.0f), glm::vec3(m_grassPos[i].x, 0.5f, m_grassPos[i].z + 2.0f)) *
    //         glm::rotate(glm::mat4(1.0f), m_grassPos[i].y, glm::vec3(0.0f, 1.0f, 0.0f));
    //     transform = projection * view * modelTransform;
    //     m_grassProgram->SetUniform("transform", transform);
    //     m_plane->Draw(m_grassProgram.get());
    // }

    m_framebuffer->BindToDefault();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    m_postProgram->Use();
    m_postProgram->SetUniform("transform", glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f)));
    m_postProgram->SetUniform("gamma", m_gamma);

    m_framebuffer->GetColorAttachment()->Bind();
    m_postProgram->SetUniform("tex", 0);
    m_plane->Draw(m_postProgram.get());
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