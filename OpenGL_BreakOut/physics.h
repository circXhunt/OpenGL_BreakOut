#include "box2d/box2d.h"
#include "ball_object.h"
#include "draw.h"
#include "convert.h"

#ifndef PHYSICS_H
#define PHYSICS_H

class MyListener : public b2ContactListener
{
	void BeginContact(b2Contact* contact)
	{
		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		b2Body* body1 = fixtureA->GetBody();
		b2Body* body2 = fixtureB->GetBody();
		if (body1->GetUserData() != nullptr && body2->GetUserData() != nullptr)
		{
			auto& g1 = *((GameObject*)body1->GetUserData());
			auto& g2 = *((GameObject*)body2->GetUserData());
			if (g2.Type == "Ball")
			{
				std::swap(g1, g2);
			}
			if (g1.Type == "Ball")
			{
				if (g2.Type == "Player")
				{

				}
				else if (g2.Type == "Box")
				{

				}
			}
		}
	}
	void EndContact(b2Contact* contact)
	{

	}
};

class Physics {
public:
	b2World* Wrold;
	MyListener myListener;
	std::string boxStr = "Box";
	std::string playerStr = "Player";
	std::string ballStr = "Ball";
	Physics(float width, float height) :Wrold(nullptr), myListener() {
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
		boxBodyDef.position = Convert::pi2ph(obj);
		b2Body* boxBody = Wrold->CreateBody(&boxBodyDef);

		b2PolygonShape boxBox;
		boxBox.SetAsBox(obj.Size.x / 2, obj.Size.y / 2);

		boxBody->CreateFixture(&boxBox, 1.0f);

		boxBody->SetUserData(&obj);
		//boxBody->SetUserData(&boxStr);

		return boxBody;
	}
	b2Body* CreatePlayerPhysics(GameObject& obj) {
		b2BodyDef playerBoxDef;
		playerBoxDef.type = b2_kinematicBody;
		playerBoxDef.position = Convert::pi2ph(obj);
		b2Body* playerBody = Wrold->CreateBody(&playerBoxDef);

		b2PolygonShape playerBox;
		playerBox.SetAsBox(obj.Size.x / 2, obj.Size.y / 2);

		playerBody->CreateFixture(&playerBox, 1.0f);
		playerBody->SetUserData(&obj);
		//playerBody->SetUserData(&playerStr);
		return playerBody;
	}
	b2Body* CreateBallPhysics(BallObject& obj) {
		b2BodyDef ballBoxDef;
		ballBoxDef.type = b2_dynamicBody;
		ballBoxDef.position = Convert::pi2ph(obj);
		b2Body* ballBody = Wrold->CreateBody(&ballBoxDef);


		b2CircleShape ballBox;
		ballBox.m_radius = obj.Radius;
		b2FixtureDef fixtureDef;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.3f;

		fixtureDef.shape = &ballBox;

		//ballBody->CreateFixture(&ballBox, 1.0f);
		ballBody->CreateFixture(&fixtureDef);
		ballBody->SetUserData(&obj);
		
		//ballBody->SetUserData(&ballStr);
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

		Wrold->SetContactListener(&myListener);
	}
};

#endif // !PHYSICS_H
