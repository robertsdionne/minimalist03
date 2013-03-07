//
//  Critter.h
//  minimalist01
//
//  Created by Robert Dionne on 2/12/13.
//
//

#ifndef __minimalist01__critter__
#define __minimalist01__critter__

#include <list>
#include <set>

#include "gameobject.h"
#include "ofMain.h"

class Critter : public GameObject {
public:
  Critter(bool player, float food, float mass = kMass, float area = kArea, ofVec2f position = ofVec2f(), ofVec2f velocity = ofVec2f());
  
  virtual ~Critter() {}
  
  virtual bool attacker() const;
  virtual ofColor membrane_color() const;
  virtual ofColor wall_cell_color() const;
  virtual ofColor interior_cell_color() const;
  
  virtual float reproductivity() const;
  virtual float mortality() const;
  
  virtual void Draw() const;
  
  virtual void DrawInternal() const;
  
  virtual void MaybeReproduce(std::list<Critter *> &group);

  void Update(float dt);
  
  virtual void UpdateInternal(float dt);
  
public:
  bool player;
  std::list<Critter *> neighbors;
  float parity;
  float food;
  float age;
  float orientation;
  float orientation_speed;
  bool is_attacker;
  
  static constexpr float kAgeRate = 0.0005;
  static constexpr float kAttackChance = 0.01;
  static constexpr float kBreederSize = 10.0;
  static constexpr float kChildScaleFactor = 0.8;
  static constexpr float kDrag = 0.9;
  static constexpr float kGrowthRate = 0.1;
  static const ofColor kInteriorCellColor;
  static constexpr float kLineWidthScaleFactor = 0.004;
  static constexpr float kMaxComponentOfVelocity = 10.0;
  static constexpr unsigned int kMaxPopulation = 200;
  static constexpr float kMaxSize = 20.0;
  static const ofColor kMembraneColor;
  static constexpr float kMinSize = 2.0;
  static constexpr float kMortality = 0.0001;
  static constexpr float kReproductivity = 0.001;
  static const ofColor kWallCellColor;
  static constexpr float kWallMortality = 0.00002;
  static constexpr float kWallSize = 12.0;
};

#endif /* defined(__minimalist01__critter__) */
