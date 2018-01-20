#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "graphics.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>

using glm::mat4;

typedef GLuint VertexBuffer;
typedef GLuint ElementBuffer;
typedef GLuint Texture;

struct Framebuffer {
    int width, height;
    GLuint fbo;
    Texture colorBuffer;
    Texture depthAndStencilBuffer;
};

class Engine {
public:

    static void errorMessage(std::string message);
    static void dieIfOpenGLError();
    
    Engine();
    ~Engine();
    SDL_Window* createWindow(std::string title, int width, int height);
    void destroyWindow(SDL_Window*);
    bool shouldQuit();
    void handleInput();
    void waitForNextFrame(float secondsPerFrame);

    // input state
    bool isKeyDown(int scancode);
    bool isMouseButtonDown(int button);
    void mousePosition(int *x, int *y);
    int mouseX();
    int mouseY();

    // callback functions; child classes should override
    virtual void onKeyDown(SDL_KeyboardEvent&) {}
    virtual void onKeyUp(SDL_KeyboardEvent&) {}
    virtual void onMouseMotion(SDL_MouseMotionEvent&) {}
    virtual void onMouseButtonDown(SDL_MouseButtonEvent&) {}
    virtual void onMouseButtonUp(SDL_MouseButtonEvent&) {}

    // vertex and element buffers
    VertexBuffer allocateVertexBuffer(int bytes);
    void copyVertexData(VertexBuffer buffer, void *data, int bytes);
    void setVertexArray(VertexBuffer buffer);
    void setColorArray(VertexBuffer buffer);
    void setNormalArray(VertexBuffer buffer);
    void setTexCoordArray(VertexBuffer buffer);
    void unsetVertexArray();
    void unsetColorArray();
    void unsetNormalArray();
    void unsetTexCoordArray();
    ElementBuffer allocateElementBuffer(int bytes);
    void copyElementData(ElementBuffer buffer, void *data, int bytes);
    void drawElements(GLenum mode, ElementBuffer buffer, int count);
    // convenience functions
    template <typename T> VertexBuffer allocateVertexBuffer(std::vector<T> &data);
    template <typename T> ElementBuffer allocateElementBuffer(std::vector<T> &data);

    // textures
    Texture loadTexture(std::string bmpFile);
    Texture createTexture(int width, int height);
    Texture createDepthAndStencilTexture(int width, int height);
    void setTexture(Texture texture);
    void unsetTexture();

    // transformation matrices
    void matrixMode(GLenum mode);
    mat4 getMatrix(GLenum mode=GL_NONE);
    void setMatrix(mat4 m, GLenum mode=GL_NONE);
    void pushMatrix(GLenum mode=GL_NONE);
    void popMatrix(GLenum mode=GL_NONE);

    // framebuffers
    Framebuffer createFramebuffer(int width, int height);
    void setFramebuffer(Framebuffer framebuffer);
    void unsetFramebuffer(SDL_Window *window);

protected:
    bool userQuit;
    int lastFrameTime;
    GLenum matMode;
    std::vector<mat4> modelViewStack;
    std::vector<mat4> projectionStack;
    void dieWithSDLError(std::string message);
    std::vector<mat4>& stack(GLenum mode=GL_NONE);

};

// Definitions below

inline Engine::Engine() {
    int status = SDL_Init(SDL_INIT_VIDEO);
    if (status < 0)
        dieWithSDLError("Failed to initialize SDL");
    userQuit = false;
    lastFrameTime = 0;
    modelViewStack.push_back(mat4());
    projectionStack.push_back(mat4());
}

inline Engine::~Engine() {
    SDL_Quit();
}

inline void Engine::dieWithSDLError(std::string message) {
    errorMessage(message + ": " + SDL_GetError());
    exit(EXIT_FAILURE);
}

inline SDL_Window* Engine::createWindow(std::string title, int width, int height) {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_Window *window =
        SDL_CreateWindow(title.c_str(),
                         SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                         width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL)
        dieWithSDLError("Failed to create window");
    SDL_GLContext context = SDL_GL_CreateContext(window);
    if (context == NULL)
        dieWithSDLError("Failed to create OpenGL context");
    SDL_GL_SetSwapInterval(1);
#ifndef __APPLE__
    glewExperimental = GL_TRUE;
    glewInit();
    glGetError();
#endif
    glEnable(GL_DEPTH_TEST);
    dieIfOpenGLError();
    return window;
}

inline void Engine::destroyWindow(SDL_Window *window) {
    SDL_DestroyWindow(window);
}

inline void Engine::errorMessage(std::string message){
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), NULL);
}

inline void Engine::dieIfOpenGLError() {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        errorMessage(std::string("OpenGL error: ") + (char*)(gluErrorString(error)));
        abort();
        exit(EXIT_FAILURE);
    }
}

inline void Engine::handleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            userQuit = true;
            break;
        case SDL_KEYDOWN:
            onKeyDown(event.key);
            break;
        case SDL_KEYUP:
            onKeyUp(event.key);
            break;
        case SDL_MOUSEMOTION:
            onMouseMotion(event.motion);
            break;
        case SDL_MOUSEBUTTONDOWN:
            onMouseButtonDown(event.button);
            break;
        case SDL_MOUSEBUTTONUP:
            onMouseButtonUp(event.button);
            break;
        }
    }
}

inline bool Engine::shouldQuit() {
    return userQuit;
}

inline void Engine::waitForNextFrame(float secondsPerFrame) {
    int millisPerFrame = (int)(1000*secondsPerFrame);
    int now = SDL_GetTicks();
    int nextFrameTime = lastFrameTime + millisPerFrame;
    lastFrameTime = now;
    if (nextFrameTime > now)
        SDL_Delay(nextFrameTime - now);
}

inline bool Engine::isKeyDown(int scancode) {
    return SDL_GetKeyboardState(NULL)[scancode];
}

inline bool Engine::isMouseButtonDown(int button) {
    return SDL_GetMouseState(NULL,NULL) & SDL_BUTTON(button);
}

inline void Engine::mousePosition(int *x, int *y) {
    SDL_GetMouseState(x,y);
}

inline int Engine::mouseX() {
    int x;
    SDL_GetMouseState(&x,NULL);
    return x;
};

inline int Engine::mouseY() {
    int y;
    SDL_GetMouseState(NULL,&y);
    return y;
}

inline VertexBuffer Engine::allocateVertexBuffer(int size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
    dieIfOpenGLError();
    return buffer;
}

inline ElementBuffer Engine::allocateElementBuffer(int size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
    dieIfOpenGLError();
    return buffer;
}

inline void Engine::copyVertexData(VertexBuffer buffer, void *data, int size) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    dieIfOpenGLError();
}

inline void Engine::copyElementData(ElementBuffer buffer, void *data, int size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, data);
    dieIfOpenGLError();
}

inline void Engine::setVertexArray(VertexBuffer buffer) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexPointer(3, GL_FLOAT, 0, 0);
    dieIfOpenGLError();
}

inline void Engine::setColorArray(VertexBuffer buffer) {
    glEnableClientState(GL_COLOR_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glColorPointer(3, GL_FLOAT, 0, 0);
    dieIfOpenGLError();
}

inline void Engine::setNormalArray(VertexBuffer buffer) {
    glEnableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glNormalPointer(GL_FLOAT, 0, 0);
    dieIfOpenGLError();
}

inline void Engine::setTexCoordArray(VertexBuffer buffer) {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);
    dieIfOpenGLError();
}

inline void Engine::unsetVertexArray() {
    glDisableClientState(GL_VERTEX_ARRAY);
    dieIfOpenGLError();
}

inline void Engine::unsetColorArray() {
    glDisableClientState(GL_COLOR_ARRAY);
    dieIfOpenGLError();
}

inline void Engine::unsetNormalArray() {
    glDisableClientState(GL_NORMAL_ARRAY);
    dieIfOpenGLError();
}

inline void Engine::unsetTexCoordArray() {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    dieIfOpenGLError();
}

inline void Engine::drawElements(GLenum mode, ElementBuffer buffer, int count) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
    dieIfOpenGLError();
}

template <typename T>
inline VertexBuffer Engine::allocateVertexBuffer(std::vector<T> &data) {
    int size = data.size()*sizeof(T);
    VertexBuffer buffer = allocateVertexBuffer(size);
    copyVertexData(buffer, &data[0], size);
    return buffer;
}

template <typename T>
inline ElementBuffer Engine::allocateElementBuffer(std::vector<T> &data) {
    int size = data.size()*sizeof(T);
    ElementBuffer buffer = allocateElementBuffer(size);
    copyElementData(buffer, &data[0], size);
    return buffer;
}

inline Texture Engine::loadTexture(std::string bmpFile) {
    SDL_Surface *surface = SDL_LoadBMP(bmpFile.c_str());
    if (surface == NULL)
        dieWithSDLError("Failed to load image");
    SDL_LockSurface(surface);
    char *pixelData = (char*)surface->pixels;
    for (int row = 0; row < surface->h/2; row++) {
        int row2 = surface->h - 1 - row;
        for (int byte = 0; byte < surface->pitch; byte++) {
            std::swap(pixelData[row*surface->pitch + byte],
                      pixelData[row2*surface->pitch + byte]);
        }
    }
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_BGR,
                 GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    SDL_FreeSurface(surface);
    dieIfOpenGLError();
    return texture;
}

inline Texture Engine::createTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    dieIfOpenGLError();
    return texture;
}

inline Texture Engine::createDepthAndStencilTexture(int width, int height) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, width, height, 0,
                 GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    dieIfOpenGLError();
    return texture;
}

inline void Engine::setTexture(Texture texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    dieIfOpenGLError();
}

inline void Engine::unsetTexture() {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    dieIfOpenGLError();
}

inline void Engine::matrixMode(GLenum mode) {
    if (mode != GL_MODELVIEW && mode != GL_PROJECTION) {
        errorMessage("matrixMode must be GL_MODELVIEW or GL_PROJECTION");
        exit(EXIT_FAILURE);
    }
    matMode = mode;
}

inline std::vector<mat4>& Engine::stack(GLenum mode) {
    if (mode == GL_NONE)
        mode = matMode;
    return (mode == GL_MODELVIEW) ? modelViewStack : projectionStack;
}

inline mat4 Engine::getMatrix(GLenum mode) {
    return stack(mode).back();
}

inline void Engine::setMatrix(mat4 m, GLenum mode) {
    stack(mode).back() = m;
}

inline void Engine::pushMatrix(GLenum mode) {
    stack(mode).push_back(stack(mode).back());
}

inline void Engine::popMatrix(GLenum mode) {
    if (stack(mode).size() == 1) {
        errorMessage("Can't pop the last matrix off the stack");
        exit(EXIT_FAILURE);
    }
    stack(mode).pop_back();
}

inline Framebuffer Engine::createFramebuffer(int width, int height) {
    Framebuffer f;
    f.width = width;
    f.height = height;
    glGenFramebuffers(1, &f.fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, f.fbo);
    dieIfOpenGLError();
    f.colorBuffer = createTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           f.colorBuffer, 0);
    f.depthAndStencilBuffer = createDepthAndStencilTexture(width, height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                           GL_TEXTURE_2D, f.depthAndStencilBuffer, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        errorMessage("Framebuffer is not complete!");
        abort();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    dieIfOpenGLError();
    return f;
}

inline void Engine::setFramebuffer(Framebuffer framebuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
    glViewport(0, 0, framebuffer.width, framebuffer.height);
}

inline void Engine::unsetFramebuffer(SDL_Window *window) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int width, height;
    SDL_GL_GetDrawableSize(window, &width, &height);
    glViewport(0, 0, width, height);
}

#endif
