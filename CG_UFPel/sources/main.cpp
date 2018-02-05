// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <stack>


// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* g_pWindow;
unsigned int g_nWidth = 1280, g_nHeight = 800;
//unsigned int g_nWidth = 1024, g_nHeight = 768;

// Include AntTweakBar
#include <AntTweakBar.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/spline.hpp>

//rand
#include <stdlib.h>


using namespace glm;

using namespace std;


#include <shader.hpp>
#include <texture.hpp>
#include <controls.hpp>
#include <objloader.hpp>
#include <vboindexer.hpp>
#include <glerror.hpp>
#include <mesh.hpp>
#include <model.hpp>


void WindowSizeCallBack(GLFWwindow *pWindow, int nWidth, int nHeight) {

	g_nWidth = nWidth;
	g_nHeight = nHeight;
	glViewport(0, 0, g_nWidth, g_nHeight);
	TwWindowSize(g_nWidth, g_nHeight);
}

int main(void) {
	int nUseMouse = 0;
    
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	g_pWindow = glfwCreateWindow(g_nWidth, g_nHeight, "Space Game", NULL, NULL);
	if (g_pWindow == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(g_pWindow);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	check_gl_error();//OpenGL error from GLEW

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(g_nWidth, g_nHeight);

	// Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
	glfwSetMouseButtonCallback(g_pWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW); // - Directly redirect GLFW mouse button events to AntTweakBar
	glfwSetCursorPosCallback(g_pWindow, (GLFWcursorposfun)TwEventMousePosGLFW);          // - Directly redirect GLFW mouse position events to AntTweakBar
	glfwSetScrollCallback(g_pWindow, (GLFWscrollfun)TwEventMouseWheelGLFW);    // - Directly redirect GLFW mouse wheel events to AntTweakBar
	glfwSetKeyCallback(g_pWindow, (GLFWkeyfun)TwEventKeyGLFW);                         // - Directly redirect GLFW key events to AntTweakBar
	glfwSetCharCallback(g_pWindow, (GLFWcharfun)TwEventCharGLFW);                      // - Directly redirect GLFW char events to AntTweakBar
	glfwSetWindowSizeCallback(g_pWindow, WindowSizeCallBack);
    
    
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(g_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetCursorPos(g_pWindow, g_nWidth / 2, g_nHeight / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

    char* vertex = (char*) "../shaders/StandardShading.vertexshader";
    char* fragment = (char*) "../shaders/StandardShading.fragmentshader";
    char* textura = (char*) "../mesh/uvmap.DDS";
    
    char* suzane = (char*) "../mesh/suzanne.obj";
    char* planet = (char*) "../mesh/planeta.obj";
    
    Model ship(vertex, fragment, textura);;
    Model enemy(vertex, fragment, textura);;
    
    Mesh shipMesh;
    shipMesh.SetMesh(suzane);
    
    Mesh enemyMesh;
    enemyMesh.SetMesh(planet);
    
//	Load it into a VBO
    shipMesh.LoadVbo();
    enemyMesh.LoadVbo();
    
    ship.SetLight();
    enemy.SetLight();
    

//    For speed computation
	double lastTime = glfwGetTime();
    double frameTime = lastTime;
//    int nbFrames    = 0;
    
    int time = 10, i = 0;
    int nEnemy = 2;
    int nPositionEnemy = 10;

    
    float startEnemy = 30.0;
    float finishEnemy = 0.0;
    
//    float lefttShip = -15;
//    float rightShip = 15;
    
//    postion's enemys
    std::vector<float> positonEnemy;
    positonEnemy.resize(10);
    
    std::vector<int> positionIndex;
    std::vector<int> positionFlag;
    positionIndex.resize(nEnemy);
    positionFlag.resize(nPositionEnemy);
    
    for (i=0; i<nPositionEnemy; ++i) {
        positionFlag[i] = 0;
        if (i==0) {
            positonEnemy[i] = -14;
        } else {
            positonEnemy[i] = positonEnemy[i-1]+3;
        }
    }
    
    std::vector<float> movEnemy;
    movEnemy.resize(nEnemy);

    
    for (i=0; i<nEnemy; ++i) {
        positionIndex[i] = i;
        movEnemy[i] = startEnemy;
    }
    
    float movShip = 0.0;
    
	do{
        check_gl_error();

        
//        use the control key to free the mouse
        if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
            nUseMouse = 0;
        else
            nUseMouse = 1;
        
        double currentTime = glfwGetTime();
        float dT = 5*(currentTime-frameTime);
        frameTime = currentTime;
        
        double moveTime = currentTime-lastTime;
        
        if( moveTime >= time ){
            
            nEnemy++;
            if (nEnemy>10) {
                nEnemy=2;
                time--;
            }
            movEnemy.resize(nEnemy);
            positionIndex.resize(nEnemy);
            movEnemy[nEnemy] = startEnemy;
            positionIndex[nEnemy] = nEnemy-1;
            positionFlag[nEnemy] = 0;
            
            lastTime = glfwGetTime();
        }

        
//		Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        ship.ShaderModel();
        enemy.ShaderModel();
		

//		Compute the MVP matrix from keyboard and mouse input
        glm::mat4 MVP;
        glm::mat4 ProjectionMatrix;
        glm::mat4 ViewMatrix;
        
        computeMatricesFromInputs(nUseMouse, g_nWidth, g_nHeight);
        

//        O P e V do Pvm ficam na main agora;.
        ProjectionMatrix = getProjectionMatrix();
        ViewMatrix = getViewMatrix();

        
//        ##########################
//        Ship position
//        ##########################
        ship.SetPvm();
        
        ship.SetModelMatrix(scale(ship.GetModelMatrix(), vec3(0.6,0.6,0.6)));
        
        if (glfwGetKey(g_pWindow, GLFW_KEY_RIGHT) != GLFW_PRESS){
            movShip-=dT;
        }
        if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT) != GLFW_PRESS){
            movShip+=dT;
        }
        
        ship.SetModelMatrix(translate(ship.GetModelMatrix(), vec3(movShip,3.0,0.0)));
        
        
        MVP = ProjectionMatrix * ViewMatrix * ship.GetModelMatrix();
        
                
//        Send our transformation to the currently bound shader,
//        in the "MVP" uniform
        ship.SetDraw(MVP, ViewMatrix);
            
        ship.Light();
            
//        Bind our texture in Texture Unit 0
        ship.TextureM();
                
                
        shipMesh.SetBuffer(); //chamar apenas quando trocase o buffer (obj)
                
        ship.DrawModel(shipMesh.GetIndices());

        
//        ##########################
//        first enemy position
//        ##########################

        enemy.SetPvm();
        
        enemy.SetModelMatrix(scale(enemy.GetModelMatrix(), vec3(0.5,0.5,0.5)));
        
        if (movEnemy[0]>=30) {
            positionIndex[0]=rand()%10;
            positionFlag[positionIndex[0]]=1;

        }

        enemy.SetModelMatrix(translate(enemy.GetModelMatrix(), vec3(positonEnemy[positionIndex[0]],movEnemy[0],0.0)));
        MVP = ProjectionMatrix * ViewMatrix * enemy.GetModelMatrix();
        
        movEnemy[0] = (startEnemy-((moveTime*startEnemy)/time));
        
        if(movEnemy[0] <= finishEnemy){
            startEnemy = 30.00;
            movEnemy[0] = startEnemy;
            positionFlag[positionIndex[0]]=0;
        }
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        enemy.SetDraw(MVP, ViewMatrix);
        
        
        enemy.Light();
        
        // Bind our texture in Texture Unit 0
        enemy.TextureM();
        
        
        enemyMesh.SetBuffer(); //chamar apenas quando trocase o buffer (obj)
        
        enemy.DrawModel(enemyMesh.GetIndices());
        
//        Other enemys
        for(i=1; i<nEnemy; i++) {
            enemy.SetPvm();
            
            enemy.SetModelMatrix(scale(enemy.GetModelMatrix(), vec3(0.5,0.5,0.5)));
            
            if (movEnemy[i]>=30) {
                positionIndex[i]=rand()%10;
                
                while (positionFlag[positionIndex[i]]==1) {
                    positionIndex[i]=rand()%10;
                }
                
                positionFlag[positionIndex[i]]=1;
            }
            
            enemy.SetModelMatrix(translate(enemy.GetModelMatrix(), vec3(positonEnemy[positionIndex[i]],movEnemy[i],0.0)));
            MVP = ProjectionMatrix * ViewMatrix * enemy.GetModelMatrix();

            movEnemy[i] = (startEnemy-((moveTime*startEnemy)/time));
            
            if(movEnemy[i] <= finishEnemy){
                startEnemy = 30.00;
                movEnemy[i] = startEnemy;
                positionFlag[positionIndex[i]]=0;
            }
            
            
            
//            Send our transformation to the currently bound shader,
//            in the "MVP" uniform
            enemy.SetDraw(MVP, ViewMatrix);
            
            
            enemy.DrawModel(enemyMesh.GetIndices());
            
        }

        
        glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(g_pWindow);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(g_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(g_pWindow) == 0);

	// Cleanup VBO and shader
    //movido para mesh.CleanVbo
    shipMesh.CleanVbo();
    enemyMesh.CleanVbo();
        
    ship.CleanPtv();
    enemy.CleanPtv();
    
	// Terminate AntTweakBar and GLFW
	glfwTerminate();

	return 0;
}
