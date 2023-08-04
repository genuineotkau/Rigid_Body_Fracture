#define GLM_ENABLE_EXPERIMENTAL
#define FPS_LIMIT 70.0f

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// animated GIF writer
#include <GIF/gif.h>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "RigidBody.h"
#include "Simulation.h"
#include "RigidBodyPicker.h"
#include "MouseRaycasting.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <conio.h>

// image loader and writer
#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image.h"
//#include "stb_image/stb_image_write.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
Simulation* createSimulation();
void DemoCollision();

void Demo1();
void Demo2();
void Demo3();
void Demo4();
void Demo5();
void Demo6();
void Demo7();
void Demo8();

// GIF recording params
static bool globalRecording = false;
static GifWriter globalGIFfile;
constexpr int globalGIFdelay = 1;

// settings
const unsigned int globalWidth = 1280;
const unsigned int globalHeight = 800;

// camera
Camera camera(glm::vec3(0.0f, 5.0f, 5.0f));
float lastX = globalWidth / 2.0f;
float lastY = globalHeight / 2.0f;
bool firstMouse = true;
bool lockCam = false;
// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// raycasting
RigidBodyPicker picker(&camera);
MouseRaycasting raycast(&camera);
glm::vec3 ray;
glm::mat4 projection;
glm::mat4 view;
glm::vec3 deltaPos;
Simulation* simulation;
Shader* mainShader;
Model* objModel;
//Model* cube = new Model(std::string("resources/objects/cube2.obj"));
Model* quad;

bool hideObjs = false;
// 0: not generate ray, 1: generate ray but not apply force, 2: generate ray and apply force
int fireRay = 0;
bool showAABB = false;
bool triggerDemo3 = false;
float forceStrength = 0.0f;
float duration = 0.0f;
float angularRatio = 0.1f;
glm::vec3 g = glm::vec3(0.f, -1.98f, 0.f); /*virual gravitational acceleration, smaller to show effect*/

// fast random number generator based pcg32_fast
#include <stdint.h>
#include <future>
namespace PCG32 {
    static uint64_t mcg_state = 0xcafef00dd15ea5e5u;	// must be odd
    static uint64_t const multiplier = 6364136223846793005u;
    uint32_t pcg32_fast(void) {
        uint64_t x = mcg_state;
        const unsigned count = (unsigned)(x >> 61);
        mcg_state = x * multiplier;
        x ^= x >> 22;
        return (uint32_t)(x >> (22 + count));
    }
    float rand() {
        return float(double(pcg32_fast()) / 4294967296.0);
    }
}

int main()
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(globalWidth, globalHeight, "CollideSimulation", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_click_callback);
    void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods);

    // tell GLFW to capture our mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    simulation = createSimulation();

    // draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        glfwSetKeyCallback(window, key_callback);

        // background color
        glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view/projection transformations
        projection = glm::perspective(glm::radians(camera.Zoom), (float)globalWidth / (float)globalHeight, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        for (int i = 0; i < simulation->objs.size(); ++i) {
            simulation->objs[i]->shader->use();
            simulation->objs[i]->shader->setVec3("viewPos", camera.Position);
        }

        if (triggerDemo3) {
            for (int i = 0; i < simulation->objs.size(); ++i) {
                if (simulation->objs[i]->position.y < -1) {
                    simulation->objs[i]->SetSelect(true);
                }
            }
        }

        simulation->UpdateAABB();
        simulation->BuildBVH();


        
        // render
        // update object positions
        simulation->UpdatePositions(deltaTime);
        if (!hideObjs)
            simulation->Render(projection, view);
        // for showing AABB
        if (showAABB)
            simulation->RenderTree(&(simulation->GetTreeRoot()), projection, view);

        //GIF_Recording
        if (globalRecording) {
            // Create a buffer to hold the pixel data
            std::vector<uint8_t> pixels(globalWidth * globalHeight * 4); // 4 for RGBA

            // Read pixels from the framebuffer into the buffer
            glReadPixels(0, 0, globalWidth, globalHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

            // gif library expects the data in a different order (top-to-bottom), 
            // but OpenGL gives us the data bottom-to-top. So we need to flip the rows.
            std::vector<uint8_t> flipped_pixels(globalWidth * globalHeight * 4);
            for (int y = 0; y < globalHeight; ++y) {
                for (int x = 0; x < globalWidth; ++x) {
                    for (int c = 0; c < 4; ++c) {
                        flipped_pixels[(y * globalWidth + x) * 4 + c] =
                            pixels[((globalHeight - 1 - y) * globalWidth + x) * 4 + c];
                    }
                }
            }

            // Write the frame to the gif
            GifWriteFrame(&globalGIFfile, flipped_pixels.data(), globalWidth, globalHeight, globalGIFdelay);
        }


        glfwSwapBuffers(window);
        glfwPollEvents();

        // limit the frame rate to prevent the animation from going too fast
        float target_frame_time = 1.0 / FPS_LIMIT;
        float frameEnd = glfwGetTime();
        float frameTime = frameEnd - currentFrame; // time taken to process this frame
        if (frameTime < target_frame_time)
        {
            float delayTime = target_frame_time - frameTime;
            // convert delayTime from seconds to milliseconds, as required by the delay function
            int delayMillis = static_cast<int>(delayTime * 1000);
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMillis));
        }

    }

    glfwTerminate();

    // Wait for user input before closing the console
    std::cout << "Press any key to exit...";
    _getch();

    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_W:
            camera.ProcessKeyboard(FORWARD, deltaTime);
            break;
        case GLFW_KEY_S:
            camera.ProcessKeyboard(BACKWARD, deltaTime);
            break;
        case GLFW_KEY_A:
            camera.ProcessKeyboard(LEFT, deltaTime);
            break;
        case GLFW_KEY_D:
            camera.ProcessKeyboard(RIGHT, deltaTime);
            break;
        case GLFW_KEY_LEFT_SHIFT:
            camera.ProcessKeyboard(DOWN, deltaTime);
            break;
        case GLFW_KEY_SPACE:
            camera.ProcessKeyboard(UP, deltaTime);
            break;
        case GLFW_KEY_L:
            lockCam = !lockCam;
            break;
        case GLFW_KEY_H:
            hideObjs = !hideObjs;
            break;
        case GLFW_KEY_G:
            showAABB = !showAABB;
            break;
        case GLFW_KEY_F:
            if (!globalRecording) {
                char fileName[1024];
                sprintf_s(fileName, "output%d.gif", int(1000.0 * PCG32::rand()));
                printf("Saving \"%s\"...\n", fileName);
                GifBegin(&globalGIFfile, fileName, globalWidth, globalHeight, globalGIFdelay);
                globalRecording = true;
                printf("(Recording started)\n");
            }
            else {
                GifEnd(&globalGIFfile);
                globalRecording = false;
                printf("(Recording done)\n");
            }
            break;
        default:
            break;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    if (!lockCam)
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        RigidBody* rb;
        if (fireRay == 0) return;
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            simulation->ResetSelectStatus();
            ray = raycast.GenerateMouseRay2(lastX, lastY, globalWidth, globalHeight, projection, view);
            if (fireRay == 2) {
                // determine if force is big enough to "break" objects
                if (forceStrength > 10.0f) {
                    for (int i = 0; i < simulation->objs.size(); ++i) {
                        if (simulation->objs[i]->IsStatic()) continue;
                        // change pieces and whole objects status 
                        if (simulation->objs[i]->IsPieces())
                            simulation->objs[i]->SetHide(false);
                        else 
                            simulation->objs[i]->SetHide(true);
                    }
                }
            }

            rb = picker.Click(simulation, ray, deltaPos);
            if (rb) {
                if (fireRay == 1) {
                    rb->SetSelect(true);
                }
                else if (fireRay == 2) {
                    float t = 0.1;
                    //rb->torque = glm::vec3(abs(ray.x) > t ? 1 : 0, abs(ray.y) > t ? 1 : 0, abs(ray.z) > t ? 1 : 0);
                    if (forceStrength <= 10.0f) {
                        if (abs(ray.x) > t || abs(ray.z) > t)
                            rb->torque = glm::vec3(0, 1, 0);
                        rb->angularVel = forceStrength * angularRatio;
                    }
                    rb->ApplyForce(forceStrength * ray, duration);

                    for (int i = 0; i < simulation->objs.size(); ++i) {
                        if (simulation->objs[i]->IsHide() || simulation->objs[i]->IsStatic()) continue;
                        simulation->objs[i]->ApplyG(g);
                    }
                }
            }
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            break;
        default:
            return;
        }
    }
    return;
}

// Demo is here
Simulation* createSimulation()
{
    simulation = new Simulation();
    
    mainShader = new Shader("resources/shaders/shader_light.vs", "resources/shaders/shader_light.fs");
    //objModel = new Model(std::string("resources/media/cube2.obj")); // change objects here (Big Cube)
    //objModel = new Model(std::string("resources/media/Plain_Cube_Fractured.obj")); // change objects here (Prefractured Cube)
    objModel = new Model(std::string("resources/media/Teapot_Fractured.obj"));     // change objects here (Prefractured Teapot)
    quad = new Model(std::string("resources/media/wall.obj"));

    Shader* treeShader = new Shader("resources/shaders/shader_color.vs", "resources/shaders/shader_color.fs");
    Model* cube = new Model(std::string("resources/media/cube2.obj"));
    simulation->SetLight(mainShader);
    simulation->treeModel = cube;
    simulation->treeShader = treeShader;

    Demo1();
    //Demo2();
    //Demo3();
    //Demo4();
    //Demo5();
    //Demo6();
    //Demo7();
    //Demo8();


    simulation->SetBoundary();
    return simulation;
}

//Rigid body delcaration reference
//RigidBody(camera, shader, model, mesh, position, velocity, mass, torque, rotationAngle, angularVel, isStatic);

void Demo1()
{
    fireRay = 1;
    for (int i = 0; i < objModel->meshes.size(); ++i) {
        RigidBody* rb = new RigidBody(camera, mainShader, objModel, &objModel->meshes[i], glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.f, 0.0f, 0.f), 1, glm::vec3(0, 0, 0), 0.0f, 1.0f);
        simulation->objs.push_back(rb);
    }
}


void Demo2()
{
    RigidBody* rb = new RigidBody(camera, mainShader, objModel, nullptr, glm::vec3(0.0f, 12.0f, 0.0f), g, 1, glm::vec3(0, 1, 0), 0.0f, 1.0f);
    simulation->objs.push_back(rb);
    simulation->skipCheckStatic = true;
    //for (int i = 0; i < objModel->meshes.size(); ++i) {
    //    RigidBody* rb = new RigidBody(camera, *mainShader, *objModel, objModel->meshes[i], glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.f, -9.8f, 0.f), 1, glm::vec3(0, 1, 0), 0.0f, 1.0f);
    //    simulation->objs.push_back(rb);
    //}
}

void Demo3()
{
    RigidBody* rb = new RigidBody(camera, mainShader, objModel, nullptr, glm::vec3(0.0f, 12.0f, 0.0f), g, 1, glm::vec3(0, 1, 0), 0.0f, 1.0f);
    simulation->objs.push_back(rb);
    simulation->skipCheckStatic = true;
    triggerDemo3 = true;
}

void Demo4()
{
    RigidBody* rb = new RigidBody(camera, mainShader, objModel, nullptr, glm::vec3(0.0f, 12.0f, 0.0f), g, 1.0, glm::vec3(0, 1, 0), 0.0f, 1.0f);
    simulation->objs.push_back(rb);
    RigidBody* floor = new RigidBody(camera, mainShader, quad, nullptr, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1005, glm::vec3(1, 0, 0), 1.57f, 0.0f, true);
    simulation->objs.push_back(floor);
}

void Demo5()
{
    srand(time(0));
    mainShader->use();
    mainShader->setBool("isColorMode", true);
    simulation->isCheckMode = true;
    for (int i = 0; i < objModel->meshes.size(); ++i) {
        RigidBody* rb = new RigidBody(camera, mainShader, objModel, &objModel->meshes[i], glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.f, 0.0f, 0.f), 1, glm::vec3(0, 0, 0), 0.0f, 1.0f);
        rb->color = glm::vec3(rand() % 100 / (double)101, rand() % 100 / (double)101, rand() % 100 / (double)101);
        simulation->objs.push_back(rb);
    }
}

void Demo6()
{
    forceStrength = 10.0f;
    duration = 0.5f;
    DemoCollision();
}

void Demo7()
{
    mainShader->use();
    mainShader->setBool("isColorMode", true);
    forceStrength = 15.0f;
    duration = 1.0f;
    //simulation->isCheckMode = true;
    simulation->is_Demo7 = true;
    DemoCollision();
}

void Demo8()
{
    forceStrength = 15.0f;
    duration = 1.0f; 
    DemoCollision();
}


void DemoCollision()
{
    fireRay = 2;
    RigidBody* rb = new RigidBody(camera, mainShader, objModel, nullptr, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.f, 0.0f, 0.f), 1, glm::vec3(0, 0, 0), 0.0f, 1.0f);
    simulation->objs.push_back(rb);
    for (int i = 0; i < objModel->meshes.size(); ++i) {
        RigidBody* rb = new RigidBody(camera, mainShader, objModel, &objModel->meshes[i], glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.f, 0.0f, 0.f), 0.6, glm::vec3(0, 0, 0), 0.0f, 1.0f);
        rb->SetHide(true); // initialy hide pieces 
        simulation->objs.push_back(rb);
    }

    RigidBody* floor = new RigidBody(camera, mainShader, quad, nullptr, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1005, glm::vec3(1, 0, 0), 1.57f, 0.0f, true);
    simulation->objs.push_back(floor);
    RigidBody* ceil = new RigidBody(camera, mainShader, quad, nullptr, glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1005, glm::vec3(1, 0, 0), 1.57f, 0.0f, true);
    simulation->objs.push_back(ceil);
    RigidBody* left = new RigidBody(camera, mainShader, quad, nullptr, glm::vec3(-2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1005, glm::vec3(0, 1, 0), 1.57f, 0.0f, true);
    simulation->objs.push_back(left);
    RigidBody* right = new RigidBody(camera, mainShader, quad, nullptr, glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1005, glm::vec3(0, 1, 0), 1.57f, 0.0f, true);
    simulation->objs.push_back(right);
    RigidBody* inner = new RigidBody(camera, mainShader, quad, nullptr, glm::vec3(0.0f, 0.0f, -2.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1005, glm::vec3(0, 1, 0), 0.0f, 0.0f, true);
    simulation->objs.push_back(inner);
}