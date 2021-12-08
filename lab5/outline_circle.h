#ifndef OUTLINE_CIRCLE_H
#define OUTLINE_CIRCLE_H

#include <GL/glew.h>
#include "program.h"

class OutlineCircle {
public:
    void Initialize();

    void FindPixels(int r);

    void write_pixel(int x, int y, int offset);

    ~OutlineCircle();

    void Draw(const Program &program);

private:
    GLuint vao_;
    GLuint vertex_buffer_;
    GLuint color_buffer_;
};

#endif // OUTLINE_CIRCLE_H
