//
//  wz3585.cpp
//  Simulation
//
//  Created by Wenwen on 3/21/16.
//  Copyright © 2016 wenwen. All rights reserved.
//
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

#include "wz3585.h"
#include "CraigUtils.h"
#include "Event.h"
#include "ObjInfo.h"
#include "Params.h"
#include "Random.h"
#include "Window.h"

#ifdef _MSC_VER
using namespace epl;
#endif
using namespace std;
using String = std::string;

Initializer<wz3585> __wz3585_initializer;
bool flag = false;
String wz3585::species_name(void) const
{
    if(flag)
        return "Algea";
    else
        if (health()*start_energy<=eat_cost_function()||health()*start_energy>=min_energy*1.1) {
            return "WenwenIgnoreMe";
        }
        else
            return "WenwenEatMe";
}
String wz3585::player_name(void) const{
    return "wz3585";
}

Action wz3585::encounter(const ObjInfo& info)
{
    if (info.species=="WenwenIgnoreMe") {
        /* don't be cannibalistic */
        set_course(info.bearing + M_PI);
        return LIFEFORM_IGNORE;
    }
    else{
        hunt_event->cancel();
        SmartPointer<wz3585> self = SmartPointer<wz3585>(this);
        hunt_event = new Event(0.0, [self](void) { self->hunt(); });
        return LIFEFORM_EAT;
    }
}

void wz3585::initialize(void) {
    LifeForm::add_creator(wz3585::create, "wz3585");
}

/*
 * REMEMBER: do not call set_course, set_speed, perceive, or reproduce
 * from inside the constructor!!!!
 * you must wait until the object is actually alive
 */
wz3585::wz3585() {
    SmartPointer<wz3585> self = SmartPointer<wz3585>(this);  //?????????????use this instead of self?
    new Event(0, [self](void) { self->startup(); });
}

wz3585::~wz3585() {}

void wz3585::startup(void) {
    if(drand48()<0.25){
        flag = true;
          set_speed(max_speed*0.1 * drand48());  
        set_course(drand48() * 2.0 * M_PI); 
        SmartPointer<wz3585> self = SmartPointer<wz3585>(this);
        hunt_event = new Event(0, [self](void) { self->hunt(); }); 

    }else{
    set_course(drand48() * 2.0 * M_PI);
    set_speed(2+5*drand48());
    SmartPointer<wz3585> self = SmartPointer<wz3585>(this);
        hunt_event = new Event(2.0*drand48(), [self](void) { self->hunt(); });
    }
}

void wz3585::spawn(void) {
    SmartPointer<wz3585> child = new wz3585;  // constructor, startup, hunt,??
    reproduce(child);
}


Color wz3585::my_color(void) const {
    return BLUE;
}

SmartPointer<LifeForm> wz3585::create(void) {
    return new wz3585;
}


void wz3585::hunt(void) {
    const String fav_food = "Algae";
    
        
  
    
    if (health() == 0.0) { return; } // we died
    
    double MyEnergy = health()*start_energy;
    double perceive_range;
    double cost = perceive_cost(max_perceive_range);
    // if(cost<MyEnergy*0.03)
    //     perceive_range = max_perceive_range;
    // else if(cost<MyEnergy*0.1)
        perceive_range = max_perceive_range/2;
    ///else
       // perceive_range = min_perceive_range+(max_perceive_range-min_perceive_range)*0.1;
    ObjList prey = perceive(perceive_range);
    double best_d = HUGE;
    int count = 0;
    double speed = 0;
    double best_d_1 = HUGE;
   
    if (flag){   
          if(prey.size()<40) set_speed(max_speed*0.7); 
            for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {

                if ((*i).species == fav_food||"WenwenEatMe") {
                    if (best_d > (*i).distance) {
                        set_course((*i).bearing);
                        course_changed = 0;
                        best_d = (*i).distance;                       
                    }
                }
                else if((*i).species!="WenwenIgnoreMe"){
                    name = (*i).species;
                    if((*i).health>health()){
                        if(best_d_1>(*i).distance){
                    //      course_changed = 0;
                          count++;
                       //  set_course((*i).bearing+M_PI*0.5);  
                        }
                        
                    }else{
                          if (best_d > (*i).distance) {
                            best_d = (*i).distance;
                            course_changed = 0;
                            set_course((*i).bearing);
                            if(speed<(*i).their_speed)
                                speed = (*i).their_speed;
                        }

                    }
                   
                }
            }

        
    }else{
        
        for (ObjList::iterator i = prey.begin(); i != prey.end(); ++i) {
            if ((*i).species==fav_food||"WenwenEatMe") {
                if (best_d > (*i).distance) {
                    set_course((*i).bearing);
                    course_changed = 0;
                    best_d = (*i).distance;
                  
                }
            }else if((*i).species == "WenwenIgnoreMe"&&species_name()=="WenwenEatMe"){
                set_speed(0);
                course_changed = -1;
                break;
             }
             else if((*i).species!="WenwenIgnoreMe"){
                name = (*i).species;
                    if((*i).health>health()){
                        if(best_d_1>(*i).distance){
                        //  course_changed = 0;
                          count++;
                       //  set_course((*i).bearing+M_PI*0.5);  
                        }
                    }else{
                        
                        if (best_d > (*i).distance) {
                        best_d = (*i).distance;
                        course_changed = 0;
                        set_course((*i).bearing);
                        }
                    }
                   
            }
        }
    }
    if(course_changed==-1) return;
    if(best_d==HUGE) {
        if(course_changed==0){
            course_changed = 1;
            set_course(get_course()+M_PI*0.5);
        }else if(course_changed==1){
            course_changed = 2;
            set_course(get_course()+M_PI*0.5);
        }else if(course_changed ==2){
            course_changed = 3;
            set_course(get_course()+M_PI*0.5);
        }else{
            set_course(get_course()+M_PI*0.5);
        }

    }
    if (count<20) {
         if (health() >= 1.5&&(1-reproduce_cost)*MyEnergy>min_energy*1.1) {
            spawn();
       //  if(flag) set_speed(speed*1.2);
     }
    }else{
         if (health() >= 4.0&&(1-reproduce_cost)*MyEnergy>min_energy*1.1) spawn();
    }
    hunt_event = nullptr;
    SmartPointer<wz3585> self = SmartPointer<wz3585>(this);
    hunt_event = new Event(10.0, [self](void) { self->hunt(); });
    
    
}
