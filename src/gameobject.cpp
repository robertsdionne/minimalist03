//
//  critter.cpp
//  minimalist01
//
//  Created by Robert Dionne on 2/26/13.
//
//

#include "gameobject.h"
#include "ofMain.h"

GameObject::GameObject(float mass, float area, ofVec2f position, ofVec2f velocity)
: mass(mass), area(area), position(position), previous_position(position - velocity / kFrameRate) {}

float GameObject::density() const {
  return mass / area;
}

float GameObject::radius() const {
  return sqrt(area / M_PI);
}

ofVec2f GameObject::velocity() const {
  return (position - previous_position) * kFrameRate;
}

void GameObject::Accelerate(float dt) {
  position += force / mass * dt * dt;
  force = ofVec2f();
}

void GameObject::Inertia() {
  const ofVec2f new_position = position * 2 - previous_position;
  previous_position = position;
  position = new_position;
}
