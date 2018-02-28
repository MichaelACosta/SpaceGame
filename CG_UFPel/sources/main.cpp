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

// Include AntTweakBar
#include <AntTweakBar.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/spline.hpp>

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
#include <text2D.hpp>


void WindowSizeCallBack(GLFWwindow *pWindow, int nWidth, int nHeight) {
    
    g_nWidth = nWidth;
    g_nHeight = nHeight;
    glViewport(0, 0, g_nWidth, g_nHeight);
    TwWindowSize(g_nWidth, g_nHeight);
}

int main(void) {
    int nUseMouse = 0;
    
//    Initialise GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    g_pWindow = glfwCreateWindow(g_nWidth, g_nHeight, "Space Game", NULL, NULL);
    if (g_pWindow == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(g_pWindow);
    
//    Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }
    
    check_gl_error();
    
//    Initialize the GUI
    TwInit(TW_OPENGL_CORE, NULL);
    TwWindowSize(g_nWidth, g_nHeight);
    
//    Set GLFW event callbacks. I removed glfwSetWindowSizeCallback for conciseness
    glfwSetMouseButtonCallback(g_pWindow, (GLFWmousebuttonfun)TwEventMouseButtonGLFW);
    glfwSetCursorPosCallback(g_pWindow, (GLFWcursorposfun)TwEventMousePosGLFW);
    glfwSetScrollCallback(g_pWindow, (GLFWscrollfun)TwEventMouseWheelGLFW);
    glfwSetKeyCallback(g_pWindow, (GLFWkeyfun)TwEventKeyGLFW);
    glfwSetCharCallback(g_pWindow, (GLFWcharfun)TwEventCharGLFW);
    glfwSetWindowSizeCallback(g_pWindow, WindowSizeCallBack);
    
    
//    Ensure we can capture the escape key being pressed below
    glfwSetInputMode(g_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetCursorPos(g_pWindow, g_nWidth / 2, g_nHeight / 2);
    
//    Dark blue background
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
//    Enable depth test
    glEnable(GL_DEPTH_TEST);
//    Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
//    Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    
//    ****************************
//    define variables and objects
//    ****************************
    char* vertex = (char*) "../shaders/toonShader.vertexshader";
    char* fragment = (char*) "../shaders/toonShader.fragmentshader";
    
    char* tiro = (char*) "../mesh/tiro.bmp";
    char* pelo = (char*) "../mesh/pelo.bmp";
    char* monolito = (char*) "../mesh/monolito.bmp";
    
    char* suzane = (char*) "../mesh/suzanne.obj";
    char* cube = (char*) "../mesh/cube.obj";
    
    Model ship(vertex, fragment, monolito);
    Model enemy(vertex, fragment, pelo);
    Model guns(vertex, fragment, tiro);
    
    Mesh shipMesh;
    shipMesh.SetMesh(cube);
    
    Mesh enemyMesh;
    enemyMesh.SetMesh(suzane);
    
    Mesh gunsMesh;
    gunsMesh.SetMesh(cube);
    
    shipMesh.LoadVbo();
    enemyMesh.LoadVbo();
    gunsMesh.LoadVbo();
    
    ship.SetLight();
    enemy.SetLight();
    guns.SetLight();
    
    
//    For speed computation
    double lastTime = glfwGetTime();
    double frameTime = lastTime;
    
    
//    ******************
//    Start Variables
//    ******************
    int i = 0;
    int time = 10;
    int level = 10;
    int nEnemy = 2;
    int nPositionEnemy = 8;
    int nGuns = 1000;
    int contGuns = 0;
    int points = 0;
    int pointsTmp = 0;
    int pointsEnemy = 0;
    int flagShip = 10;
    int live = 3;
    
    float movLeftShip = -12.0;
    float movRightShip = 12.0;
    float startEnemy = 30.0;
    float finishEnemy = 0.0;
    float movShip = 0.0;
    
//    Start Model Matrix Enemys
    std::vector<glm::mat4> ModelMatrixEnemys;
    ModelMatrixEnemys.resize(nEnemy);
    
//    conflict flag enemys
    std::vector<int> flagConflict;
    flagConflict.resize(nEnemy);
    
//    postion index enemy's control
    std::vector<int> positionIndex;
    positionIndex.resize(nEnemy);
    
//    postion flag enemy's control
    std::vector<int> positionFlagEnemy;
    positionFlagEnemy.resize(nPositionEnemy);
    
//    postion's enemys
    std::vector<float> positonEnemy;
    positonEnemy.resize(10);
    
//    positon's guns
    std::vector<float> positonGuns;
    positonGuns.resize(nGuns);
    
//    move guns
    std::vector<float> moveGuns;
    moveGuns.resize(nGuns);
    
//    move enemy
    std::vector<float> movEnemy;
    movEnemy.resize(nEnemy);
    
//    set the enemy positons
    for (i=0; i<nPositionEnemy; ++i) {
        positionFlagEnemy[i] = 0;
        if (i==0) {
            positonEnemy[i] = -11;
        } else {
            positonEnemy[i] = positonEnemy[i-1]+3;
        }
    }
    
//    set enemy move position
    for (i=0; i<nEnemy; ++i) {
        positionIndex[i] = i;
        movEnemy[i] = startEnemy;
        flagConflict[i]=0;
    }
    
    initText2D( "../shaders/Holstein.DDS" );
    
    do{
        check_gl_error();
        
        if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS)
            nUseMouse = 0;
        else
            nUseMouse = 1;
        
        if (glfwGetKey(g_pWindow, GLFW_KEY_ENTER) == GLFW_PRESS){
            flagShip = 3;
            lastTime = glfwGetTime();
        }

        
        if(flagShip!=3){
            if ((time-1)<=1 && nEnemy==nPositionEnemy){
                if (pointsEnemy>=points) {
                    flagShip=1;
                } else {
                    flagShip=2;
                }
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (flagShip==0) {
            
            double currentTime = glfwGetTime();
            float dT = 5*(currentTime-frameTime);
            frameTime = currentTime;

//          ********************************
//          control of move enemys with time
//          ********************************
            double moveTime = currentTime-lastTime;
            if( moveTime >= time ){
                
                pointsEnemy += nEnemy-pointsTmp;
                pointsTmp = 0;
                nEnemy++;
                if (nEnemy>nPositionEnemy) {
                    nEnemy=2;
                    time--;
                }
                ModelMatrixEnemys.resize(nEnemy);
                movEnemy.resize(nEnemy);
                positionIndex.resize(nEnemy);
                flagConflict.resize(nEnemy);
                for (i=0; i<nEnemy; ++i) {
                    movEnemy[i] = startEnemy;
                    positionIndex[i] = i;
                    positionFlagEnemy[i] = 0;
                    flagConflict[i]=0;
                }
                
                lastTime = glfwGetTime();
                
            }
            
            
            ship.ShaderModel();
            enemy.ShaderModel();
            guns.ShaderModel();
            
            
            glm::mat4 MVP;
            glm::mat4 ProjectionMatrix;
            glm::mat4 ViewMatrix;
            
            computeMatricesFromInputs(nUseMouse, g_nWidth, g_nHeight);
            
            
            ProjectionMatrix = getProjectionMatrix();
            ViewMatrix = getViewMatrix();
            
//            ##############
//            Movemente
//            ##############
            
            if (glfwGetKey(g_pWindow, GLFW_KEY_RIGHT) == GLFW_PRESS){
                if (movShip <= movRightShip) {
                    movShip+=dT;
                }
            }
            if (glfwGetKey(g_pWindow, GLFW_KEY_LEFT) == GLFW_PRESS){
                if (movShip >= movLeftShip) {
                    movShip-=dT;
                }
            }
            if (glfwGetKey(g_pWindow, GLFW_KEY_SPACE) == GLFW_PRESS){
                if (glfwGetKey(g_pWindow, GLFW_KEY_SPACE) == GLFW_RELEASE){
                    if (contGuns<=nGuns) {
                        contGuns++;
                        positonGuns[contGuns] = movShip;
                        moveGuns[contGuns] = 4.5;
                    }else if (moveGuns[contGuns]>=22) {
                        contGuns = 0;
                        positonGuns[contGuns] = movShip;
                        moveGuns[contGuns] = 4.5;
                    }
                }
            }
            
            
//            ##########################
//            Ship position
//            ##########################
            ship.SetPvm();
            
            ship.SetModelMatrix(scale(ship.GetModelMatrix(), vec3(0.6,0.6,0.1)));
            
            
            ship.SetModelMatrix(translate(ship.GetModelMatrix(), vec3(movShip,3.0,0.0)));
            ship.SetModelMatrix(rotate(ship.GetModelMatrix(), (float) 150, vec3(1.0,0.0,0.0)));
            
            ship.SetModelMatrix(scale(ship.GetModelMatrix(), vec3(0.7,1,0.2)));
            MVP = ProjectionMatrix * ViewMatrix * ship.GetModelMatrix();
            
            ship.Light();
            
            ship.TextureM();
            
            
            shipMesh.SetBuffer();
            
            if (flagShip==0) {
                ship.SetDraw(MVP, ViewMatrix);
                ship.DrawModel(shipMesh.GetIndices());
            }
            
//          ##########################
//          first enemy position
//          ##########################
            
            enemy.SetPvm();
            
            enemy.SetModelMatrix(scale(enemy.GetModelMatrix(), vec3(0.5,0.5,0.5)));
            
            if (movEnemy[0]>=30) {
                positionIndex[0]=rand()%nPositionEnemy;
                positionFlagEnemy[positionIndex[0]]=1;
            }
            
            enemy.SetModelMatrix(translate(enemy.GetModelMatrix(), vec3(positonEnemy[positionIndex[0]],movEnemy[0],0.0)));
            ModelMatrixEnemys[0] = enemy.GetModelMatrix();
            MVP = ProjectionMatrix * ViewMatrix * ModelMatrixEnemys[0];
            
            movEnemy[0] = (startEnemy-((moveTime*startEnemy)/time));
            
            if(movEnemy[0] <= finishEnemy){
                startEnemy = 30.00;
                movEnemy[0] = startEnemy;
                positionFlagEnemy[positionIndex[0]]=0;
            }
            
            enemy.Light();
            
            enemy.TextureM();
            
            
            enemyMesh.SetBuffer();
            
            
            if (flagConflict[0]==0) {
                
                if ( (((ship.GetModelMatrix()[3][0]-0.6) <= (ModelMatrixEnemys[0][3][0]+0.6)) && ((ship.GetModelMatrix()[3][0]+0.6) >= (ModelMatrixEnemys[0][3][0]-0.6))) && (((ship.GetModelMatrix()[3][1]-0.6) <= (ModelMatrixEnemys[0][3][1]+0.6)) && ((ship.GetModelMatrix()[3][1]+0.6) >= (ModelMatrixEnemys[0][3][1]-0.6))) ){
                    
                    flagConflict[0] = 1;
                    live--;
                    if (live==0) {
                        flagShip = 1;
                    }
                }
            }
            
            if (flagConflict[0]==0) {
                enemy.SetDraw(MVP, ViewMatrix);
                enemy.DrawModel(enemyMesh.GetIndices());
            } else {
                movEnemy[0] = finishEnemy-1;
            }
            
//        ##########################
//        Other enemys
//        ##########################
            for(i=1; i<nEnemy; i++) {
                enemy.SetPvm();
                
                enemy.SetModelMatrix(scale(enemy.GetModelMatrix(), vec3(0.5,0.5,0.5)));
                
                if (movEnemy[i]>=30) {
                    positionIndex[i]=rand()%nPositionEnemy;
                    
                    while (positionFlagEnemy[positionIndex[i]]==1) {
                        positionIndex[i]=rand()%nPositionEnemy;
                    }
                    
                    positionFlagEnemy[positionIndex[i]]=1;
                }
                
                enemy.SetModelMatrix(translate(enemy.GetModelMatrix(), vec3(positonEnemy[positionIndex[i]],movEnemy[i],0.0)));
                ModelMatrixEnemys[i] = enemy.GetModelMatrix();
                MVP = ProjectionMatrix * ViewMatrix * ModelMatrixEnemys[i];
                
                movEnemy[i] = (startEnemy-((moveTime*startEnemy)/time));
                
                if(movEnemy[i] <= finishEnemy){
                    startEnemy = 30.00;
                    movEnemy[i] = startEnemy;
                    positionFlagEnemy[positionIndex[i]]=0;
                }
                
                
                if (flagConflict[i]==0) {
                    
                    if ( (((ship.GetModelMatrix()[3][0]-0.6) <= (ModelMatrixEnemys[i][3][0]+0.6)) && ((ship.GetModelMatrix()[3][0]+0.6) >= (ModelMatrixEnemys[i][3][0]-0.6))) && (((ship.GetModelMatrix()[3][1]-0.6) <= (ModelMatrixEnemys[i][3][1]+0.6)) && ((ship.GetModelMatrix()[3][1]+0.6) >= (ModelMatrixEnemys[i][3][1]-0.6))) ){
                        flagConflict[i] = 1;
                        live--;
                        if (live==0) {
                            flagShip = 1;
                        }
                    }
                }
                
                if (flagConflict[i]==0) {
                    enemy.SetDraw(MVP, ViewMatrix);
                    enemy.DrawModel(enemyMesh.GetIndices());
                } else {
                    movEnemy[i] = finishEnemy-1;
                }
                
            }
            
//        ##########################
//        Guns position
//        ##########################
            
            guns.SetPvm();
            
            guns.SetModelMatrix(scale(guns.GetModelMatrix(), vec3(0.6,0.6,0.6)));
            
            if(contGuns>=1) {
                
                moveGuns[1]+=dT;
                
                guns.SetModelMatrix(translate(guns.GetModelMatrix(), vec3(positonGuns[1],moveGuns[1],0.0)));
                
                guns.SetModelMatrix(scale(guns.GetModelMatrix(), vec3(0.08,0.3,0.01)));
                
                if(moveGuns[1]<=28){
                    for (int y=0; y<nEnemy; ++y) {
                        
                        if ( (((guns.GetModelMatrix()[3][0]-0.3) <= (ModelMatrixEnemys[y][3][0]+0.3)) && ((guns.GetModelMatrix()[3][0]+0.3) >= (ModelMatrixEnemys[y][3][0]-0.3))) && (((guns.GetModelMatrix()[3][1]-0.3) <= (ModelMatrixEnemys[y][3][1]+0.3)) && ((guns.GetModelMatrix()[3][1]+0.3) >= (ModelMatrixEnemys[y][3][1]-0.3))) ){
                            flagConflict[y]=1;
                            moveGuns[1] = 30;
                            points++;
                            pointsTmp++;
                        }
                    }
                    
                    MVP = ProjectionMatrix * ViewMatrix * guns.GetModelMatrix();
                }
                
                guns.Light();
                
                guns.TextureM();
                
                gunsMesh.SetBuffer();
                
                if(moveGuns[1]<=28){
                    guns.SetDraw(MVP, ViewMatrix);
                    guns.DrawModel(gunsMesh.GetIndices());
                }
                
                for (int i=2; i<=contGuns; ++i) {
                    guns.SetPvm();
                    
                    guns.SetModelMatrix(scale(guns.GetModelMatrix(), vec3(0.6,0.6,0.6)));
                    
                    moveGuns[i]+=dT;
                    
                    guns.SetModelMatrix(translate(guns.GetModelMatrix(), vec3(positonGuns[i],moveGuns[i],0.0)));
                    
                    guns.SetModelMatrix(scale(guns.GetModelMatrix(), vec3(0.08,0.3,0.01)));
                    
                    
                    if(moveGuns[i]<=28){
                        
                        for (int y=0; y<nEnemy; ++y) {
                            
                            if ( (((guns.GetModelMatrix()[3][0]-0.3) <= (ModelMatrixEnemys[y][3][0]+0.3)) && ((guns.GetModelMatrix()[3][0]+0.3) >= (ModelMatrixEnemys[y][3][0]-0.3))) && (((guns.GetModelMatrix()[3][1]-0.3) <= (ModelMatrixEnemys[y][3][1]+0.3)) && ((guns.GetModelMatrix()[3][1]+0.3) >= (ModelMatrixEnemys[y][3][1]-0.3))) ){
                                flagConflict[y]=1;
                                moveGuns[i] = 30;
                                points++;
                                pointsTmp++;
                                
                            }
                        }
                        
                        MVP = ProjectionMatrix * ViewMatrix * guns.GetModelMatrix();
                        
                        guns.SetDraw(MVP, ViewMatrix);
                        guns.DrawModel(gunsMesh.GetIndices());
                    }
                    
                }
                
            }
            
            char text[25];
            sprintf(text,"Level: %d", level-time+1 );
            printText2D(text, 50, 570, 15);
            
            sprintf(text,"Points: %d", points );
            printText2D(text, 230, 570, 15);
            
            sprintf(text,"Enemys: %d", pointsEnemy );
            printText2D(text, 430, 570, 15);
            
            sprintf(text,"Lives: %d", live );
            printText2D(text, 620, 570, 15);
            
        } else if(flagShip==10) {
            char text[50];
            sprintf(text,"Move Ship:" );
            printText2D(text, 160, 400, 18);

            sprintf(text,"Key Left and Key Right" );
            printText2D(text, 100, 370, 14);
            
            sprintf(text,"Shot:" );
            printText2D(text, 510, 400, 18);
            
            sprintf(text,"Key Space" );
            printText2D(text, 490, 370, 14);
            
            sprintf(text,"Start Game:" );
            printText2D(text, 300, 200, 18);
            
            sprintf(text,"Press Enter" );
            printText2D(text, 320, 170, 14);
        } else if(flagShip==1){
            char text[25];
            sprintf(text,"Game Over!" );
            printText2D(text, 175, 300, 50);
            
            sprintf(text,"Play Again:" );
            printText2D(text, 300, 200, 18);
            
            sprintf(text,"Press Enter" );
            printText2D(text, 320, 170, 14);
        } else if (flagShip==3){
            contGuns=0;
            live=3;
            points=0;
            pointsEnemy = 0;
            pointsTmp = 0;
            nEnemy = 2;
            ModelMatrixEnemys.resize(nEnemy);
            movEnemy.resize(nEnemy);
            positionIndex.resize(nEnemy);
            flagConflict.resize(nEnemy);
            for (i=0; i<nEnemy; ++i) {
                movEnemy[i] = startEnemy;
                positionIndex[i] = i;
                positionFlagEnemy[i] = 0;
                flagConflict[i]=0;
            }
            lastTime = glfwGetTime();
            time = 10;
            flagShip=0;
        } else {
            char text[25];
            sprintf(text,"You Win!" );
            printText2D(text, 200, 300, 50);
            
            sprintf(text,"Play Again:" );
            printText2D(text, 300, 200, 18);
            
            sprintf(text,"Press Enter" );
            printText2D(text, 320, 170, 14);
        }
        
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        
        glfwSwapBuffers(g_pWindow);
        glfwPollEvents();
        
    }
    while (glfwGetKey(g_pWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(g_pWindow) == 0);
    
    shipMesh.CleanVbo();
    enemyMesh.CleanVbo();
    gunsMesh.CleanVbo();
    
    ship.CleanPtv();
    enemy.CleanPtv();
    guns.CleanPtv();
    
    cleanupText2D();
    
    glfwTerminate();
    
    return 0;
}
