#include "model.hpp"

Model::Model(){}

Model::Model(char* vertex, char* fragment, char* textura){
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    programID = LoadShaders(vertex, fragment);
    MatrixID      = glGetUniformLocation(programID, "MVP");
    ViewMatrixID  = glGetUniformLocation(programID, "V");
    ModelMatrixID = glGetUniformLocation(programID, "M");
    Texture = loadBMP_custom(textura);
    TextureID = glGetUniformLocation(programID, "myTextureSampler");
}

Model::~Model(){}

void Model::SetLight(){
    glUseProgram(programID);
    LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
}

void Model::ShaderModel(){
    glUseProgram(programID);
}

void Model::SetDraw(glm::mat4 MVP, glm::mat4 ViewMatrix ){
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
}

void Model::DrawModel(std::vector<unsigned short> inde){
    glDrawElements(
                   GL_TRIANGLES,
                   inde.size(),
                   GL_UNSIGNED_SHORT,
                   (void*)0
                   );
}

void Model::Light(){
    lightPos = glm::vec3(-2, 7, 6);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
}

void Model::TextureM(){
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glUniform1i(TextureID, 0);
}

void Model::CleanPtv(){
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture);
    glDeleteVertexArrays(1, &VertexArrayID);
}

void Model::SetPvm(){
    ModelMatrix      = glm::mat4(1.0);
}

glm::mat4 Model::GetModelMatrix(){
    return ModelMatrix;
}

void Model::SetModelMatrix(glm::mat4 MM){
    ModelMatrix = MM;
}
