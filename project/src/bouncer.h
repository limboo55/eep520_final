#ifndef __BOUNCER_AGENT__H
#define __BOUNCER_AGENT__H 

#include "enviro.h"
#include <string>

using namespace enviro;

class BouncerController : public Process, public AgentInterface {

    public:
    BouncerController() : Process(), AgentInterface(), animation_counter(0) {}

    void init() {
        prevent_rotation();
        // decoration function
        update_decoration();
    }

    void start() {}

    void update() {
        
        animation_counter = (animation_counter + 1) % 60;
        if (animation_counter % 20 == 0) {
            update_decoration();
        }
    }

    void stop() {}

    private:
    int animation_counter;

    void update_decoration() {
        // bouncer symbol
        std::string svg = "<g>"
            
            "<rect x=\"-15\" y=\"-15\" width=\"30\" height=\"30\" rx=\"5\" style=\"fill:#FF4500;stroke:none\" />"
            "<path d=\"M0,12 L0,-12 L-8,-4 M0,-12 L8,-4\" "
            "style=\"fill:none;stroke:white;stroke-width:4;stroke-linecap:round\" />"
            
            // add text
            "<text x=\"-20\" y=\"25\" style=\"font-family:Arial;font-size:10px;font-weight:bold;fill:white\">BOUNCE!</text>"
            "</g>";
            
        decorate(svg);
    }
};

class Bouncer : public Agent {
    public:
    Bouncer(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    BouncerController c;
};

DECLARE_INTERFACE(Bouncer)

#endif