#ifndef CAMERA_HPP
#define CAMERA_HPP

#define _USE_MATH_DEFINES
#include <cmath>
#include "engine.hpp"
#include "graphics.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using glm::vec2;
using glm::vec3;

class Perspective {
public:
    Perspective(float fov = 90, float aspect = 1,
                float zmin = 0.1, float zmax = 10):
        fov(fov), aspect(aspect), zmin(zmin), zmax(zmax) {}
    void apply(Engine *engine);
protected:
    float fov, aspect, zmin, zmax;
};

class OrbitCamera {
public:
    OrbitCamera(float dist = 1, float lat = 0, float lon = 0,
                Perspective pers = Perspective()):
        dist(dist), lat(lat), lon(lon), pers(pers), center(0,0,0) {}
    void apply(Engine *engine);
    vec3 getEye();
    vec2 getLatLon();
    void setLatLon(float lat, float lon);
    vec3 getCenter();
    void setCenter(vec3 center);
    void onMouseMotion(SDL_MouseMotionEvent&);
protected:
    vec3 center;
    float dist, lat, lon;
    Perspective pers;
};

class Camera2D {
public:
    Camera2D(vec2 worldMin=vec2(-1,-1), vec2 worldMax=vec2(1,1)):
        worldMin(worldMin), worldMax(worldMax) {}
    void apply(Engine *engine);
protected:
    vec2 worldMin, worldMax;
};

// Definitions below

inline void Perspective::apply(Engine *engine) {
    engine->matrixMode(GL_PROJECTION);
    engine->setMatrix(glm::perspective(glm::radians(fov), aspect, zmin, zmax));
}

inline void OrbitCamera::apply(Engine *engine) {
    pers.apply(engine);
    engine->matrixMode(GL_MODELVIEW);
    engine->setMatrix(glm::lookAt(getEye(), center, vec3(0,1,0)));
}

inline vec3 OrbitCamera::getEye() {
    return center + dist*vec3(sin(lon)*cos(lat), sin(lat), cos(lon)*cos(lat));
}

inline vec2 OrbitCamera::getLatLon() {
    return vec2(lat, lon);
}

inline void OrbitCamera::setLatLon(float lat, float lon) {
    this->lat = lat;
    this->lon = lon;
}

inline vec3 OrbitCamera::getCenter() {
    return center;
}

inline void OrbitCamera::setCenter(vec3 center) {
    this->center = center;
}

inline void OrbitCamera::onMouseMotion(SDL_MouseMotionEvent &e) {
    if (!(e.state & SDL_BUTTON_LMASK))
        return;
    lon -= 0.005*e.xrel;
    lat += 0.005*e.yrel;
    if (lat < -M_PI/2 + 0.001)
        lat = -M_PI/2 + 0.001;
    if (lat > M_PI/2 - 0.001)
        lat = M_PI/2 - 0.001;
}

inline void Camera2D::apply(Engine *engine) {
    engine->matrixMode(GL_PROJECTION);
    engine->setMatrix(glm::ortho(worldMin.x,worldMax.x,worldMin.y,worldMax.y));
    engine->matrixMode(GL_MODELVIEW);
    engine->setMatrix(mat4());
}

#endif
