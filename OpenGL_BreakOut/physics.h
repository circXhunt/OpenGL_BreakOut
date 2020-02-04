#include "box2d/box2d.h"
#include "ball_object.h"
#include "draw.h"

#ifndef PHYSICS_H
#define PHYSICS_H

class Physics {
public:
	b2World* Wrold;

	Physics(float width, float height) :Wrold(nullptr) {
		Init(width, height);
		g_debugDraw.Create();
	}
	~Physics()
	{
		delete Wrold;
		g_debugDraw.Destroy();
	}

	void Update(float dt) {

		int32 velocityIterations = 8;    // how strongly to correct velocity
		int32 positionIterations = 3;    // how strongly to correct position

		Wrold->Step(dt, velocityIterations, positionIterations);

	}

	void Render() {
		uint32 flags = 0;
		flags += b2Draw::e_shapeBit;
		flags += b2Draw::e_jointBit;
		flags += b2Draw::e_aabbBit;
		flags += b2Draw::e_centerOfMassBit;
		g_debugDraw.SetFlags(flags);
		Wrold->DrawDebugData();
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

	}

private:
	void Init(float width, float height) {
		b2Vec2 gravity(0.0f, 0.0f);
		Wrold = new b2World(gravity);

		Wrold->SetDebugDraw(&g_debugDraw);

		b2BodyDef WallBodyDef;
		b2Body* wallBody = Wrold->CreateBody(&WallBodyDef);

		b2EdgeShape wallBox;
		wallBox.Set(b2Vec2_zero, b2Vec2(0, height));
		wallBody->CreateFixture(&wallBox, 0.0f);

		wallBox.Set(b2Vec2_zero, b2Vec2(width, 0));
		wallBody->CreateFixture(&wallBox, 0.0f);

		wallBox.Set(b2Vec2(width, 0), b2Vec2(width, height));
		wallBody->CreateFixture(&wallBox, 0.0f);
	}
};

#endif // !PHYSICS_H
