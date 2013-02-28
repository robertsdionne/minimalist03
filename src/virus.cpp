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

const ofColor Virus::kColor = ofColor(227, 66, 52);
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
  Inertia();
  position.x += kImpulse * ofRandomf();
  position.y += kImpulse * ofRandomf();
  Wrap();
  if (ofRandomuf() < kMortality) {
    area = 0;
  }
}

void Virus::Wrap() {
  if (position.x < 0) {
    position.x += ofGetWidth();
    previous_position.x += ofGetWidth();
  }
  if (position.x >= ofGetWidth()) {
    position.x -= ofGetWidth();
    previous_position.x -= ofGetWidth();
  }
  if (position.y < 0) {
    position.y += ofGetHeight();
    previous_position.y += ofGetHeight();
  }
  if (position.y >= ofGetHeight()) {
    position.y -= ofGetHeight();
    previous_position.y -= ofGetHeight();
  }
}
