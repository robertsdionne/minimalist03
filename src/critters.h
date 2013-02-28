#pragma once

#include <limits>
#include <list>
#include <map>

#include "critter.h"
#include "food.h"
#include "ofMain.h"
#include "virus.h"

class Critters : public ofBaseApp {
public:
  void setup();
  
  void update();
  
  void draw();
  
  void keyPressed(int key);
  
  void keyReleased(int key);
  
  void mouseMoved(int x, int y);
  
  void mouseDragged(int x, int y, int button);
  
  void mousePressed(int x, int y, int button);
  
  void mouseReleased(int x, int y, int button);
  
  void windowResized(int width, int height);
  
  void dragEvent(ofDragInfo dragInfo);
  
  void gotMessage(ofMessage msg);
  
private:
  struct Statistic {
    Statistic()
    : min(std::numeric_limits<float>::infinity()),
      max(-std::numeric_limits<float>::infinity()),
      total(0),
      mean(0),
      variance(0) {}
    float min;
    float max;
    float total;
    float mean;
    float variance;
  };
  
  struct Statistics {
    Statistics() : overlap(), food() {}
    Statistic overlap;
    Statistic food;
  };
  
  void Collide(std::list<Critter *> &group, Statistics &statistics);
  void Collide(std::list<Critter *> &critters, std::list<Critter *> &enemy_critters);
  void CollideFood(std::list<Critter *> &group, std::list<Food *> &food);
  void CollideVirii(std::list<Critter *> &group, std::list<Virus *> &virii);
  ofVec2f FindCenterOfMass(std::list<Critter *> &group);
  void CreateShape(std::list<Critter *> &group, bool player, ofVec2f at);
  void DrawGroup(std::list<Critter *> &group) const;
  void DrawGroup(std::list<Food *> &group) const;
  void DrawGroup(std::list<Virus *> &group) const;
  void RemoveDeadFood(std::list<Food *> &group);
  void RemoveDeadVirii(std::list<Virus *> &group);
  void RemoveDeadIndividuals(std::list<Critter *> &group);
  void SteerGroup(std::list<Critter *> &group, ofVec2f target);
  void UpdateGroup(std::list<Critter *> &group, Statistics &statistics, ofVec2f target, bool move, bool player);
  void UpdateFood(std::list<Food *> &group);
  void UpdateVirii(std::list<Virus *> &group);
  void Launch(std::list<Critter *> &group);
  void Wrap(ofVec2f &position);
  
private:
  static constexpr unsigned int kNumFood = 100;
  static constexpr unsigned int kNumCritters = 50;
  static constexpr unsigned int kNumVirii = 100;
  static constexpr float kOverlap = 0.5;
  
  std::list<Food *> food;
  std::list<Critter *> critters;
  std::list<Critter *> enemy_critters;
  std::list<Virus *> virii;
  
  float enemy_target_angle;
  ofVec2f mouse_position;
  ofVec2f enemy_target;
  ofVec2f enemy_center_of_mass;
  unsigned int reproduce_type;
  
  Statistics statistics;
  Statistics enemy_statistics;
  
  bool debug;
  std::map<int, bool> keys;
  std::map<int, bool> previous_keys;
};
