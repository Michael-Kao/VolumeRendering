#include "Application.h"

Application::Application(Window *wnd_ptr)
    :   m_window(wnd_ptr) {
    Init();
}

Application::Application() {
    m_window = new Window();
    Init();
}

Application::~Application() {
    glfwTerminate();
}

void Application::Init() {
    m_cube = new Object("assets/objfiles/cube.obj");
    m_shader = new Shader("assets/shaders/volume.vert", "assets/shaders/volume.frag");
    m_camera = new Camera();

    // 創建 ImGui 上下文
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // 初始化 ImGui 的 GLFW 繫結
    ImGui_ImplGlfw_InitForOpenGL(m_window->getWindow(), true);

    // 初始化 ImGui 的 OpenGL3 繫結
    ImGui_ImplOpenGL3_Init("#version 450");

    // 建立字體圖集
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    // 設置 ImGui 的風格（可選）
    ImGui::StyleColorsDark();
}

void Application::drawTest() {
    m_shader->bind();
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);
    // projection = glm::perspective(glm::radians(45.0f), (float)m_window->getWidth() / (float)m_window->getHeight(), 0.1f, 100.0f);
    // view       = glm::translate(view, glm::vec3(0.0f, 0.0f, -10.0f));
    projection = glm::perspective(glm::radians(m_camera->fov), (float)m_window->getWidth() / (float)m_window->getHeight(), 0.1f, 100.0f);
    view       = m_camera->getLookAt();
    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(45.0f), glm::vec3(1, 0, 0));
    m_shader->setMat4("model", model);
    m_cube->draw();
    m_shader->unbind();
}

void Application::processCamera(float delta_time) {
    if(m_window->isPressed(GLFW_KEY_W)){
        m_camera->updateInput(FORWARD, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_S)){
        m_camera->updateInput(BACKWARD, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_A)){
        m_camera->updateInput(LEFT, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_D)){
        m_camera->updateInput(RIGHT, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_K)){
        m_camera->updateInput(RPITCH, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_J)){
        m_camera->updateInput(PITCH, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_L)){
        m_camera->updateInput(YAW, delta_time);
    }
    if(m_window->isPressed(GLFW_KEY_H)){
        m_camera->updateInput(RYAW, delta_time);
    }
}

void Application::processInput() {
    if(m_window->isPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(m_window->getWindow(), true);
    }
}

void Application::run() {
    TransferFunc test;

    // timing
    float deltaTime = 0.0f;	// time between current frame and last frame
    float lastFrame = 0.0f;
    std::string filePath = "assets\\Scalar\\";
    Model model(filePath, "skull.inf");
    std::cout << "done\n";
    m_camera->Position = model.center + glm::vec3(0, 0, 500);
    // for Rendering semi-transparent textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    while(!m_window->shouldClose()) {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // glViewport(0, 0, m_window->getWidth()/2, m_window->getHeight()/2);
        m_window->setClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        m_window->clear();

        processCamera(deltaTime);
        processInput();

        glViewport(0, 0, m_window->getWidth(), m_window->getHeight());
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // drawTest();
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        if (ImGui::GetCurrentContext() == NULL) {
            std::cout << "ImGui is not initialized!" << std::endl;
            return;
        } 


        m_camera->width = m_window->getWidth();
        m_camera->height = m_window->getHeight();
        m_camera->center = model.center;
        m_shader->bind();
        // m_shader->setVec3("eyePos", glm::vec3(0, 0, 0));
        m_shader->setVec3("eyePos", m_camera->Position);
        m_shader->setVec3("lightPos", m_camera->Position);
        m_shader->setMat4("transform", m_camera->ViewProjectionMatrix());
        m_shader->setInt("volume", 0);
        m_shader->setInt("transformation", 1);
        // m_shader->setVec3("light_color", {1.f, 1.f, 1.f});
        // m_shader->setVec3("model_color", {0.f, 0.5f, 1.f});
        // m_shader->setMat4("view_proj_matrix", m_camera->ViewProjectionMatrix());
        // m_shader->setVec3("light_src", m_camera->Position);
        // m_shader->setVec3("light_pos", m_camera->Position);
        if(test.isChanged) {
            test.isChanged = false;
            model.reBindTexture(test.GetTransferFun());
        }
        model.DrawVolume();

        {
            ImGuiIO &io = ImGui::GetIO();
            io.DisplaySize = ImVec2((float)m_window->getWidth() / 2, (float)m_window->getHeight() / 2);
            // Start the Dear ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // 創建一個新的 ImGui 視窗
            ImGui::Begin("Camera Info");

            // 顯示 m_camera 的信息
            ImGui::Text("Position: (%f, %f, %f)", m_camera->Position.x, m_camera->Position.y, m_camera->Position.z);
            ImGui::Text("Front: (%f, %f, %f)", m_camera->Front.x, m_camera->Front.y, m_camera->Front.z);
            ImGui::Text("Up: (%f, %f, %f)", m_camera->Up.x, m_camera->Up.y, m_camera->Up.z);

            // // 創建一個滑塊來調整 m_camera 的 Yaw 和 Pitch
            // ImGui::SliderFloat("Yaw", &m_camera->Yaw, -180.0f, 180.0f);
            // ImGui::SliderFloat("Pitch", &m_camera->Pitch, -89.0f, 89.0f);

            // 結束 ImGui 視窗
            ImGui::End();

            ImGui::Begin("Transfer Function");
            test.Draw();
            ImGui::End();

            // 在每一幀的結束，你需要調用 ImGui::Render() 函數，然後調用 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()) 函數
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        m_window->swapBuffers();
    }
}