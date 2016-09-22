//
// Created by aash29 on 22.09.16.
//

#ifndef COINS_COIN_H
#define COINS_COIN_H
struct coin {
    int id;
    int player;
    b2Body *wheel;
    b2FrictionJoint *fric;
    b2Fixture *fix;
    b2Color color;
    float buffer[100];
    bool connected = false;
};
#endif //COINS_COIN_H
