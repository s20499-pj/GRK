#include <GL/glew.h>
#include <iostream>

#include "cube.h"
#include "vertices.h"

Cube::Cube(float init_velocity, float init_angle){
    velocity_ = init_velocity;
    angle_ = init_angle;
    animated_ = true;
}


void Cube::Move(float delta_t){
    if (!animated_) return;
    angle_ += delta_t * velocity_;
    if(angle_>360) angle_ -= 360;
    if(angle_<-360) angle_ += 360;
    model_matrix_.SetUnitMatrix();
    model_matrix_.RotateAboutX(angle_);
    model_matrix_.RotateAboutY(angle_);
}

void Cube::Left() {
    model_matrix_.RotateAboutY(-2);
}

void Cube::Right() {
    model_matrix_.RotateAboutY(2);
}

void Cube::Up() {
    model_matrix_.RotateAboutX(-2);
}

void Cube::Down() {
    model_matrix_.RotateAboutX(2);
}

void Cube::SpeedUp(){
    velocity_ *= 1.09544511501;
}

void Cube::SlowDown(){
    velocity_ /= 1.09544511501;
}

void Cube::ToggleAnimated(){
    animated_ = ! animated_;
}

void Cube::Initialize(){
    const ColorVertex kVertices[12] =
            {
                    {{-.5f, 0.f,  -.5f, 1.0f}, {1, 0, 0, 1}},
                    {{0.f,  .5f,  -.5f, 1.0f}, {0, 1, 0, 1}},
                    {{.5f,  0.f,  -.5f, 1.0f}, {0, 0, 1, 1}},
                    {{-.5f, 0.f,  0.f,  1.0f}, {0, 0, 1, 1}},
                    {{.5f,  0.f,  0.f,  1.0f}, {1, 0, 0, 1}},
                    {{-.5f, 0.f,  .5f,  1.0f}, {1, 0, 0, 1}},
                    {{0.f,  -.5f, .5f,  1.0f}, {0, 1, 0, 1}},
                    {{.5f,  0.f,  .5f,  1.0f}, {0, 0, 1, 1}},
                    {{-.5f, -.5f, .5f,  1.0f}, {0, 1, 0, 1}},
                    {{.5f,  -.5f, .5f,  1.0f}, {1, 0, 0, 1}},
                    {{.5f,  -.5f, -.5f, 1.0f}, {0, 1, 0, 1}},
                    {{-.5f, -.5f, -.5f, 1.0f}, {0, 0, 1, 1}}
            };

    const GLuint kIndices[60] =
            {
                    0, 1, 3,  //rzut z góry
                    1, 2, 4,  //rzut z góry
                    3, 1, 4,  //rzut z góry
                    3, 5, 6,  //rzut z góry
                    3, 6, 4,  //rzut z góry
                    4, 6, 7,  //rzut z góry
                    4, 7, 9, //rzut z prawej
                    4, 9, 10,//rzut z prawej
                    4, 2, 10, //rzut z prawej
                    3, 5, 8,  //rzut z lewej
                    3, 8, 11, //rzut z lewej
                    3, 0, 11, //rzut z lewej
                    6, 8, 11, //rzut z dołu
                    6, 10, 11,//rzut z dołu
                    6, 10, 9, //rzut z dołu
                    0, 1, 2,  //rzut z tyłu
                    0, 2, 10, //rzut z tyłu
                    0, 10, 11,//rzut z tyłu
                    5, 6, 8,  //rzut z przodu
                    7, 6, 9  //rzut z przodu*/
            };


    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(kVertices[0]), (GLvoid*) 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(kVertices[0]),
                          (GLvoid*) sizeof(kVertices[0].position));
    glEnableVertexAttribArray(1);


    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void Cube::Draw(const ModelProgram &program) const{

    glUseProgram(program);
    glBindVertexArray(vao_);

    program.SetModelMatrix(model_matrix_);

    glDrawElements(GL_TRIANGLES, 60, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glUseProgram(0);


}
