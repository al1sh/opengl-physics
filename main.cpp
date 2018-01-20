#include "engine.hpp"
#include "camera.hpp"
#include "config.hpp"
#include "draw.hpp"
#include "mesh.hpp"
#include "shapes.hpp"
#include "uihelper.hpp"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>
#include "./Box2D/Box2D.h"

using namespace std;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;

class PencilPhysics: public Engine, UIMain {
public:

    SDL_Window *window;
    Camera2D camera;
    UIHelper uiHelper;
    Draw draw;
	b2World *world;

    Polyline walls;
    Circle redCircle;
    Box whiteBox;
    vector<Circle> circles;
    vector<Box> boxes;
    vector<Polyline> polylines;

	vector<b2Body*> bodies;
	b2MouseJoint* mouseJoint;

    vec2 worldMin, worldMax;

    PencilPhysics() {
		world = new b2World(b2Vec2(0, -9.8));
        worldMin = vec2(-8, 0);
        worldMax = vec2(8, 9);
        window = createWindow("4611", 1280, 720);
        camera = Camera2D(worldMin, worldMax);
        uiHelper = UIHelper(this, worldMin, worldMax, 1280, 720);
        draw = Draw(this);
        // Initialize world
        initWorld();
    }

    ~PencilPhysics() {
        SDL_DestroyWindow(window);
    }

    void initWorld() {

        // TODO: Create a Box2D world and make these shapes static
        // bodies in it.

		//b2Vec2 gravity(0.0f,-10.0f);
		//b2World world(gravity);
		//b2BodyDef groundBodyDef;
		//groundBodyDef.position.Set(worldMax.x,worldMin.x);
		//b2Body* groundBody = world.CreateBody(&groundBodyDef);
		//b2PolygonShape groundBox;
		//groundBox.SetAsBox(50.0f, 10.0f);
		//groundBody->CreateFixture(&groundBox, 0.0f);

		//b2BodyDef bodyDef;
		//bodyDef.type = b2_dynamicBody;
		//bodyDef.position.Set(0.0f, 4.0f);
		//b2Body* body = world.CreateBody(&bodyDef);
		//b2PolygonShape dynamicBox;
		//dynamicBox.SetAsBox(1.0f, 1.0f);
		//b2FixtureDef fixtureDef;
		//fixtureDef.shape = &dynamicBox;
		//fixtureDef.density = 1.0f;
		//fixtureDef.friction = 0.3f;
		//body->CreateFixture(&fixtureDef);

		
		// Set up ground body
		float height = 0.1;
		b2BodyDef groundDef;
		groundDef.position.Set(0, 0);
		b2Body *groundBody = world->CreateBody(&groundDef);
		b2PolygonShape groundShape;
		groundShape.SetAsBox(16, 0.5);
		groundBody->CreateFixture(&groundShape, 0.0f);

        // Create walls
        vector<vec2> wallVerts;
        wallVerts.push_back(vec2(worldMin.x, worldMax.y));
        wallVerts.push_back(vec2(worldMin.x, worldMin.y));
        wallVerts.push_back(vec2(worldMax.x, worldMin.y));
        wallVerts.push_back(vec2(worldMax.x, worldMax.y));
        walls = Polyline(wallVerts, world);

		polylines.push_back(walls);

        // Create two static bodies
        redCircle = Circle(vec2(-5,2), 0.5);
        whiteBox = Box(vec2(5,2), vec2(0.9,0.9));

		 //Set up rigid body
		 //red circle
		b2BodyDef red_body_def;
		red_body_def.type = b2_staticBody;
		red_body_def.position.Set(redCircle.center.x, redCircle.center.y);
		red_body_def.linearVelocity.Set(0, 0);
		red_body_def.angularVelocity = 0.1;
		b2Body *red_circle_body = world->CreateBody(&red_body_def);

		b2CircleShape b2_red;
		b2_red.m_p.Set(0, 0);
		b2_red.m_radius = redCircle.radius;

		b2FixtureDef fixtureDef;
		fixtureDef.shape = &b2_red;
		fixtureDef.density = .2;
		fixtureDef.friction = .4;
		fixtureDef.restitution = .4;
		red_circle_body->CreateFixture(&fixtureDef);

		

		// white rectangle
		b2BodyDef white_rect_def;
		white_rect_def.type = b2_staticBody;
		white_rect_def.position.Set(whiteBox.center.x, whiteBox.center.y);
		white_rect_def.linearVelocity.Set(0, 0);
		white_rect_def.angularVelocity = 0.1;
		b2Body *white_rect_body = world->CreateBody(&white_rect_def);

		b2PolygonShape polygon;
		polygon.SetAsBox(whiteBox.size.x/2, whiteBox.size.y/2);
		b2FixtureDef fixtureDef_whitebox;
		fixtureDef_whitebox.shape = &polygon;
		fixtureDef_whitebox.density = .2;
		fixtureDef_whitebox.friction = .4;
		fixtureDef_whitebox.restitution = .4;
		white_rect_body->CreateFixture(&fixtureDef_whitebox);

		bodies.push_back(red_circle_body);
		bodies.push_back(white_rect_body);
    }

    void run() {
        float fps = 60, dt = 1/fps;
        while (!shouldQuit()) {
            handleInput();
            advanceState(dt);
            drawGraphics();
            waitForNextFrame(dt);
        }
    }

    vec2 randomVec2() {
        return vec2(2.*rand()/RAND_MAX-1, 2.*rand()/RAND_MAX-1);
    }

    void addCircle() {
        vec2 position = vec2(-5,7) + 0.5*randomVec2();
        circles.push_back(Circle(position, 0.5, world));
    }

    void addBox() {
        vec2 position = vec2(-5,7) + 0.5*randomVec2();
        boxes.push_back(Box(position, vec2(1.2,0.6), world));
    }

    void addPolyline(vector<vec2> vertices) {
        polylines.push_back(Polyline(vertices, world));
    }

    void clear() {
        for (int i = 0; i < circles.size(); i++)
            world->DestroyBody(circles[i].circle_body);
        circles.clear();
        for (int i = 0; i < boxes.size(); i++)
			world->DestroyBody(boxes[i].rect_body);
        boxes.clear();
        for (int i = 0; i < polylines.size(); i++)
			world->DestroyBody(polylines[i].chain_body);
        polylines.clear();
    }

    void onKeyDown(SDL_KeyboardEvent &e) {
        uiHelper.onKeyDown(e);
    }
    void onKeyUp(SDL_KeyboardEvent &e) {
        uiHelper.onKeyUp(e);
    }
    void onMouseButtonDown(SDL_MouseButtonEvent &e) {
        uiHelper.onMouseButtonDown(e);
    }
    void onMouseButtonUp(SDL_MouseButtonEvent &e) {
        uiHelper.onMouseButtonUp(e);
    }
    void onMouseMotion(SDL_MouseMotionEvent &e) {
        uiHelper.onMouseMotion(e);
    }

    void advanceState(float dt) {

        // TODO: Step the Box2D world by dt.
		world->Step(dt, 8, 3);

		for (int i = 0; i < circles.size(); i++)
		{
			circles[i].center.x = circles[i].circle_body->GetPosition().x;
			circles[i].center.y = circles[i].circle_body->GetPosition().y;
		}

		for (int i = 0; i < boxes.size(); i++)
		{
			boxes[i].center.x = boxes[i].rect_body->GetPosition().x;
			boxes[i].center.y = boxes[i].rect_body->GetPosition().y;
		}

		//for (int i = 0; i < polylines.size(); i++)
		//{
		//	polylines[i].center.x = polylines[i].chain_body->GetPosition().x;
		//	polylines[i].center.y = polylines[i].chain_body->GetPosition().y;
		//}


		redCircle.center.x = bodies[0]->GetPosition().x;
		redCircle.center.y = bodies[0]->GetPosition().y;

		whiteBox.center.x = bodies[1]->GetPosition().x;
		whiteBox.center.y = bodies[1]->GetPosition().y;

    }

    void drawGraphics() {
        // Light gray background
        glClearColor(0.8,0.8,0.8, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST); // don't use z-buffer because 2D
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1,1);
        // Apply camera transformation
        camera.apply(this);
        // Draw current polyline
        vector<vec2> currentPolyline = uiHelper.getPolyline();
        if (currentPolyline.size() >= 2)
            draw.polyline(mat4(), currentPolyline, vec3(0.6,0.6,0.6));

        // TODO: Modify these draw calls to draw the bodies with the
        // correct positions and angles.

        // Draw red circle and white box.
        draw.circle(mat4(), redCircle.center, redCircle.radius, vec3(1,0,0));
        draw.box(mat4(), whiteBox.center, whiteBox.size, vec3(1,1,1));
        // Draw all the other circles, boxes, and polylines
        for (int i = 0; i < circles.size(); i++)
            draw.circle(circles[i].getTransformation(), vec2(0, 0), circles[i].radius, vec3(0,0,0));
        for (int i = 0; i < boxes.size(); i++)
            draw.box(boxes[i].getTransformation(), vec2(0, 0), boxes[i].size, vec3(0,0,0));
        for (int i = 0; i < polylines.size(); i++)
            draw.polyline(mat4(), polylines[i].vertices, vec3(0,0,0));

        // Finish
        SDL_GL_SwapWindow(window);
    }

    void attachMouse(vec2 worldPoint) {

        // TODO: Check if there is a circle or box that contains the
        // world point. If so, set mouseJoint to be a new b2MouseJoint
        // attached to the body with the point as target. The most
        // important parameters of b2MouseJointDef are bodyA (a static
        // Box2D body, e.g. the walls), bodyB (the chosen body being
        // moved) and target (the world-space point to pull bodyB
        // towards). Recommended values for other parameters you will
        // need to set are:
        //   collideConnected: true
        //   maxForce: 100
        //   frequencyHz: 2
        //   dampingRatio: 0.5

		b2Body* attachment = NULL;
		for (int i = 0; i < circles.size(); i++)
		{
			if (circles[i].contains(worldPoint))
				attachment = circles[i].circle_body;
		}

		for (int i = 0; i < boxes.size(); i++)
		{
			if (boxes[i].contains(worldPoint))
				attachment = boxes[i].rect_body;
		}

		if (attachment != NULL)
		{
			b2MouseJointDef mouseJointDef;
			mouseJointDef.maxForce = 100;
			mouseJointDef.bodyA = bodies[0];
			mouseJointDef.bodyB = attachment;
			mouseJointDef.target = b2Vec2(worldPoint.x, worldPoint.y);
			mouseJointDef.collideConnected = true;
			mouseJointDef.maxForce = 100;
			mouseJointDef.frequencyHz = 2;
			mouseJointDef.dampingRatio = 0.5;

			mouseJoint = static_cast<b2MouseJoint*>(world->CreateJoint(&mouseJointDef));
		}


    }

    void moveMouse(vec2 worldPoint) {

        // TODO: If mouseJoint is not null, use SetTarget() to update
        // its target to the given point.

		if (mouseJoint != NULL)
		{
			b2Vec2 point;
			point.x = worldPoint.x;
			point.y = worldPoint.y;
			mouseJoint->SetTarget(point);
		}
    }

    void detachMouse() {

        // TODO: If mouseJoint is not null, destroy it and set it to
        // null.

		if (mouseJoint != NULL)
		{	
			world->DestroyJoint(mouseJoint);
			mouseJoint = NULL;
		};

    }

};

int main(int argc, char **argv) {
    PencilPhysics physics;
    physics.run();
    return EXIT_SUCCESS;
}
