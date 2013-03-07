//
//  virus.cpp
//  minimalist01
//
//  Created by Robert Dionne on 2/27/13.
//
//

#include "gameobject.h"
#include "ofMain.h"
#include "virus.h"

const ofColor Virus::kColor = ofColor(255, 255, 255);
constexpr float Virus::kGrowthRate;
constexpr float Virus::kInfectionRate;

Virus::Virus()
: GameObject(1.0, kArea, ofVec2f(ofRandomWidth(), ofRandomHeight())) {}

void Virus::Draw() const {
  ofPushStyle();
  ofSetColor(kColor);
  ofCircle(position, radius());
  ofPopStyle();
}

void Virus::Update(float dt) {
  Accelerate(dt);
  Inertia(dt);
  velocity.x += kImpulse * ofRandomf();
  velocity.y += kImpulse * ofRandomf();
  Wrap();
  if (ofRandomuf() < kMortality) {
    area = 0;
  }
}

void Virus::Wrap() {
  if (position.x < 0) {
    position.x += ofGetWidth();
  }
  if (position.x >= ofGetWidth()) {
    position.x -= ofGetWidth();
  }
  if (position.y < 0) {
    position.y += ofGetHeight();
  }
  if (position.y >= ofGetHeight()) {
    position.y -= ofGetHeight();
  }
}
