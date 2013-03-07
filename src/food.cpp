//
//  food.cpp
//  minimalist01
//
//  Created by Robert Dionne on 2/27/13.
//
//

#include "food.h"

void Food::RandomSpawn() {
  const float choice = ofRandomuf();
  if (choice < 0.25) {
    position = ofVec2f(ofRandomWidth(), 0.0);
    velocity = ofVec2f(0, kSpawnSpeed);
  } else if (choice < 0.5) {
    position = ofVec2f(ofRandomWidth(), ofGetHeight());
    velocity = ofVec2f(0, -kSpawnSpeed);
  } else if (choice < 0.75) {
    position = ofVec2f(0.0, ofRandomHeight());
    velocity = ofVec2f(kSpawnSpeed, 0);
  } else {
    position = ofVec2f(ofGetWidth(), ofRandomHeight());
    velocity = ofVec2f(-kSpawnSpeed, 0);
  }
}

Food::Food()
: GameObject(1.0, kArea) {
  RandomSpawn();
}

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
  if (position.x < 0 || position.x >= ofGetWidth() ||
      position.y < 0 || position.y >= ofGetHeight()) {
    RandomSpawn();
  }
}
