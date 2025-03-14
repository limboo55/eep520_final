#ifndef __OBSTACLE_AGENT__H
#define __OBSTACLE_AGENT__H 

#include "enviro.h"

using namespace enviro;

class obstacleController : public Process, public AgentInterface {

    public:
    obstacleController() : Process(), AgentInterface() {}

    void init() {}
    void start() {}
    void update() {}
    void stop() {}

};

class Obstacle : public Agent {
    public:
    Obstacle(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    obstacleController c;
};

DECLARE_INTERFACE(Obstacle)

#endif