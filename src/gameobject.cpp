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
: mass(mass), area(area), position(position), velocity(velocity) {}

float GameObject::density() const {
  return mass / area;
}

float GameObject::radius() const {
  return sqrt(area / M_PI);
}

void GameObject::Accelerate(float dt) {
  velocity += force / mass * dt;
  force = ofVec2f();
}

void GameObject::Inertia(float dt) {
  position += velocity * dt;
}
