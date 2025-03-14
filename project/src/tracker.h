#ifndef __TRACKER_AGENT__H
#define __TRACKER_AGENT__H 

#include "enviro.h"
#include <cmath> 

using namespace enviro;

class TrackerController : public Process, public AgentInterface {

    public:
    // Initialize controller with default values
    TrackerController() : Process(), AgentInterface(),
                         TRACKING_SPEED(40),
                         CLOSE_RANGE(50),
                         CLOSE_FACTOR(0.1),
                         NORMAL_FACTOR(0.15),
                         VISUAL_RANGE(100),
                         DANGER_RANGE(30),
                         COLLISION_THRESHOLD(0.5),
                         robot_id(-1), 
                         tracking(false) {}

    void init() {
        prevent_rotation();
        
        // Listen for robot startup event to get its ID
        watch("robot_start", [&](Event& e) {
            robot_id = e.value()["id"];
            tracking = true;
        });
        
        // Reset position when reset button is clicked
        watch("button_click", [&](Event& e) {
            if (e.value()["value"] == "reset") {
                teleport(initial_x, initial_y, 0); 
            }
        });
        
        // Add visual decoration (angry eyes and mouth)
        decorate(R"(<g>
            <circle cx="-5" cy="-3" r="3" style="fill:red;stroke:none"/>
            <circle cx="5" cy="-3" r="3" style="fill:red;stroke:none"/>
            <path d="M-7,5 C-4,8 4,8 7,5" style="fill:none;stroke:black;stroke-width:1.5"/>
        </g>)");
        
        tracking = true;
    }

    void start() {
        // Store initial position for reset functionality
        initial_x = x();
        initial_y = y(); 
    }

    void update() {
        if (tracking && robot_id >= 0 && agent_exists(robot_id)) {
            try {
                // Get robot position
                Agent& robot = find_agent(robot_id);
                double robot_x = robot.x();
                double robot_y = robot.y();
                
                // Calculate distance to robot
                double dx = robot_x - x();
                double dy = robot_y - y();
                double distance = sqrt(dx*dx + dy*dy);
                
                // Adjust speed based on distance
                double current_speed = distance < CLOSE_RANGE ? 
                                      TRACKING_SPEED * CLOSE_FACTOR : 
                                      TRACKING_SPEED * NORMAL_FACTOR;
                
                // Calculate direction vector
                double dir_x = dx / (distance == 0 ? 1 : distance);
                double dir_y = dy / (distance == 0 ? 1 : distance);
                
                // Apply movement toward robot
                omni_track_velocity(dir_x * current_speed, dir_y * current_speed, 5);
                
                // Check for collision with robot
                if (distance < COLLISION_THRESHOLD) {
                    emit(Event("tracker_collision", {{"robot_id", robot_id}}));
                }
                
                // Update visual appearance based on distance
                if (distance < VISUAL_RANGE) {
                    std::string color = distance < DANGER_RANGE ? "darkred" : "red";
                    set_style({{"fill", color}, {"stroke", "black"}});
                } else {
                    set_style({{"fill", "crimson"}, {"stroke", "black"}});
                }
            } catch (std::exception& e) {
                tracking = false;
            }
        } else {
            // Slow down if not tracking
            omni_damp_movement();
        }
    }

    void stop() {}

    private:
    // Tracking constants
    const double TRACKING_SPEED;    // Base movement speed
    const double CLOSE_RANGE;       // Distance threshold for speed adjustment
    const double CLOSE_FACTOR;      // Speed multiplier when close to target
    const double NORMAL_FACTOR;     // Speed multiplier when far from target
    const double VISUAL_RANGE;      // Range for visual color change
    const double DANGER_RANGE;      // Range for danger color indication
    const double COLLISION_THRESHOLD; // Distance threshold for collision detection
    
    // State variables
    int robot_id;                  // ID of the robot to track
    bool tracking;                 // Flag indicating active tracking
    double initial_x, initial_y;   // Initial position for reset
};

class Tracker : public Agent {
    public:
    Tracker(json spec, World& world) : Agent(spec, world) {
        add_process(c);
    }
    private:
    TrackerController c;
};

DECLARE_INTERFACE(Tracker)

#endif