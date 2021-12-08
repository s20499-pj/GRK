#ifndef OUTLINE_START_H
#define OUTLINE_START_H

#include <GL/glew.h>
#include "program.h"

class OutlineStar {
public:
    void Initialize();

    ~OutlineStar();

    void Draw(const Program &program);

private:
    GLuint vao_;
    GLuint vertex_buffer_;
    GLuint color_buffer_;
};

#endif // OUTLINE_START_H
