#ifndef __TRACKER_AGENT__H
#define __TRACKER_AGENT__H 

#include "enviro.h"
#include <cmath> // 用于数学计算

using namespace enviro;

class TrackerController : public Process, public AgentInterface {

    public:
    TrackerController() : Process(), AgentInterface(),speed(40), robot_id(-1), tracking(false) {}

    void init() {
        prevent_rotation();
        watch("robot_start", [&](Event& e) {
            robot_id = e.value()["id"];
            // std::cout << "Tracker: Found Robot with ID " << robot_id << " from start event" << std::endl;
            tracking = true;
        });
        watch("button_click", [&](Event& e) {
            if (e.value()["value"] == "reset") {
                teleport(initial_x, initial_y,0); 
            }
        });
        decorate(R"(<g>
            <circle cx="-5" cy="-3" r="3" style="fill:red;stroke:none"/>
            <circle cx="5" cy="-3" r="3" style="fill:red;stroke:none"/>
            <path d="M-7,5 C-4,8 4,8 7,5" style="fill:none;stroke:black;stroke-width:1.5"/>
        </g>)");
        
        tracking = true;
    }

    void start() {
        
        initial_x = x();
        initial_y = y(); 
        
    }

    void update() {
        if (tracking && robot_id >= 0 && agent_exists(robot_id)) {
            try {
                
                Agent& robot = find_agent(robot_id);
                double robot_x = robot.x();
                double robot_y = robot.y();
                
                
                double dx = robot_x - x();
                double dy = robot_y - y();
                double distance = sqrt(dx*dx + dy*dy);
                
                
                // if (rand() % 100 == 0) { 
                //     std::cout << "Tracker: distance to Robot = " << distance 
                //               << ", dx=" << dx << ", dy=" << dy << std::endl;
                // }
                
                
                double current_speed = distance < 50 ? speed * 0.1 : speed * 0.15;
                
    
                double dir_x = dx / (distance == 0 ? 1 : distance);
                double dir_y = dy / (distance == 0 ? 1 : distance);
                
                
                omni_track_velocity(dir_x * current_speed, dir_y * current_speed, 5);
                
                
                if (distance < 0.5) {
                    emit(Event("tracker_collision", {{"robot_id", robot_id}}));
                     
                }
                
                if (distance < 100) {
                    std::string color = distance < 30 ? "darkred" : "red";
                    set_style({{"fill", color}, {"stroke", "black"}});
                } else {
                    set_style({{"fill", "crimson"}, {"stroke", "black"}});
                }
            } catch (std::exception& e) {
                std::cout << "Tracker: Error tracking Robot: " << e.what() << std::endl;
                tracking = false;
            }
        } else {
    
            omni_damp_movement();
        }
    }

    void stop() {}

    private:
    double speed;
    int robot_id; 
    bool tracking; 
    double initial_x, initial_y; 
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