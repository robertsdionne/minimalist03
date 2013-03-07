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
  ofPushMatrix();
  ofTranslate(position);
  ofPushStyle();
  ofEnableAlphaBlending();
  ofColor interior = ofColor::white;
  ofSetColor(interior, 128);
  ofFill();
  ofSetLineWidth(0);
  DrawInternal();
  ofDisableAlphaBlending();
  ofColor membrane = ofColor::white;
  ofSetColor(membrane / (21.0 / radius()));
  ofNoFill();
  ofSetLineWidth(area * kLineWidthScaleFactor);
  DrawInternal();
  ofPopStyle();
  ofPopMatrix();
}

void Food::DrawInternal() const {
  const float resolution = 4 * 10;
  ofBeginShape();
  for (unsigned int i = 0; i < resolution + 1; ++i) {
    ofVertex(radius() * cos(i * 2.0 *  M_PI / resolution), radius() * sin(i * 2.0 * M_PI / resolution));
  }
  ofEndShape();
}

void Food::Update(float dt) {
  velocity.x += kImpulse * ofRandomf();
  velocity.y += kImpulse * ofRandomf();
  Accelerate(dt);
  Inertia(dt);
  Wrap();
}

void Food::Wrap() {
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
