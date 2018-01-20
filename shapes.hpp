#ifndef SHAPES_HPP
#define SHAPES_HPP

#include <glm/glm.hpp>
#include <vector>
#include <Box2D/Box2D.h>
using namespace std;
using glm::vec2;


class Circle {
public:
    vec2 center;
    float radius;
	b2Body* circle_body;
	mat4 rotation;

	vec2 getLocalPoint(vec2 worldPoint)
	{
		return vec2(circle_body->GetPosition().x, circle_body->GetPosition().x);
	}

	mat4 getTransformation()
	{
		return glm::translate(mat4(),vec3(circle_body->GetPosition().x, circle_body->GetPosition().y, 0)) * glm::rotate(mat4(),circle_body->GetAngle(), vec3(0, 0, 1));
	}

    Circle(vec2 center, float radius, b2World *world_ptr) {
		this->center = center;
		this->radius = radius;

		b2BodyDef circle_def;
		circle_def.type = b2_dynamicBody;
		circle_def.position.Set(center.x, center.y);
		circle_def.linearVelocity.Set(0, 0);
		circle_def.angularVelocity = 0.1;
		b2Body *circle_body = world_ptr->CreateBody(&circle_def);

		b2CircleShape b2_circle;
		b2_circle.m_p.Set(0, 0);
		b2_circle.m_radius = radius;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &b2_circle;
		fixtureDef.density = .2;
		fixtureDef.friction = .4;
		fixtureDef.restitution = .4;
		circle_body->CreateFixture(&fixtureDef);

		this->circle_body = circle_body;
	}


    Circle(vec2 center, float radius) {
        this->center = center;
        this->radius = radius;
    }

	Circle() {}

    bool contains(vec2 worldPoint) {
        return glm::length(worldPoint - center) <= radius;
    }
    void destroy() {}


};

class Box {
public:
    vec2 center;
    vec2 size;
	b2Body* rect_body;

	mat4 getTransformation()
	{
		return glm::translate(mat4(), vec3(rect_body->GetPosition().x, rect_body->GetPosition().y, 0)) * glm::rotate(mat4(), rect_body->GetAngle(), vec3(0, 0, 1));
	}

    Box() {}
    Box(vec2 center, vec2 size) {
        this->center = center;
        this->size = size;
    }

	Box(vec2 center, vec2 size, b2World *world_ptr) {
		this->center = center;
		this->size = size;

		b2BodyDef rect_def;
		rect_def.type = b2_dynamicBody;
		rect_def.position.Set(center.x, center.y);
		rect_def.linearVelocity.Set(0, 0);
		rect_def.angularVelocity = 0.1;
		b2Body *rect_body = world_ptr->CreateBody(&rect_def);

		b2PolygonShape polygon;
		polygon.SetAsBox(size.x/2, size.y/2);
		b2FixtureDef fixtureDef_box;
		fixtureDef_box.shape = &polygon;
		fixtureDef_box.density = .2;
		fixtureDef_box.friction = .4;
		fixtureDef_box.restitution = .4;
		rect_body->CreateFixture(&fixtureDef_box);

		this->rect_body = rect_body;
	}

    bool contains(vec2 worldPoint) {
        vec2 d = worldPoint - center;
        return (abs(d.x) <= size.x/2 && abs(d.y) <= size.y/2);
    }
    void destroy() {}
};

class Polyline {
public:
    vector<vec2> vertices;
	b2Body* chain_body;

    Polyline() {}
    Polyline(vector<vec2> vertices) {
        this->vertices = vertices;
    }

	Polyline(vector<vec2> vertices, b2World *world_ptr) {
		this->vertices = vertices;

		vector<b2Vec2> bv2vertices;
		for (int i = 0; i < vertices.size(); i++) {
			bv2vertices.push_back(b2Vec2(vertices[i].x, vertices[i].y));
		}

		b2ChainShape chain;
		b2Vec2* v = &bv2vertices[0];
		chain.CreateChain(v, bv2vertices.size());

		b2BodyDef chain_def;
		chain_def.type = b2_staticBody;
		chain_def.position.Set(0, 0);
		chain_def.linearVelocity.Set(0, 0);
		chain_def.angularVelocity = 0.0;

		b2Body *chain_body = world_ptr->CreateBody(&chain_def);

		b2FixtureDef fixtureDef_chain;
		fixtureDef_chain.shape = &chain;
		fixtureDef_chain.density = 1.0;
		fixtureDef_chain.friction = .4;
		fixtureDef_chain.restitution = .4;
		chain_body->CreateFixture(&fixtureDef_chain);

		this->chain_body = chain_body;

	}

    void destroy() {}
};

#endif
