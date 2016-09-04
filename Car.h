/*
* Copyright (c) 2006-2011 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef CAR_H
#define CAR_H

#include <vector>
#include <map>
#include <iostream>
#include "imgui.h"
#include "graph.h"
#include "DebugDraw.h"


class QueryCallback : public b2QueryCallback {
public:
    QueryCallback(const b2Vec2 &point) {
        m_point = point;
        m_fixture = NULL;
    }

    bool ReportFixture(b2Fixture *fixture) {
        b2Body *body = fixture->GetBody();
        if (body->GetType() == b2_dynamicBody) {
            bool inside = fixture->TestPoint(m_point);
            if (inside) {
                m_fixture = fixture;

                // We are done, terminate the query.
                return false;
            }
        }

        // Continue the query.
        return true;
    }

    b2Vec2 m_point;
    b2Fixture *m_fixture;
};

// This is a fun demo that shows off the wheel joint
class Car : public Test {
public:


    struct coin {
        int id;
        b2Body *wheel;
		b2FrictionJoint *fric;
        b2Fixture *fix;
        float buffer[100];
    };

	std::vector<coin> *bots;
    coin *body2Bot(b2Body *b1) {
        for (std::vector<coin>::iterator bb = bots->begin(); bb != bots->end(); bb++) {
            if (b1 == bb->wheel) {
                return &(*bb);
            }
        }
        return 0;


    };

    coin createCoin(float x, float y, int id) {
        coin bb;

        bb.id=id;


        b2CircleShape circle;
        circle.m_radius = 1.5f;

        b2BodyDef bd;

        bd.type = b2_dynamicBody;
        bd.position.Set(x, y);


        b2FixtureDef fd;
        fd.shape = &circle;
        fd.density = 1.0f;
        fd.friction = 0.9f;
		fd.restitution = 0.7f;
        fd.filter.groupIndex = 2;


        bd.position.Set(x, y);
        bb.wheel = m_world->CreateBody(&bd);
        bb.wheel->CreateFixture(&fd);


		b2FrictionJointDef jf;

		jf.Initialize(bb.wheel, m_groundBody, bb.wheel->GetPosition());

		jf.maxForce = 100.f;
		jf.maxTorque = 100.f;

		bb.fric = (b2FrictionJoint*)m_world->CreateJoint(&jf);

        return bb;
    };

    Car() {


        b2Body *ground = NULL;
        {
            b2BodyDef bd;
            ground = m_world->CreateBody(&bd);

            b2EdgeShape shape;

            b2FixtureDef fd;
            fd.shape = &shape;
            fd.density = 0.0f;
            fd.friction = 0.9f;
			fd.restitution = 0.7f;

            ground->CreateFixture(&fd);

            float32 groundMap[5][2] = {{-15.0f,-15.0f}, {15.0f, -15.0f}, {15.0f,15.0f}, {-15.f,15.0f} , {-15.0f,-15.0f } };

            float32 x = 20.0f, y1 = 0.0f, dx = 5.0f;

			float x1 = groundMap[0][0];
            y1 = groundMap[0][1];
            for (int i = 1; i < 5; ++i) {
                float32 x2 = groundMap[i][0];
                float32 y2 = groundMap[i][1];
                shape.Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
                ground->CreateFixture(&fd);
                y1 = y2;
                x1 = x2;
            }
        }




        //m_car=createCoin(0.f,2.f);


        bots = new std::vector<coin>;
        bots->push_back(createCoin(0.f, 9.f, 0));
        bots->push_back(createCoin(0.f, 6.f, 1));
        bots->push_back(createCoin(0.f, 2.f, 2));
        bots->push_back(createCoin(-2.f, 2.f, 3));
        bots->push_back(createCoin(2.f, 2.f, 3));
        

        magnetJoints = new std::map<int,b2RevoluteJoint *>;
        currentBot = &((*bots)[0]);
    }

    int symmHash(short int a, short int b)
    {
        if (a>b)
            return symmHash(b,a);
        return  a << 16 |  b;
    };


    void Keyboard(int key) {
        switch (key) {

        }
    }

    //coin* body2bot(b2)


    void MouseDown(const b2Vec2 &p) {
        //Test::MouseDown(p);

        m_mouseWorld = p;


        if (m_mouseJoint != NULL) {
            return;
        }


        // Make a small box.
        b2AABB aabb;
        b2Vec2 d;
        d.Set(0.001f, 0.001f);
        aabb.lowerBound = p - d;
        aabb.upperBound = p + d;

        // Query the world for overlapping shapes.
        QueryCallback callback(p);
        m_world->QueryAABB(&callback, aabb);

        if (callback.m_fixture) {

            b2Body *body = callback.m_fixture->GetBody();

            currentBot = body2Bot(body);

          
            {

                b2MouseJointDef md;
                md.bodyA = m_groundBody;
                md.bodyB = body;
                md.target = p;
                md.maxForce = 1000.0f * body->GetMass();
                m_mouseJoint = (b2MouseJoint *) m_world->CreateJoint(&md);
                body->SetAwake(true);
            }

        }



		/*
        for (int i = 0; i < bots->size(); i++) {
            for (int j = 0; j < 4; j++) {
                if ((m_mouseWorld - (*bots)[i].magnets[j].pos).Length()<0.2f){
                    (*bots)[i].magnets[j].active=!(*bots)[i].magnets[j].active;
                }
            }
        }
		*/

    }



    void Step(Settings *settings) {

        Test::Step(settings);
    }


    void plotGraphs() {


        ImVec2 foo[10];

        foo[0].x = 0; // init data so editor knows to take it from here

        for (int i = 0; i < 10; i++) {
            foo[i].x = i * 0.1f;
            foo[i].y = i * 0.1f;
        }


        float foo2[100];
        for (int i = 0; i < 100; i++) {
            foo2[i] = sinf(i * 0.1f);
        }

        bool f = true;
        ImGui::GraphTestWindow((*bots)[0].buffer, 100);

        ImGui::Curve("Curve", ImVec2(600, 200), 10, foo);

    }


 

 

    static Test *Create() {
        return new Car;
    }


 

    coin *currentBot;

    float32 m_speed;
    std::map<int,b2RevoluteJoint *> *magnetJoints;
};

#endif
