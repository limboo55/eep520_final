#ifndef __BOX_AGENT__H
#define __BOX_AGENT__H 

#include "enviro.h"

using namespace enviro;

class BoxController : public Process, public AgentInterface {

    public:
    BoxController() : Process(), AgentInterface() {}

    void init() {
        prevent_rotation();
        // watch reset button click,teleport to original position
        watch("button_click", [&](Event& e) {
            if (e.value()["value"] == "reset") {
                teleport(initial_x,initial_y,0);
            }
        });
    }

    void start() {
        initial_x = x();
        initial_y = y();
    }

    void update() {
        // Apply damping to make the box movement feel more natural and prevent excessive sliding
        omni_damp_movement();
        
    }

    void stop() {}
    private:
    double initial_x;
    double initial_y;
};

class Box : public Agent {
    public:
    Box(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    BoxController c;
};

DECLARE_INTERFACE(Box)

#endif