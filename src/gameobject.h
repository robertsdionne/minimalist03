//
//  gameobject.h
//  minimalist01
//
//  Created by Robert Dionne on 2/12/13.
//
//

#ifndef __minimalist01__gameobject__
#define __minimalist01__gameobject__

#include <iostream>
#include <list>
#include <set>

#include "ofMain.h"

class GameObject {
public:
  GameObject(bool player, float mass, float size, float food, float orientation, ofVec2f position, ofVec2f velocity);
  
  virtual ~GameObject();
  
  virtual ofColor membrane_color() const = 0;
  virtual ofColor wall_cell_color() const = 0;
  virtual ofColor interior_cell_color() const = 0;
  
  virtual float reproductivity() const = 0;
  virtual float mortality() const = 0;
  
  void Draw();
  
  virtual void DrawInternal() const = 0;
  
  virtual void MaybeReproduce(std::list<GameObject *> &group);

  void Update(float dt);
  
  virtual void UpdateInternal(float dt);
  
public:
  bool player;
  std::list<GameObject *> neighbors;
  std::set<GameObject *> connected;
  float parity;
  float food;
  float mass;
  float size;
  float age;
  float poison;
  float orientation;
  ofVec2f position;
  ofVec2f velocity;
  ofVec2f force;
  
  static constexpr unsigned int kMaxPopulation = 200;
  static constexpr float kMinSize = 2.0;
  static constexpr float kBreederSize = 10.0;
  static constexpr float kMaxSize = 20.0;
  
private:
  static constexpr float kChildScaleFactor = 0.8;
  static constexpr float kDrag = 0.9;
  static constexpr float kGrowthRate = 0.005;
  static constexpr float kAgeRate = 0.0005;
  static constexpr float kWallMortality = 0.00001;
  static constexpr float kLineWidthScaleFactor = 0.25;
  static constexpr float kMaxComponentOfVelocity = 10.0;};

#endif /* defined(__minimalist01__gameobject__) */
