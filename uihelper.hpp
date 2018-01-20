#ifndef UIHELPER_HPP
#define UIHELPER_HPP

#include "engine.hpp"
#include <vector>
#include <glm/glm.hpp>
using glm::vec2;

class UIMain { // Method that UIHelper expects the main program to have
public:
    virtual void addCircle() = 0;
    virtual void addBox() = 0;
    virtual void addPolyline(std::vector<vec2> vertices) = 0;
    virtual void clear() = 0;
    virtual void attachMouse(vec2 point) = 0;
    virtual void moveMouse(vec2 point) = 0;
    virtual void detachMouse() = 0;
};

class UIHelper {
public:
    UIMain *main;
    vec2 worldMin, worldMax;
    int width, height;
    enum {DrawMode, PullMode} dragMode;
    bool mouseDown;
    std::vector<vec2> polyline;
    float edgeMin;
    UIHelper(): main(NULL) {}
    UIHelper(UIMain *main, vec2 worldMin, vec2 worldMax, int width, int height);
    std::vector<vec2> getPolyline();
    void onKeyDown(SDL_KeyboardEvent &e);
    void onKeyUp(SDL_KeyboardEvent &e);
    void onMouseButtonDown(SDL_MouseButtonEvent &e);
    void onMouseButtonUp(SDL_MouseButtonEvent &e);
    void onMouseMotion(SDL_MouseMotionEvent &e);
    vec2 windowToWorld(int x, int y);
};

inline UIHelper::UIHelper(UIMain *main, vec2 worldMin, vec2 worldMax, int w, int h):
    main(main), worldMin(worldMin), worldMax(worldMax), width(w), height(h) {
    dragMode = DrawMode;
    mouseDown = false;
    edgeMin = 0.1;
}

inline std::vector<vec2> UIHelper::getPolyline() {
    return polyline;
}

inline void UIHelper::onKeyDown(SDL_KeyboardEvent &e) {
    if (e.keysym.scancode == SDL_SCANCODE_B) {
        main->addBox();
    } else if (e.keysym.scancode == SDL_SCANCODE_C) {
        main->addCircle();
    } else if (e.keysym.scancode == SDL_SCANCODE_BACKSPACE) {
        main->clear();
    } else if (e.keysym.scancode == SDL_SCANCODE_TAB) {
        if (!mouseDown)
            dragMode = (dragMode==DrawMode) ? PullMode : DrawMode;
    }
}

inline void UIHelper::onKeyUp(SDL_KeyboardEvent &e) {
}

inline void UIHelper::onMouseButtonDown(SDL_MouseButtonEvent &e) {
    mouseDown = true;
    vec2 point = windowToWorld(e.x,e.y);
    if (dragMode == DrawMode) {
        polyline.push_back(point);
    } else {
        main->attachMouse(point);
    }
}

inline void UIHelper::onMouseButtonUp(SDL_MouseButtonEvent &e) {
    mouseDown = false;
    if (dragMode == DrawMode) {
        if (polyline.size() > 1)
            main->addPolyline(polyline);
        polyline.clear();
    } else {
        main->detachMouse();
    }
}

inline void UIHelper::onMouseMotion(SDL_MouseMotionEvent &e) {
    if (!mouseDown)
        return;
    vec2 point = windowToWorld(e.x,e.y);
    if (dragMode == DrawMode) {
        if (glm::length(point - polyline.back()) > edgeMin)
            polyline.push_back(point);
    } else {
        main->moveMouse(point);
    }
}

inline vec2 UIHelper::windowToWorld(int x, int y) {
    vec2 u = vec2((float)x/width, (float)(height-1-y)/height);
    return worldMin + u*(worldMax - worldMin);
}

#endif
