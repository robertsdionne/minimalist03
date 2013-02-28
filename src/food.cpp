//
//  food.cpp
//  minimalist01
//
//  Created by Robert Dionne on 2/27/13.
//
//

#include "food.h"

Food::Food()
: GameObject(1.0, kArea, ofVec2f(ofRandomWidth(), ofRandomHeight())) {}

void Food::Draw() const {
  ofPushStyle();
  ofSetColor(ofColor::yellow);
  ofCircle(position, radius());
  ofPopStyle();
}

void Food::Update(float dt) {
  Accelerate(dt);
  Inertia();
  position.x += kImpulse * ofRandomf();
  position.y += kImpulse * ofRandomf();
  Wrap();
}

void Food::Wrap() {
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
