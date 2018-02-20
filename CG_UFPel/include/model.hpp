#ifndef MODEL_HPP
#define MODEL_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <mesh.hpp>
#include <vboindexer.hpp>
#include <controls.hpp>
#include <shader.hpp>
#include <texture.hpp>

class Model{
public:
    
    Model();
    ~Model();
    
    Model(char* vertex, char* fragment, char* textura);
    
    void SetLight();
    
    void ShaderModel();
    
    void Light();
    
    void TextureM();
    
    void SetDraw(glm::mat4 MVP, glm::mat4 ViewMatrix);
    void DrawModel(std::vector<unsigned short> inde);
    
    void CleanPtv();
    
    void SetPvm();
    
    glm::mat4 GetModelMatrix();
    
    void SetModelMatrix(glm::mat4 MM);
    
private:
    
    glm::mat4 ModelMatrix;
    GLuint VertexArrayID;
    GLuint programID;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint Texture;
    GLuint TextureID;
    GLuint LightID;
    glm::vec3 lightPos;

};

#endif


