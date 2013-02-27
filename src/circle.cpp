//
//  circle.cpp
//  minimalist01
//
//  Created by Robert Dionne on 2/12/13.
//
//

#include "circle.h"
#include "ofMain.h"

const ofColor Circle::kMembraneColor = ofColor(255.0, 102.0, 102.0);
const ofColor Circle::kWallCellColor = ofColor(0.0, 0.0, 255.0);
const ofColor Circle::kInteriorCellColor = ofColor(128.0, 0, 128.0);

Circle::Circle(bool player, float mass, float size, float food, float orientation, ofVec2f position, ofVec2f velocity)
: GameObject(player, mass, size, food, orientation, position, velocity) {}

Circle::~Circle() {}

ofColor Circle::membrane_color() const {
  if (player) {
    return kMembraneColor;
  } else {
    return ofColor::white - kMembraneColor;
  }
}

ofColor Circle::wall_cell_color() const {
  if (player) {
    return kWallCellColor;
  } else {
    return ofColor::white - kWallCellColor;
  }
}

ofColor Circle::interior_cell_color() const {
  if (player) {
    return kInteriorCellColor;
  } else {
    return ofColor::white - kInteriorCellColor;
  }
}

float Circle::reproductivity() const {
  return kReproductivity;
}

float Circle::mortality() const {
  return kMortality;
}

void Circle::DrawInternal() const {
  ofBeginShape();
  for (unsigned int i = 0; i < 26; ++i) {
    ofVertex(cos(i * 2.0 *  M_PI / 25.0), sin(i * 2.0 * M_PI / 25.0));
  }
  ofEndShape();
}
