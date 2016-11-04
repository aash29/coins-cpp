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
#include <sstream>
#include <algorithm>

#include "imgui.h"
#include "graph.h"
#include "DebugDraw.h"

#include "coinsLog.h"
#include "graph2.h"
#include "cycles.h"

#include "Test.h"
#include "coin.h"

#include "json.hpp"
#include <fstream>

#include "tinydir.h"

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
    coin *body2Bot(b2Body *b1) {
        for (auto &kv : coins) {
            //for (std::vector<coin>::iterator bb = coins->begin(); bb != coins->end(); bb++) {
            if (b1 == kv.second.wheel) {
                return &(kv.second);
            }
        }
        return 0;


    };

    void sInterface() {


        if (ImGui::BeginMainMenuBar()) {

            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Reload")) {

                    coinsLog.AddLog("Reloading level \n");
                    coinsLog.AddLog(m_currentLevel.c_str());
                    coinsLog.AddLog("\n");
                    loadLevel(m_currentLevel.c_str());
                };
                ImGui::MenuItem("Open", "Ctrl+O", &m_showOpenDialog);
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::SetNextWindowPos(ImVec2(g_camera.m_width - 100, 200));
        ImGui::SetNextWindowSize(ImVec2(50, 400), ImGuiSetCond_FirstUseEver);

        ImGui::Begin("Force", nullptr, ImVec2(0, 0), 0.3f,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::VSliderFloat("##force", ImVec2(50, 350), &m_force, 0.f, 1.0f);
        m_force = std::min(m_force, m_forceLeft);
        ImGui::End();


        ImGui::SetNextWindowPos(ImVec2(10, 30));
        ImGui::SetNextWindowSize(ImVec2(g_camera.m_width - 20, 100), ImGuiSetCond_FirstUseEver);
        if (!ImGui::Begin("HUD", nullptr, ImVec2(0, 0), 0.3f,
                          ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                          ImGuiWindowFlags_NoSavedSettings)) {
            ImGui::End();
            return;
        }

        //ImGui::Text("Player: %i", m_currentPlayer);
        ImGui::Text("Turn: %i", m_currentTurn);
        ImGui::Separator();
        ImGui::Text("Mouse Position: (%.1f,%.1f)", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
        ImGui::Text("Mouse Position in world : (%.2f,%.2f)", m_mouseWorld.x, m_mouseWorld.y);
        ImGui::Text("%d", winConditionSatisfied());
        ImGui::End();


        if (m_showMenu) {
            ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiSetCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(10, 300), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Properties");
            ImGui::Text("Force multiplier");
            ImGui::SliderFloat("##fm", &m_forceMult, 10000.f, 50000.f);
            ImGui::Separator();
            if (m_currentCoin != nullptr) {
                if (ImGui::CollapsingHeader("Current coin")) {
                    ImGui::Text("id");
                    ImGui::InputInt("##id", &(m_currentCoin->id));

                    ImGui::Text("player");
                    ImGui::InputInt("##player", &(m_currentCoin->player));

                    static float pos[2] = {0.0f, 0.0f};
                    static b2Vec2 posv;
                    posv = m_currentCoin->wheel->GetPosition();
                    pos[0] = posv.x;
                    pos[1] = posv.y;
                    ImGui::Text("position");
                    ImGui::InputFloat2("##pos", pos);


                }
            }

            ImGui::End();
        }

        if (m_showOpenDialog) {
            ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiSetCond_FirstUseEver);

            if (ImGui::Begin("Open level", &m_showOpenDialog)) {

                // left
                static char selected[100] = "";
                ImGui::BeginChild("left pane", ImVec2(150, 0), true);
                tinydir_dir dir;
                if (tinydir_open(&dir, "../") != -1) {
                    tinydir_file file;
                    int i = 0;

                    while (dir.has_next) {
                        if (tinydir_readfile(&dir, &file) != -1) {
                            //ImGui::TextWrapped(file.name);
                            //coinsLog.AddLog(file.extension, "\n");
                            if (!strcmp(file.extension, "cs")) {
                                if (ImGui::Selectable(file.name, !strcmp(selected, file.name)))
                                    strcpy(selected, file.name);
                            }
                        }
                        tinydir_next(&dir);
                        i++;
                    }
                tinydir_close(&dir);
                }

                ImGui::EndChild();
                ImGui::SameLine();

                // right
                ImGui::BeginGroup();
                ImGui::BeginChild("item view", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing())); // Leave room for 1 line below us
                ImGui::Text("Selected level: %s", selected);
                ImGui::Separator();

                char pathToFile[100] = "";
                strcat(pathToFile, "../");
                strcat(pathToFile, selected);

                std::ifstream t(pathToFile);
                std::stringstream buffer;
                buffer << t.rdbuf();

                static char bstr[2000];



                strcpy(bstr,buffer.str().c_str());

                //coinsLog.AddLog(buffer.str().c_str());
                ImGui::TextWrapped(bstr);


                /*
                if (tinydir_open(&dir, "../") != -1) {
                    tinydir_file file;
                    while (dir.has_next)
                    {
                        if (tinydir_readfile(&dir, &file) != -1) {
                            ImGui::TextWrapped(file.name);
                        }
                        tinydir_next(&dir);
                    }
                }
                */


                ImGui::EndChild();
                ImGui::BeginChild("buttons");
                if (ImGui::Button("Load")) { loadLevel(selected); };
                ImGui::SameLine();
                ImGui::EndChild();
                ImGui::EndGroup();
            }
            ImGui::End();
        }


        coinsLog.Draw("Log");
    }


    int getUID() {
        static int i = -1;
        i++;
        return i;
    }

    coin createCoin(float x, float y, int id, int player) {
        coin bb;

        bb.id = id;
        bb.player = player;
        if (player == 0)
            bb.color = b2Color(1.f, 0.f, 0.f);
        else
            bb.color = b2Color(0.f, 1.f, 0.f);


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

        bb.fric = (b2FrictionJoint *) m_world->CreateJoint(&jf);

        return bb;
    };

    void removeDeadCoins() {
        for (auto c = coins.begin(); c != coins.end();) {
            if (c->second.dead) {
                m_world->DestroyBody(c->second.wheel);
                c = coins.erase(c);
            }
            else {
                ++c;
            }
        }
    }


    void cleanup() {
        for (auto c: coins) {
            m_world->DestroyBody(c.second.wheel);
        }

        for (auto g: grounds) {
            m_world->DestroyBody(g);
        }

        coins.clear();
        grounds.clear();
        m_currentCoin = nullptr;



    };

    void loadLevel(const char *name) {

        cleanup();

        m_currentLevel = std::string(name);

        coinsLog.AddLog("\n");
        coinsLog.AddLog(name);
        coinsLog.AddLog("\n");

        coinsLog.AddLog(m_currentLevel.c_str());
        coinsLog.AddLog("\n");


        nlohmann::json j;

        std::ifstream file;
        file.open(std::string("../") + std::string(name), std::ios::in);
        if (file) {
            j << file;
            file.close();
        }

        if (j.find("force") != j.end()) {
            m_forceLeft = j["force"];
            //m_force = m_forceLeft;
        }

        for (auto c : j) {

            if (c.is_object()) {
                std::string s1 = c["type"];

                if (s1 == "coin") {
                    std::vector<float> ar1 = c["pos"];

                    coinsLog.AddLog(s1.c_str());
                    coinsLog.AddLog("\n");
                    printVector(ar1, "pos");

                    int uid = getUID();
                    coins.insert(std::make_pair(uid, createCoin(ar1[0], ar1[1], uid, c["player"])));
                }


                if (s1 == "ground") {

                    b2Body *ground = NULL;
                    b2BodyDef bd;
                    bd.type=b2_staticBody;
                    ground = m_world->CreateBody(&bd);

                    b2EdgeShape shape;

                    b2FixtureDef fd;
                    fd.shape = &shape;
                    fd.density = 0.0f;
                    fd.friction = 0.9f;
                    fd.restitution = 0.7f;


                    std::vector<std::vector<float> > ar1 = c["coords"];
                    //printVector(ar1[1], "ground coords");

                    float x1 = ar1[0][0];
                    float y1 = ar1[0][1];
                    for (int i = 1; i < ar1.size(); i++) {
                        float32 x2 = ar1[i][0];
                        float32 y2 = ar1[i][1];
                        shape.Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
                        ground->CreateFixture(&fd);
                        y1 = y2;
                        x1 = x2;
                    }
                    grounds.push_back(ground);


                }
                if (s1 == "gears")
                {

                    std::vector<float> p1 = c["p1"];
                    std::vector<float> p2 = c["p2"];


                    b2RevoluteJoint* m_joint1;
                    b2RevoluteJoint* m_joint2;
                    b2PrismaticJoint* m_joint3;
                    b2GearJoint* m_joint4;
                    b2GearJoint* m_joint5;


                    b2CircleShape circle1;
                    circle1.m_radius = 1.0f;

                    b2CircleShape circle2;
                    circle2.m_radius = 2.0f;


                    b2BodyDef bd1;
                    bd1.type = b2_dynamicBody;
                    bd1.position.Set(p1[0], p1[1]);
                    b2Body* body1 = m_world->CreateBody(&bd1);
                    body1->CreateFixture(&circle1, 5.0f);

                    b2EdgeShape blade;
                    blade.Set(b2Vec2(0.0f, 0.0f), b2Vec2(10.0f, 0.0f));
                    body1->CreateFixture(&blade,1.0f);

                    grounds.push_back(body1);

                    b2RevoluteJointDef jd1;
                    jd1.bodyA = m_ground;
                    jd1.bodyB = body1;
                    jd1.localAnchorA = m_ground->GetLocalPoint(bd1.position);
                    jd1.localAnchorB = body1->GetLocalPoint(bd1.position);
                    jd1.referenceAngle = body1->GetAngle() - m_ground->GetAngle();
                    m_joint1 = (b2RevoluteJoint*)m_world->CreateJoint(&jd1);

                    b2BodyDef bd2;
                    bd2.type = b2_dynamicBody;
                    bd2.position.Set(p2[0], p2[1]);
                    b2Body* body2 = m_world->CreateBody(&bd2);
                    body2->CreateFixture(&circle2, 5.0f);
                    body2->CreateFixture(&blade,1.0f);
                    grounds.push_back(body2);

                    b2RevoluteJointDef jd2;
                    jd2.Initialize(m_ground, body2, bd2.position);
                    m_joint2 = (b2RevoluteJoint*)m_world->CreateJoint(&jd2);

                    b2GearJointDef jd4;
                    jd4.bodyA = body1;
                    jd4.bodyB = body2;
                    jd4.joint1 = m_joint1;
                    jd4.joint2 = m_joint2;
                    jd4.ratio = circle2.m_radius / circle1.m_radius;
                    m_joint4 = (b2GearJoint*)m_world->CreateJoint(&jd4);




                }


            }
        }


    };

    Car() {


        {
            b2BodyDef bd;
            m_ground = m_world->CreateBody(&bd);
/*
            b2EdgeShape shape;
            shape.Set(b2Vec2(50.0f, 0.0f), b2Vec2(-50.0f, 0.0f));
            ground->CreateFixture(&shape, 0.0f);
            */
        }

        loadLevel(m_currentLevel.c_str());

        m_currentCoin = &(coins.begin()->second);
    };

    int symmHash(short int a, short int b) {
        if (a > b)
            return symmHash(b, a);
        return a << 16 | b;
    };

    void newTurn() {
        m_currentCoin = nullptr;
        //m_currentPlayer = (m_currentPlayer + 1) % 2;
        m_currentTurn++;
        m_force = m_forceDefault;
        m_forceLeft = 1.0f;
    };


    void Keyboard(int key) {

        ImGuiIO &io = ImGui::GetIO();
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

    void RightMouseDown(const b2Vec2 &p) {
        m_currentCoin = nullptr;

    };

    void launchCoin(const b2Vec2 &p) {
        if (m_currentCoin) {
            b2Vec2 f1 =  (p - m_currentCoin->wheel->GetPosition());

			f1.Normalize();
			f1 = (m_force * m_forceMult) * f1;

            m_currentCoin->wheel->ApplyForceToCenter(f1, true);
            m_forceLeft = m_forceLeft - m_force;
            m_force = m_forceDefault;
            m_currentCoin = nullptr;
        }
    };

    void MouseDown(const b2Vec2 &p) {
        //Test::MouseDown(p);
        ImGuiIO &io = ImGui::GetIO();
        if (!io.WantCaptureMouse) {

            m_mouseWorld = p;

			coinsLog.AddLog("[%g,%g],\n", p.x, p.y);

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

                if (body2Bot(body)->player == m_currentPlayer) {
                    m_currentCoin = body2Bot(body);
                }
                else {
                    launchCoin(p);
                }
            }
            else {
                launchCoin(p);
            }
        }
    }


    void Step(Settings *settings) {

        Test::Step(settings);

        HighlightCurrentCoin();

        DrawArrow();

        DrawCoins();

        sInterface();

        DrawCycles();


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
        ImGui::GraphTestWindow(coins[0].buffer, 100);

        ImGui::Curve("Curve", ImVec2(600, 200), 10, foo);

    }


    void HighlightCurrentCoin() {
        if (m_currentCoin) {
            g_debugDraw.DrawCircle(m_currentCoin->wheel->GetWorldCenter(), 2.f, b2Color(1.f, 1.f, 1.f));
            g_debugDraw.DrawSolidCircle(m_currentCoin->wheel->GetWorldCenter(), m_proximityRadius, b2Vec2(0.f, 0.f),
                                        b2Color(1.f, 1.f, 0.3f));
        }
    };

    void DrawArrow() {
        if (m_currentCoin)
            g_debugDraw.DrawSegment(m_currentCoin->wheel->GetWorldCenter(), m_mouseWorld, b2Color(1.f, 1.f, 1.f));
    }

    void DrawCoins() {
        for (auto &kv : coins) {
            g_debugDraw.DrawSolidCircle(kv.second.wheel->GetPosition(), 1.5f, b2Vec2(0.f, 0.f), kv.second.color);
        }
    }

    void DrawCycles() {
        cycles c2 = cycles(&coins, m_proximityRadius);
        static std::vector<std::vector<b2Vec2>> polygons;
        static std::vector<std::vector<int> > cyclesOut;

        c2.FindCycles(0, c2.coinCycles[0], c2.cyclesList[0]);
        c2.FindCycles(1, c2.coinCycles[1], c2.cyclesList[1]);

        //std::vector<std::vector<b2Vec2> > c3 = c2.FindCycles(0, polygons, cyclesOut);
        b2Vec2 v0;
        if (c2.coinCycles[0].size() > 0) {
            for (auto &c : c2.coinCycles[0]) {
                v0 = c[0];
                for (auto &v : c) {
                    g_debugDraw.DrawSegment(v0, v, b2Color(1.f, 1.f, 1.f));
                    v0 = v;
                }
            }
        }

        if (c2.coinCycles[1].size() > 0) {
            for (auto &c : c2.coinCycles[1]) {
                v0 = c[0];
                for (auto &v : c) {
                    g_debugDraw.DrawSegment(v0, v, b2Color(1.f, 1.f, 1.f));
                    v0 = v;
                }
            }
        }
        c2.deleteTrappedCoins(1, c2.coinCycles[0]);
        c2.deleteTrappedCoins(0, c2.coinCycles[1]);
        removeDeadCoins();
        //c2.;
    }


    void printVector(std::vector<float> line, char *header = nullptr) {
        std::string s1;
        std::stringstream ss;
        if (header != nullptr) {
            ss << header;
        };
        ss << std::endl;
        ss << "(";
        for (auto it = line.begin(); it != line.end(); it++) {
            ss << *it << ",";
        }
        ss << ")";
        ss << std::endl;
        coinsLog.AddLog(ss.str().c_str());
    }


    void printVector(std::vector<int> line) {
        std::string s1;
        std::stringstream ss;
        //log->AddLog("lalala");
        for (auto it = line.begin(); it != line.end(); it++) {
            ss << *it << ",";
        }
        ss << std::endl;
        coinsLog.AddLog(ss.str().c_str());
    }


    void printVector(std::vector<b2Vec2> line) {
        std::string s1;
        std::stringstream ss;
        //log->AddLog("lalala");
        for (auto it = line.begin(); it != line.end(); it++) {
            ss << "(" << it->x << "," << it->y << ")";
        }
        ss << std::endl;
        coinsLog.AddLog(ss.str().c_str());
    }

    bool winConditionSatisfied() {
        return (coins.end() == std::find_if(coins.begin(), coins.end(),
                                            [](std::pair<int, coin> c1) -> bool { return (c1.second.player == 1); }));
    }


    void testCgraph() {

        std::vector<std::vector<int>> ig = std::vector<std::vector<int>>();

        std::vector<b2Vec2> vert = std::vector<b2Vec2>();

        vert.push_back(b2Vec2(0.f, 1.f));
        vert.push_back(b2Vec2(-1.f, 0.f));
        vert.push_back(b2Vec2(1.f, 0.f));


        vert.push_back(b2Vec2(10.f, 1.f));
        vert.push_back(b2Vec2(9.f, 0.f));
        vert.push_back(b2Vec2(11.f, 0.f));


        std::vector<int> v1 = std::vector<int>();

        v1.push_back(0);
        v1.push_back(1);

        ig.push_back(v1);

        v1[0] = 0;
        v1[1] = 2;

        ig.push_back(v1);

        v1[0] = 1;
        v1[1] = 2;

        ig.push_back(v1);


        ig.push_back(v1);

        v1[0] = 3;
        v1[1] = 5;

        ig.push_back(v1);

        v1[0] = 4;
        v1[1] = 5;

        ig.push_back(v1);

        v1[0] = 3;
        v1[1] = 4;

        ig.push_back(v1);


        coinsLog.AddLog("neighbors \n");
        CGraph cg1 = CGraph(ig, vert, &coinsLog);

        //cg1.printNeighbors();

        std::vector<std::vector<int>> c1 = cg1.findCycles2();
        coinsLog.AddLog("cycles \n");
        printVector(c1[0]);
        printVector(c1[1]);

        std::vector<std::vector<b2Vec2>> polygons;
        std::vector<std::vector<int> > cyclesOut;
        cycles c2 = cycles(&coins, m_proximityRadius);
        std::vector<std::vector<b2Vec2> > c3 = c2.FindCycles(0, polygons, cyclesOut);
        for (auto &v : c3) {
            printVector(v);
        }

    };

    static Test *Create() {

        Car *c1 = new Car();
        //c1->testCgraph();
        return c1;
    }


    coin *m_currentCoin;

    float32 m_forceDefault = 0.333f;
    float32 m_force = m_forceDefault;
    float32 m_forceLeft = 1.f;
    float32 m_forceMult = 35000.f;
    float32 m_proximityRadius = 25.f;


    b2Body* m_ground = nullptr;

    bool m_showMenu = true;
    bool m_showOpenDialog = false;

    int m_currentPlayer = 0;

    int m_currentTurn = 1;

    std::string m_currentLevel = "level.cs";

    AppLog coinsLog;

    std::map<int, coin> coins = std::map<int, coin>();
    std::vector<b2Body*> grounds = std::vector<b2Body*>();
    //std::vector<b2Body*> gears = std::vector<b2Body*>();
};


#endif
