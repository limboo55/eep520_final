#ifndef __ROBOT_AGENT__H
#define __ROBOT_AGENT__H 

#include "enviro.h"
#include <math.h>

using namespace enviro;

// Helper function to find minimum of two values
double min(double a, double b) {
    if (a < b) { 
        return a;
    } else {
        return b;
    }
}

class RobotController : public Process, public AgentInterface {

    public:
    // Constructor initializes movement state flags and physics constants
    RobotController() : Process(), AgentInterface(), 
                        LEFT(false), RIGHT(false), JUMP(false),
                        VEL_X(20), JUMP_F(-3000), K_X(25), G(400), H_MIN(1.0),
                        NORMAL_JUMP_FORCE(-3000), CHEAT_JUMP_FORCE(-10000),
                        BOUNCER_FORCE(-7000), AIR_CONTROL_FACTOR(0.4),
                        RESET_ROTATION(0) {}

    // Check distance from ground using sensors
    double height() {
        return min(sensor_value(0), sensor_value(1));
    }

    // Determine if robot is in the air
    double airborne() { 
        return height() > H_MIN;
    }

    void init() {
        prevent_rotation();
        
        // Robot movement controller - keyboard input handling
        watch("keydown", [&](Event& e) {
            std::string k = e.value()["key"];
            if (k == KEY_JUMP && !airborne()) {
                JUMP = true;
            } else if (k == KEY_LEFT) {
                LEFT = true;
            } else if (k == KEY_RIGHT) {
                RIGHT = true;
            } 
        });
        
        watch("keyup", [&](Event& e) {
            std::string k = e.value()["key"];
            if (k == KEY_LEFT) {
                LEFT = false;
            } else if (k == KEY_RIGHT) {
                RIGHT = false;
            }
        });
        
        // Enemy collision handling - reset position when hit
        notice_collisions_with("Ghost", [&](Event &e) {
            teleport(initial_x, initial_y, RESET_ROTATION); 
        }); 
        
        notice_collisions_with("Wander", [&](Event &e) {
            teleport(initial_x, initial_y, RESET_ROTATION); 
        }); 
        
        // Handle tracker collision events
        watch("tracker_collision", [&](Event& e) {
            teleport(initial_x, initial_y, RESET_ROTATION); 
        });
        
        // Bouncer interaction - apply upward force when hitting a bouncer
        notice_collisions_with("Bouncer", [&](Event &e) {
            omni_apply_force(0, BOUNCER_FORCE);
        });
        
        // Reset button handler
        watch("button_click", [&](Event& e) {
            if (e.value()["value"] == BUTTON_RESET) {
                teleport(initial_x, initial_y, RESET_ROTATION); 
                G = DEFAULT_GRAVITY;
                decorate(EMPTY_DECORATION); // Clear any decorations
                emit(Event(EVENT_ROBOT_START, {{"id", id()}}));
            }
        });
        
        // Cheat mode toggle - modify jump force
        watch("button_click", [&](Event& e) {
            if (e.value()["value"] == BUTTON_CHEAT) {
                JUMP_F = (JUMP_F == NORMAL_JUMP_FORCE ? CHEAT_JUMP_FORCE : NORMAL_JUMP_FORCE);
            }
        });
        
        // Victory handling - add crown decoration when reaching door
        notice_collisions_with("Door", [&](Event &e) {
            // Add golden crown decoration to robot
            decorate(CROWN_DECORATION);
        });
    }
    
    void start() {
        // Store initial position for reset functionality
        initial_x = x();
        initial_y = y();
        
        // Broadcast robot ID for tracker enemies
        emit(Event(EVENT_ROBOT_START, {{"id", id()}}));
    }

    void update() {
        // Movement physics implementation
        double fx;
        double fy = JUMP ? JUMP_F : 0;
        
        if (!airborne()) {
            // Ground movement controls
            if (RIGHT) {
                vx = VEL_X;
            } if (LEFT) {
                vx = -VEL_X;
            } else if (!RIGHT && !LEFT) {
                vx = 0;
            }
            fx = -K_X*(velocity().x-vx);
        } else {
            // Air movement controls (reduced control)
            if (RIGHT) {
                vx = AIR_CONTROL_FACTOR * VEL_X;
            } if (LEFT) {
                vx = -AIR_CONTROL_FACTOR * VEL_X;
            }            
            fx = -K_X*(velocity().x-vx);
        }
        
        // Apply calculated forces
        omni_apply_force(fx, G+fy);
        JUMP = false;
    }

    void stop() {}
    
    private:
    // Robot position tracking
    double initial_x, initial_y; 
    
    // Movement state flags
    bool LEFT, RIGHT, JUMP;
    double vx;
    
    // Physics constants
    const double VEL_X;            // Horizontal movement speed
    double JUMP_F;                 // Current jump force
    const double K_X;              // Horizontal movement dampening constant
    double G;                      // Gravity force
    const double H_MIN;            // Minimum height to be considered airborne
    const double NORMAL_JUMP_FORCE;  // Standard jump force
    const double CHEAT_JUMP_FORCE;   // Enhanced jump force for cheat mode
    const double BOUNCER_FORCE;      // Force applied by bouncer
    const double AIR_CONTROL_FACTOR; // Reduced movement control while airborne
    const double RESET_ROTATION;     // Rotation value when resetting position
    
    // Control key definitions
    const std::string KEY_LEFT = "a";
    const std::string KEY_RIGHT = "d";
    const std::string KEY_JUMP = " ";
    
    // Button names
    const std::string BUTTON_RESET = "reset";
    const std::string BUTTON_CHEAT = "cheat";
    
    // Event names
    const std::string EVENT_ROBOT_START = "robot_start";
    
    // Default physics values
    const double DEFAULT_GRAVITY = 400;
    
    // Decorations
    const std::string EMPTY_DECORATION = "";
    const std::string CROWN_DECORATION = R"(<g>
        <path d="M-10,-25 L-7,-15 L-3,-23 L0,-15 L3,-23 L7,-15 L10,-25 L10,-20 L5,-17 L0,-18 L-5,-17 L-10,-20 Z" 
              style="fill:gold;stroke:goldenrod;stroke-width:1" />
        <circle cx="0" cy="-21" r="1.5" style="fill:red;stroke:none" />
        <circle cx="-5" cy="-20" r="1" style="fill:blue;stroke:none" />
        <circle cx="5" cy="-20" r="1" style="fill:green;stroke:none" />
    </g>)";
};

class Robot : public Agent {
    public:
    Robot(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    RobotController c;
};

DECLARE_INTERFACE(Robot)

#endif