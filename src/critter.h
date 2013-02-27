//
//  Critter.h
//  minimalist01
//
//  Created by Robert Dionne on 2/12/13.
//
//

#ifndef __minimalist01__critter__
#define __minimalist01__critter__

#include <iostream>
#include <list>
#include <set>

#include "ofMain.h"

class Critter {
public:
  Critter(bool player, float mass, float size, float food, float orientation, ofVec2f position, ofVec2f velocity);
  
  virtual ~Critter();
  
  virtual ofColor membrane_color() const;
  virtual ofColor wall_cell_color() const;
  virtual ofColor interior_cell_color() const;
  
  virtual float reproductivity() const;
  virtual float mortality() const;
  
  void Draw();
  
  virtual void DrawInternal() const;
  
  virtual void MaybeReproduce(std::list<Critter *> &group);

  void Update(float dt);
  
  virtual void UpdateInternal(float dt);
  
public:
  bool player;
  std::list<Critter *> neighbors;
  std::set<Critter *> connected;
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
  
  static constexpr float kAgeRate = 0.0005;
  static constexpr float kBreederSize = 10.0;
  static constexpr float kChildScaleFactor = 0.8;
  static constexpr float kDrag = 0.9;
  static constexpr float kGrowthRate = 0.005;
  static const ofColor kInteriorCellColor;
  static constexpr float kLineWidthScaleFactor = 0.25;
  static constexpr float kMaxComponentOfVelocity = 10.0;
  static constexpr unsigned int kMaxPopulation = 200;
  static constexpr float kMaxSize = 20.0;
  static const ofColor kMembraneColor;
  static constexpr float kMinSize = 2.0;
  static constexpr float kMortality = 0.002;
  static constexpr float kReproductivity = 0.001;
  static const ofColor kWallCellColor;
  static constexpr float kWallMortality = 0.00001;
};

#endif /* defined(__minimalist01__critter__) */
