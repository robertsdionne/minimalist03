//
//  critter.h
//  minimalist01
//
//  Created by Robert Dionne on 2/26/13.
//
//

#ifndef __minimalist01__gameobject__
#define __minimalist01__gameobject__

#include <iostream>
#include "ofMain.h"

class GameObject {
public:
  static constexpr float kDeltaTime = 1.0 / 60.0;
  static constexpr float kFrameRate = 60.0;
  static constexpr float kMass = 1.0;
  static constexpr float kRadius = 10.0;
  
  GameObject(float mass = kMass, float radius = kRadius, ofVec2f position = ofVec2f(), ofVec2f velocity = ofVec2f());
  
  virtual ~GameObject() {}
  
  ofVec2f velocity() const;
  
  void Accelerate(float dt);
  
  void Inertia();
  
  float mass;
  float radius;
  ofVec2f position;
  ofVec2f previous_position;
  ofVec2f force;
};

#endif /* defined(__minimalist01__gameobject__) */
