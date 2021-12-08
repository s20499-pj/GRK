#ifndef CIRCLE_H
#define CIRCLE_H

#include <GL/glew.h>
#include "program.h"

class Circle {
public:
    void Initialize();

    void FindPixels(int r);

    void write_pixel(int x, int y, int offset);

    ~Circle();

    void Draw(const Program &program);

private:
    GLuint vao_;
    GLuint vertex_buffer_;
    GLuint color_buffer_;
};

#endif // CIRCLE_H
