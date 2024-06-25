#include <SlugEngine.h>
#include <Pixelator.h>
#include <iostream>

void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920 ;
const unsigned int SCR_HEIGHT = 1080 ;

float deltaTime = 0.0f;
float lastFrame = 0.0f;           

float timeStep = 1.0f / 30.0f;
int32 velocityIterations = 6;
int32 positionIterations = 2;

bool DrawBackdrop = true;
float guiColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

std::vector<b2Body*> bodies;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

int main()
{
    GLFWwindow* window = initializeWindow("Slug2D", SCR_WIDTH, SCR_HEIGHT, false);
    setWindowIcon(window, "Resources/Textures/ball.png");

    Shader ourShader("Shaders/vertex2d.shad", "Shaders/fragment2d.shad");
    Shader lineShader("Shaders/linevertex.shad", "Shaders/linefragment.shad");
    Shader screenShader("Shaders/screenvertex.shad", "Shaders/screenfragment.shad");

    std::vector<unsigned int> walkFrames = { loadTexture("Resources/Textures/walk/1.png"), loadTexture("Resources/Textures/walk/2.png"), loadTexture("Resources/Textures/walk/3.png"), loadTexture("Resources/Textures/walk/4.png"), loadTexture("Resources/Textures/walk/5.png"), loadTexture("Resources/Textures/walk/6.png"), loadTexture("Resources/Textures/walk/7.png"), loadTexture("Resources/Textures/walk/8.png"), loadTexture("Resources/Textures/walk/9.png"), loadTexture("Resources/Textures/walk/10.png"), loadTexture("Resources/Textures/walk/11.png"), loadTexture("Resources/Textures/walk/12.png"), loadTexture("Resources/Textures/walk/13.png") };
    Animation walkAnimation(walkFrames, 0.05f);

    Renderer ourRenderer;
    ourRenderer.SetGlobalLight(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    Light light(glm::vec3(0.2f, 0.5f, 0.0f), 0.0001f, 4.5f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 20.0f, true);
    ourRenderer.AddLight(light);

    unsigned int texture = loadTexture("Resources/Textures/Slugarius.png");
    unsigned int transparentTexture = loadTexture("Resources/Textures/transparent.png");
    unsigned int skyboxTexture = loadTexture("Resources/Textures/background.png");
    unsigned int smokeTexture = loadTexture("Resources/Textures/smoke.png");
    unsigned int redTexture = loadTexture("Resources/Textures/redlight.png");

    b2World world(b2Vec2(0.0f, -0.5f));

    b2Body* body1 = BoxBody(world, 0.2f, 0.5f, 0.25f, 0.35f, 1.0f);
    b2Body* body2 = BoxBodyStatic(world, 0.0f, -0.5f, 10.25f, 0.25f);
    b2Body* body3 = BoxBody(world, 0.2f, 0.7f, 0.25f, 0.25f, 1.0f);

    Object bodyOne(body1, texture, glm::vec2(0.5f, 0.7f));
    Object bodyTwo(body2, texture, glm::vec2(20.5f, 0.5f));
    Object bodyThree(body3, texture, glm::vec2(0.5f, 0.5f));
    Object bodyFour(NULL, transparentTexture, glm::vec2(1.5f, 1.5f), glm::vec3(0.2f, 0.5f, -2.5f), 0.25f);
    Object skybox(NULL, skyboxTexture, glm::vec2(100.0f, 50.0f), glm::vec3(0.0f, 10.0f, -50.0f), 0.0f);
    Object redLight(NULL, redTexture, glm::vec2(0.2f, 0.2f), glm::vec3(0.6f, 0.5f, 0.0f));
    bodyOne.receivesLight = false;

    bodyOne.AddAnimation("walk", walkAnimation);
    bodyOne.SetCurrentAnimation("walk");

    bodies.push_back(body1);
    bodies.push_back(body2);
    bodies.push_back(body3);

    ParticleSystem smokeParticles(glm::vec2(0.2f, 0.5f), 0.0f, 0.1f, smokeTexture, glm::vec2(2.2f, 2.2f), 0.01f, -0.01f, -0.1f, -5.0f, 0.1f, 0.8f, false, 1000.0f, 100.0f, false, 0.5f);
    ParticleSystem smokeParticles2(glm::vec2(0.2f, 0.5f), 0.0f, 0.1f, smokeTexture, glm::vec2(0.3f, 0.02f), 0.1f, -0.01f, 0.2f, -0.1f, 2.0f, 1.5f, false, 1000.0f, 10.0f, false, 0.5f);

    ourRenderer.AddShadowCaster(bodyThree);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImFont* mainfont = io.Fonts->AddFontFromFileTTF("Resources/Fonts/newfont.ttf", 18.5f);
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        smokeParticles.Update(deltaTime);
        smokeParticles2.Update(deltaTime);

        processInput(window);

        ClearScreen();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        world.Step(timeStep, velocityIterations, positionIterations);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.GetZoom()),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);

        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 7.5f);

        bodyOne.Update(deltaTime);

        skybox.position.x = camera.Position.x;
        skybox.position.y = camera.Position.y;

        Light lightBruh(glm::vec3(bodyOne.body->GetPosition().x, bodyOne.body->GetPosition().y, 0.0f), 0.1f, 8.5f, glm::vec4(1.0f, 0.0f, 0.0f, 0.0f), 20.0f, true);
        ourRenderer.EditLight(0, lightBruh);

        if (DrawBackdrop)
        {
            ourRenderer.Render(ourShader, skybox, view, projection);
        }

        ourRenderer.Render(ourShader, bodyFour, view, projection);
        ourRenderer.Render(ourShader, bodyOne, view, projection);
        ourRenderer.Render(ourShader, bodyTwo, view, projection);
        ourRenderer.Render(ourShader, bodyThree, view, projection);
        ourRenderer.Render(ourShader, redLight, view, projection);
        ourRenderer.RenderParticles(ourShader, smokeParticles, view, projection);
        ourRenderer.RenderParticles(ourShader, smokeParticles2, view, projection);

        if (GetKey(window, GLFW_KEY_LEFT))
        {
            bodyOne.isFlipped = true;
            bodyOne.Animate();
            b2Vec2 pos = bodyOne.body->GetPosition();
            pos.x += -0.004;
            bodyOne.body->SetTransform(pos, bodyOne.body->GetAngle());
            b2Vec2 force(-0.00001f, 0.0f);
            bodyOne.body->ApplyForceToCenter(force, true);
        }
        else if (GetKey(window, GLFW_KEY_RIGHT))
        {
            bodyOne.isFlipped = false;
            bodyOne.Animate();
            b2Vec2 pos = bodyOne.body->GetPosition();
            pos.x += 0.004;
            bodyOne.body->SetTransform(pos, bodyOne.body->GetAngle());
            b2Vec2 force(0.00001f, 0.0f);
            bodyOne.body->ApplyForceToCenter(force, true);
        }
        else
        {
            bodyOne.StopAnimating();
        }

        if (GetKeyDown(window, GLFW_KEY_UP))
        {
            b2Vec2 impulse(0.0f, 0.2f);
            bodyOne.body->ApplyLinearImpulse(impulse, bodyOne.body->GetWorldCenter(), true);
        }
        if (GetKey(window, GLFW_KEY_R))
        {
            smokeParticles.Play();
        }
        if (GetKey(window, GLFW_KEY_T))
        {
            smokeParticles2.Play();
        }

        for (b2Body* body : bodies)
        {
            std::vector<glm::vec2> vertices = GetBoxVertices(body);
            DrawLines(vertices, lineShader, camera, SCR_WIDTH, SCR_HEIGHT);
        }

        ImGui::PushFont(mainfont);
        ImGui::Begin("Slug's Window");
        ImGui::Checkbox("Render backdrop.", &DrawBackdrop);
        ImGui::Checkbox("Make skeleton white.", &bodyOne.isWhite);
        ImGui::ColorEdit4("SKELETON COLOR PICKER", guiColor);
        ImGui::End();

        ImGui::PopFont();

        bodyOne.SetColor(glm::vec4(guiColor[0], guiColor[1], guiColor[2], guiColor[3]));

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        RunProgram(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    EndProgram();
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::DOWN, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(CameraMovement::BACKWARD, deltaTime);
}
