#include <GL/glew.h>

#include "outline_circle.h"

const int r = 800;
const int eighth_note = r; //powinno być *1.5 ale już przy kilkudziesięciu px zmierza do 0.7
int o_pointer = 1;
const int PIXELS = eighth_note * 8 + 1;

GLfloat o_kVertices[PIXELS * 4];
GLfloat o_kColors[PIXELS * 4];

void OutlineCircle::FindPixels(int r) {
    o_kVertices[0] = 0.0f;
    o_kVertices[1] = 0.0f;
    o_kVertices[2] = 0.0f;
    o_kVertices[3] = 0.0f;
    o_kColors[0] = 0.2f;
    o_kColors[1] = 0.2f;
    o_kColors[2] = 0.2f;
    o_kColors[3] = 0.2f;

    int i = 0;
    int j = r;
    int d = 5 - 4 * r;

    while (i < j) {
        if (d > 0) {
            j--;
            d += 8 * (i - j) + 20;
        } else d += 8 * i + 12;
        write_pixel(i, j, 0 * eighth_note + o_pointer);
        write_pixel(i, -j, 1 * eighth_note + o_pointer);
        write_pixel(-i, j, 2 * eighth_note + o_pointer);
        write_pixel(-i, -j, 3 * eighth_note + o_pointer);
        write_pixel(j, i, 4 * eighth_note + o_pointer);
        write_pixel(j, -i, 5 * eighth_note + o_pointer);
        write_pixel(-j, i, 6 * eighth_note + o_pointer);
        write_pixel(-j, -i, 7 * eighth_note + o_pointer);
        o_pointer++;
        i++;
    }
}

void OutlineCircle::write_pixel(int x, int y, int offset) {
    o_kVertices[offset * 4] = (float) x / 1000;
    o_kColors[offset * 4] = 0.2f;
    o_kVertices[offset * 4 + 1] = (float) y / 1000;
    o_kColors[offset * 4 + 1] = 0.2f;
    o_kVertices[offset * 4 + 2] = 0.0f;
    o_kColors[offset * 4 + 2] = 0.2f;
    o_kVertices[offset * 4 + 3] = 1.0f;
    o_kColors[offset * 4 + 3] = 1.0f;
}

void OutlineCircle::Initialize() {

    FindPixels(r);

	glGenVertexArrays(1, &vao_);
	glBindVertexArray(vao_);

	glGenBuffers(1, &vertex_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);

	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, sizeof(o_kVertices), o_kVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

	glGenBuffers(1, &color_buffer_);
	glBindBuffer(GL_ARRAY_BUFFER, color_buffer_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(o_kColors), o_kColors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

OutlineCircle::~OutlineCircle() {

	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &color_buffer_);
	glDeleteBuffers(1, &vertex_buffer_);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &vao_);
}

void OutlineCircle::Draw(const Program& program) {

	glUseProgram(program);
	glBindVertexArray(vao_);

	glLineWidth(2.0F);
	glDrawArrays(GL_LINE_STRIP, 0, PIXELS);

	glBindVertexArray(0);
	glUseProgram(0);
}
