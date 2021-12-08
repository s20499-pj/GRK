#include "circle.h"

#include <GL/glew.h>

const int r = 800;
const int eighth_note = r; //powinno być *1.5 ale już przy kilkudziesięciu px zmierza do 0.7
int pointer = 1;
const int PIXELS = eighth_note * 8 + 1;

GLfloat kVertices[PIXELS * 4];
GLfloat kColors[PIXELS * 4];

void Circle::FindPixels(int r) {
    kVertices[0] = 0.0f;
    kVertices[1] = 0.0f;
    kVertices[2] = 0.0f;
    kVertices[3] = 0.0f;
    kColors[0] = 0.2f;
    kColors[1] = 0.2f;
    kColors[2] = 0.2f;
    kColors[3] = 0.2f;

    int i = 0;
    int j = r;
    int d = 5 - 4 * r;

    while (i < j) {
        if (d > 0) {
            j--;
            d += 8 * (i - j) + 20;
        } else d += 8 * i + 12;
        write_pixel(i, j, 0 * eighth_note + pointer);
        write_pixel(i, -j, 1 * eighth_note + pointer);
        write_pixel(-i, j, 2 * eighth_note + pointer);
        write_pixel(-i, -j, 3 * eighth_note + pointer);
        write_pixel(j, i, 4 * eighth_note + pointer);
        write_pixel(j, -i, 5 * eighth_note + pointer);
        write_pixel(-j, i, 6 * eighth_note + pointer);
        write_pixel(-j, -i, 7 * eighth_note + pointer);
        pointer++;
        i++;
    }
}

void Circle::write_pixel(int x, int y, int offset) {
    kVertices[offset * 4] = (float) x / 1000;
    kColors[offset * 4] = 0.2f;
    kVertices[offset * 4 + 1] = (float) y / 1000;
    kColors[offset * 4 + 1] = 0.2f;
    kVertices[offset * 4 + 2] = 0.0f;
    kColors[offset * 4 + 2] = 0.2f;
    kVertices[offset * 4 + 3] = 1.0f;
    kColors[offset * 4 + 3] = 1.0f;
}

void Circle::Initialize() {

    FindPixels(r);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);

    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glGenBuffers(1, &color_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kColors), kColors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

Circle::~Circle() {

    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &color_buffer_);
    glDeleteBuffers(1, &vertex_buffer_);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao_);
}

void Circle::Draw(const Program &program) {

    glUseProgram(program);
    glBindVertexArray(vao_);

    glDrawArrays(GL_TRIANGLE_FAN, 0, PIXELS);

    glBindVertexArray(0);
    glUseProgram(0);
}
