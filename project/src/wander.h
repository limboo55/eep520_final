#ifndef __Wander_AGENT__H
#define __Wander_AGENT__H 

#include "enviro.h"

using namespace enviro;

class WanderController : public Process, public AgentInterface {

    public:
    WanderController() : Process(), AgentInterface(), vy(20) {}

    void init() {
        prevent_rotation();
        notice_collisions_with("Obstacle", [&](Event &e) {
            vy = -vy; // Reverse direction immediately on collision
            counter = 10; // debounce period
        }); 
        notice_collisions_with("Robot", [&](Event &e) {
            teleport(initial_x, initial_y,0);
        });   
        decorate(R"(<g>
            <circle cx=-5 cy=-3 r=2 style='fill:black'></circle>
            <circle cx=5 cy=-3 r=2 style='fill:black'></circle></g>)");
        
        
        // label("vy: " + std::to_string(vy), -10, -20);
    }

    void start() {
        initial_x = x();
        initial_y = y();
    }

    void update() {
        
        // if (counter % 60 == 0) {
        //     std::cout << "Position: " << y() << ", Velocity: " << velocity().y << ", Target: " << vy << "\n";
        // }
        
        double fy = -20*(velocity().y-vy); // Lower gain for stability
        omni_apply_force(0, fy);
        
        // label("vy: " + std::to_string(vy) + "\nvy: " + std::to_string(velocity().y), -10, -20);
        if (counter > 0) {
            counter--;
        }
    }

    void stop() {}
    double initial_x, initial_y; 
    double vy;
    int counter = 0;
};

class Wander : public Agent {
    public:
    Wander(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    WanderController c;
};

DECLARE_INTERFACE(Wander)

#endif