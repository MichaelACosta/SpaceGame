// Include GLFW
#include <glfw3.h>
extern GLFWwindow* g_pWindow; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
    return ProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 10 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 2.0f; // 2 units / second
float mouseSpeed = 0.05f;



void computeMatricesFromInputs(int nUseMouse, int nWidth, int nHeight){
    
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    static int nLastUseMouse = 1;
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    
    // Get mouse position
    double xpos = nWidth / 2, ypos = nHeight / 2;
    
    if (nUseMouse) {
        
        if (nLastUseMouse != nUseMouse)
            glfwSetCursorPos(g_pWindow, nWidth / 2, nHeight / 2);
        
        glfwGetCursorPos(g_pWindow, &xpos, &ypos);
        
        // Reset mouse position for next frame
        glfwSetCursorPos(g_pWindow, nWidth / 2, nHeight / 2);
    }
    
    nLastUseMouse = nUseMouse;
    
    
    // Compute new orientation
    horizontalAngle += mouseSpeed * float(nWidth / 2 - xpos);
    verticalAngle += mouseSpeed * float(nHeight / 2 - ypos);
    
    
    //rotaciona camera para direita
    if (glfwGetKey( g_pWindow, GLFW_KEY_D ) == GLFW_PRESS){
        horizontalAngle += speed * deltaTime * mouseSpeed;
        
    }
    //rotaciona camera para esquerda
    if (glfwGetKey( g_pWindow, GLFW_KEY_A ) == GLFW_PRESS){
        horizontalAngle -= speed * deltaTime * mouseSpeed;
        
    }
    //rotaciona camera para cima
    if (glfwGetKey( g_pWindow, GLFW_KEY_W ) == GLFW_PRESS){
        verticalAngle -= speed * deltaTime * mouseSpeed;
        
    }
    //rotaciona camera para baixo
    if (glfwGetKey( g_pWindow, GLFW_KEY_S ) == GLFW_PRESS){
        verticalAngle += speed * deltaTime * mouseSpeed;
        
    }
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
                        cos(verticalAngle) * sin(horizontalAngle),
                        sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle)
                        );
    
    // Right vector
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - 3.14f/2.0f),
                                0,
                                cos(horizontalAngle - 3.14f/2.0f)
                                );
    
    // Up vector
    glm::vec3 up = glm::cross( right, direction );
    
    // Move forward
    if (glfwGetKey( g_pWindow, GLFW_KEY_Z ) == GLFW_PRESS){
        position += direction * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( g_pWindow, GLFW_KEY_X ) == GLFW_PRESS){
        position -= direction * deltaTime * speed;
    }
    
    // Strafe right
    if (glfwGetKey( g_pWindow, GLFW_KEY_RIGHT ) == GLFW_PRESS){
        position += right * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( g_pWindow, GLFW_KEY_LEFT ) == GLFW_PRESS){
        position -= right * deltaTime * speed;
    }
    
    // Strafe Vertical up
    if (glfwGetKey( g_pWindow, GLFW_KEY_UP ) == GLFW_PRESS){
        position += up * deltaTime * speed;
    }
    // Strafe Vertical down
    if (glfwGetKey( g_pWindow, GLFW_KEY_DOWN ) == GLFW_PRESS){
        position -= up * deltaTime * speed;
    }
    
    //rotaciona camera para direita
    if (glfwGetKey( g_pWindow, GLFW_KEY_D ) == GLFW_PRESS){
        position += right * deltaTime * speed * (float).25;
        
        
    }
    //rotaciona camera para esquerda
    if (glfwGetKey( g_pWindow, GLFW_KEY_A ) == GLFW_PRESS){
        position -= right * deltaTime * speed * (float).25;
        
        
    }
    //rotaciona camera para cima
    if (glfwGetKey( g_pWindow, GLFW_KEY_W ) == GLFW_PRESS){
        position += up * deltaTime * speed * (float).25;
        
        
    }
    //rotaciona camera para baixo
    if (glfwGetKey( g_pWindow, GLFW_KEY_S ) == GLFW_PRESS){
        position -= up * deltaTime * speed * (float).25;
        
        
    }
    
    
    
    
    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    
    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
    // Camera matrix
    ViewMatrix       = glm::lookAt(
                                   position,           // Camera is here
                                   position+direction, // and looks here : at the same position, plus "direction"
                                   up                  // Head is up (set to 0,-1,0 to look upside-down)
                                   );
    
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
