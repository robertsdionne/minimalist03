//
//  critter.h
//  minimalist01
//
//  Created by Robert Dionne on 2/26/13.
//
//

#ifndef __minimalist01__gameobject__
#define __minimalist01__gameobject__

#include "ofMain.h"

class GameObject {
public:
  static constexpr float kArea = M_PI * 100.0;
  static constexpr float kDeltaTime = 1.0 / 60.0;
  static constexpr float kFrameRate = 60.0;
  static constexpr float kMass = 1.0;
  
  GameObject(float mass = kMass, float area = kArea, ofVec2f position = ofVec2f(), ofVec2f velocity = ofVec2f());
  
  virtual ~GameObject() {}
  
  float density() const;
  
  float radius() const;
  
  void Accelerate(float dt);
  
  virtual void Draw() const = 0;
  
  void Inertia(float dt);
  
  float mass;
  float area;
  ofVec2f position;
  ofVec2f velocity;
  ofVec2f force;
};

#endif /* defined(__minimalist01__gameobject__) */
