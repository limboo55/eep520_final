#ifndef __DOOR_AGENT__H
#define __DOOR_AGENT__H 

#include "enviro.h"

using namespace enviro;

class DoorController : public Process, public AgentInterface {

    public:
    DoorController() : Process(), AgentInterface(), victory(false) {}

    void init() {
        prevent_rotation();
        // Watch for collisions with the player
        notice_collisions_with("Robot", [&](Event &e) {
            if (!victory) {
                victory = true;
                // Add a victory decoration
                decorate(R"(<g><text x="-40" y="0" 
                          style="font-family: Arial; font-size: 24px; fill: green">
                          VICTORY!</text></g>)");
                
                // Emit a victory event
                emit(Event("victory"));
            }
        });
        // watch for reset button click , reset decoration
        watch("button_click", [&](Event& e) {
            if (e.value()["value"] == "reset") {
                victory = false;
                decorate(R"(<g>
                    <rect x="-20" y="-30" width="40" height="60" style="fill:none;stroke:brown;stroke-width:2" />
                    <circle cx="15" cy="0" r="5" style="fill:gold" />
                    </g>)");
            }
        });
        
        // Add a visual decoration to make the door recognizable
        decorate(R"(<g>
            <rect x="-20" y="-30" width="40" height="60" style="fill:none;stroke:brown;stroke-width:2" />
            <circle cx="15" cy="0" r="5" style="fill:gold" />
            </g>)");
    }

    void start() {}

    void update() {
        
    }

    void stop() {}

    private:
    bool victory;
};

class Door : public Agent {
    public:
    Door(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    DoorController c;
};

DECLARE_INTERFACE(Door)

#endif