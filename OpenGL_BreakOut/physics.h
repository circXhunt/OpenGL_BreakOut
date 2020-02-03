#include "box2d/box2d.h"
#include "ball_object.h"

#ifndef PHYSICS_H
#define PHYSICS_H

class Physics {
public :
	b2World *Wrold;
	float timeStep = 1 / 60.0;      // the length of time passed to simulate (seconds)
	Physics() :Wrold(nullptr) {
		Init();
	}
	~Physics()
	{
		delete Wrold;
	}

	void Update(float dt) {
		
		int32 velocityIterations = 8;    // how strongly to correct velocity
		int32 positionIterations = 3;    // how strongly to correct position

		Wrold->Step(timeStep, velocityIterations, positionIterations);
	}

	b2Body* CreateBoxPhysics(GameObject& obj) {
		b2BodyDef boxBodyDef;
		boxBodyDef.type = b2_staticBody;
		boxBodyDef.position.Set(obj.Position.x, obj.Position.y);
		b2Body* boxBody = Wrold->CreateBody(&boxBodyDef);

		b2PolygonShape boxBox;
		boxBox.SetAsBox(obj.Size.x, obj.Size.y);

		boxBody->CreateFixture(&boxBox, 0.0f);
		return boxBody;
	}
	b2Body* CreatePlayerPhysics(GameObject& obj) {
		b2BodyDef playerBoxDef;
		playerBoxDef.type = b2_kinematicBody;
		playerBoxDef.position.Set(obj.Position.x, obj.Position.y);
		b2Body* playerBody = Wrold->CreateBody(&playerBoxDef);

		b2PolygonShape playerBox;
		playerBox.SetAsBox(obj.Size.x, obj.Size.y);

		playerBody->CreateFixture(&playerBox, 0.0f);

		return playerBody;
	}
	b2Body* CreateBallPhysics(BallObject& obj) {
		b2BodyDef ballBoxDef;
		ballBoxDef.type = b2_dynamicBody;
		ballBoxDef.position.Set(obj.Position.x, obj.Position.y);
		b2Body* ballBody = Wrold->CreateBody(&ballBoxDef);

		b2CircleShape ballBox;
		ballBox.m_radius = obj.Radius;

		ballBody->CreateFixture(&ballBox, 0.0f);

		return ballBody;
	}

	//b2Body* CreateWallPhysics() {}

	void Test() {
		b2BodyDef groundBodyDef;
		groundBodyDef.position.Set(0.0f, -10.0f);
		b2Body* groundBody = Wrold->CreateBody(&groundBodyDef);

		b2PolygonShape groundBox;
		groundBox.SetAsBox(50.0f, 10.0f);

		groundBody->CreateFixture(&groundBox, 0.0f);
	}

private:
	void Init() {
		b2Vec2 gravity(0.0f, -10.0f);
		Wrold = new b2World(gravity);

	}
};

#endif // !PHYSICS_H
