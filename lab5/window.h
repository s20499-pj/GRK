#ifndef WINDOW_H
#define WINDOW_H

#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "triangle.h"
#include "star.h"
#include "outline_star.h"
#include "circle.h"
#include "outline_circle.h"
#include "program.h"

class Window{
 public:
    Window(const char*, int, int);
    void Initialize(int major_gl_version, int minor_gl_version);
    void Resize(int new_width, int new_height);
    void KeyEvent(int key, int scancode, int action, int mods);
    void Run(void);
    operator GLFWwindow*(){return window_;}
    int GetWidth();
    int GetHeight();
 private:
    int width_;
    int height_;
    const char* title_;
    GLFWwindow* window_;

    Triangle triangle_;
    Star star_;
    OutlineStar outline_star_;
    Circle circle_;
    OutlineCircle outline_circle_;

    Program program_;

    void InitGlfwOrDie(int major_gl_version,
                       int minor_gl_version);
    void InitGlewOrDie();
    void InitModels();
    void InitPrograms();
};


#endif // WINDOW_H

