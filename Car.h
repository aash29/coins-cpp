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

#include "coinsLog.h"
#include "graph2.h"



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
        int player;
        b2Body *wheel;
		b2FrictionJoint *fric;
        b2Fixture *fix;
        b2Color color;
        float buffer[100];
    };

	std::vector<coin> *coins;

    coin *body2Bot(b2Body *b1) {
        for (std::vector<coin>::iterator bb = coins->begin(); bb != coins->end(); bb++) {
            if (b1 == bb->wheel) {
                return &(*bb);
            }
        }
        return 0;


    };

    void sInterface() {

        ImGui::SetNextWindowSize(ImVec2(100, 360),ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(1200,300),ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Force");

        ImGui::VSliderFloat("Force", ImVec2(100,300),&m_force,0.f,1.f);
        ImGui::End();

        ImGui::SetNextWindowSize(ImVec2(100, 360),ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(1200,300),ImGuiSetCond_FirstUseEver);

        ImGui::SetNextWindowPos(ImVec2(0, 0),ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(800, 360),ImGuiSetCond_FirstUseEver);

        ImGui::Begin("Header");
        ImGui::Text("Player: %i", m_currentPlayer);
        ImGui::Text("Message: %s", m_currentMessage);
        ImGui::End();

        if (m_showMenu) {
            ImGui::SetNextWindowSize(ImVec2(400, 200),ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(1200,300),ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Properties");
            ImGui::SliderFloat("Force multiplier",&m_forceMult,1000.f,5000.f);
            ImGui::End();
        }

        coinsLog.Draw("Log");

    }

    int getUID()
    {
        static int  i = -1;
        i++;
        return i;
    }

    coin createCoin(float x, float y, int id, int player) {
        coin bb;

        bb.id=id;
        bb.player = player;
        if (player==0)
            bb.color = b2Color(1.f,0.f,0.f);
        else
            bb.color = b2Color(0.f,1.f,0.f);



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

            //ground->CreateFixture(&fd);

            float32 groundMap[5][2] = {{-15.0f,-15.0f}, {15.0f, -15.0f}, {15.0f,15.0f}, {-15.0f,15.0f},{-15.0f,-15.0f}  };

            float32 x = 20.0f, y1 = 0.0f, dx = 5.0f;

			float x1 = groundMap[0][0];
            y1 = groundMap[0][1];
            for (int i = 1; i < 5; i++) {
                float32 x2 = groundMap[i][0];
                float32 y2 = groundMap[i][1];
                shape.Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
                ground->CreateFixture(&fd);
                y1 = y2;
                x1 = x2;
            }

        }


        //m_car=createCoin(0.f,2.f);


        coins = new std::vector<coin>;
        coins->push_back(createCoin(0.f, 9.f, getUID(),0));
        coins->push_back(createCoin(0.f, 6.f, getUID(),1));
        coins->push_back(createCoin(0.f, 2.f, getUID(),0));
        coins->push_back(createCoin(-2.f, 2.f, getUID(),1));
        coins->push_back(createCoin(2.f, 2.f, getUID(),0));


        magnetJoints = new std::map<int,b2RevoluteJoint *>;
        m_currentCoin = &((*coins)[0]);
    }

    int symmHash(short int a, short int b)
    {
        if (a>b)
            return symmHash(b,a);
        return  a << 16 |  b;
    };

    void newTurn()
    {
        m_currentCoin = nullptr;
        m_currentPlayer=(m_currentPlayer + 1)%2;
        m_force = 1.0f;
        m_forceLeft = 1.0f;
    };


    void Keyboard(int key) {

        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard) {
            switch (key) {
                case GLFW_KEY_Q:
                    m_force = b2Min(m_force + 0.05f, m_forceLeft);
                    break;

                case GLFW_KEY_E:
                    m_force = b2Max(m_force - 0.05f, 0.f);
                    break;

                case GLFW_KEY_ENTER:
                    newTurn();
                    break;

                case GLFW_KEY_TAB:
                    m_showMenu = !m_showMenu;
                    break;
            }
        }
    }

	void MouseMove(const b2Vec2 &p) {
		m_mouseWorld = p;
	}

    void RightMouseDown(const b2Vec2 &p)
    {
        m_currentCoin = nullptr;

        m_currentMessage = "lalala";

        //SetCurrent(nullptr);
        //selectedBots->clear();
    };

    void MouseDown(const b2Vec2 &p) {
        //Test::MouseDown(p);
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {

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

                if (body2Bot(body)->player==m_currentPlayer){
                    m_currentCoin = body2Bot(body);
                }
            }
            else
            {
                if (m_currentCoin) {
                    b2Vec2 f1 = m_force * m_forceMult * (p - m_currentCoin->wheel->GetPosition());
                    m_currentCoin->wheel->ApplyForceToCenter(f1, true);
                    m_forceLeft = m_forceLeft - m_force;
                    m_force = m_forceLeft;
                }
            }
        }
    }



    void Step(Settings *settings) {

        Test::Step(settings);

		HighlightCurrentCoin();

		DrawArrow();

        DrawCoins();

        sInterface();





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
        ImGui::GraphTestWindow((*coins)[0].buffer, 100);

        ImGui::Curve("Curve", ImVec2(600, 200), 10, foo);

    }


	void HighlightCurrentCoin() {
        if (m_currentCoin)
		    g_debugDraw.DrawCircle(m_currentCoin->wheel->GetWorldCenter(), 2.f, b2Color(1.f, 1.f, 1.f));
	};
 
	void DrawArrow() {
        if (m_currentCoin)
		    g_debugDraw.DrawSegment(m_currentCoin->wheel->GetWorldCenter(), m_mouseWorld, b2Color(1.f, 1.f, 1.f));
	}

    void DrawCoins () {
        for (auto it =  coins->begin(); it != coins->end(); it++) {
            g_debugDraw.DrawSolidCircle(it->wheel->GetPosition(), 1.5f, b2Vec2(0.f,0.f), it->color);
        }
    }



    static Test *Create() {

        Car* c1 = new Car();
        testCgraph(&(c1->coinsLog));
        return c1;
    }

    coin *m_currentCoin;

    float32 m_force = 1.f;
    float32 m_forceLeft = 1.f;

    float32 m_forceMult = 2500.f;
    float32 m_showMenu = false;

    int m_currentPlayer = 0;

    AppLog coinsLog;

    char* m_currentMessage="";

};

#endif
