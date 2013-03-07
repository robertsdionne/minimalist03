//
//  Critter.cpp
//  minimalist01
//
//  Created by Robert Dionne on 2/12/13.
//
//

#include <list>

#include "critter.h"
#include "ofMain.h"

const ofColor Critter::kMembraneColor = ofColor(255.0, 102.0, 102.0);
const ofColor Critter::kWallCellColor = ofColor(0.0, 0.0, 255.0);
const ofColor Critter::kInteriorCellColor = ofColor(128.0, 0, 128.0);

constexpr float Critter::kAttackChance;
constexpr unsigned int Critter::kMaxPopulation;
constexpr float Critter::kMinSize;
constexpr float Critter::kBreederSize;
constexpr float Critter::kMaxSize;
constexpr float Critter::kChildScaleFactor;
constexpr float Critter::kDrag;
constexpr float Critter::kGrowthRate;
constexpr float Critter::kLineWidthScaleFactor;
constexpr float Critter::kMaxComponentOfVelocity;

Critter::Critter(bool player, float food, float mass, float area, ofVec2f position, ofVec2f velocity)
: GameObject(mass, area, position, velocity), player(player), neighbors(), food(food), age(0),
  parity(ofRandomuf() < 0.5 ? -1.0 : 1.0), orientation(0), orientation_speed(0) {}

bool Critter::attacker() const {
  return radius() > kWallSize && parity > 0;
}

ofColor Critter::membrane_color() const {
  if (player) {
    return kMembraneColor;
  } else {
    return ofColor::white - kMembraneColor;
  }
}

ofColor Critter::wall_cell_color() const {
  if (player) {
    return kWallCellColor;
  } else {
    return ofColor::white - kWallCellColor;
  }
}

ofColor Critter::interior_cell_color() const {
  if (player) {
    return kInteriorCellColor;
  } else {
    return ofColor::white - kInteriorCellColor;
  }
}

float Critter::reproductivity() const {
  return kReproductivity;
}

float Critter::mortality() const {
  return kMortality;
}

void Critter::DrawInternal() const {
  const float resolution = 4 * 10;
  ofBeginShape();
  for (unsigned int i = 0; i < resolution + 1; ++i) {
    if (attacker()) {
      if (i % 4 == 0) {
        const float attacker_radius = kWallSize + 1.0 + 2.0 * sqrt(radius() - kWallSize);
        ofVertex(attacker_radius * cos(i * 2.0 *  M_PI / resolution), attacker_radius * sin(i * 2.0 * M_PI / resolution));
      } else {
        ofVertex(kWallSize * cos(i * 2.0 *  M_PI / resolution), kWallSize * sin(i * 2.0 * M_PI / resolution));
      }
    } else {
      ofVertex(radius() * cos(i * 2.0 *  M_PI / resolution), radius() * sin(i * 2.0 * M_PI / resolution));
    }
  }
  ofEndShape();
}


void Critter::Draw() const {
  ofPushMatrix();
  ofTranslate(position);
  ofRotate(ofRadToDeg(orientation));
  ofPushStyle();
  if (food > 0) {
    ofEnableAlphaBlending();
    ofColor interior = interior_cell_color().getLerped(wall_cell_color(), (radius() - 10) / 10.0);
    ofSetColor(interior, 255 * food);
    ofFill();
    ofSetLineWidth(0);
    DrawInternal();
    ofDisableAlphaBlending();
  }
  ofColor membrane = membrane_color();
  membrane.setSaturation(255 * (1 - age * age * age));
  ofSetColor(membrane / (21.0 / radius()));
  ofNoFill();
  ofSetLineWidth(area * kLineWidthScaleFactor);
  DrawInternal();
  ofPopStyle();
  ofPopMatrix();
}

void Critter::MaybeReproduce(std::list<Critter *> &group) {
  if (radius() <= kBreederSize && food >= 0.5 && ofRandomuf() < reproductivity() && group.size() < kMaxPopulation) {
    area *= kChildScaleFactor;
    food -= 0.5;
    age = 0;
    const ofVec2f epsilon = ofVec2f(0.1, 0.1);
    Critter *critter = new Critter(player, 0, mass, area, position + epsilon, velocity);
    group.push_back(critter);
  }
  const float cell_mortality = radius() <= kBreederSize ? mortality() : kWallMortality;
  if (ofRandomuf() < cell_mortality * age * age * age) {
    area = 0;
  }
}

void Critter::Update(float dt) {
  UpdateInternal(dt);
  Accelerate(dt);
  Inertia(dt);
}

void Critter::UpdateInternal(float dt) {
  if (radius() < kBreederSize - kGrowthRate) {
    area += kGrowthRate * ofRandomuf();
  }
  if (age <= 1.0 - kAgeRate) {
    age += kAgeRate * ofRandomuf();
  }
  force += -kDrag * velocity;
  orientation_speed *= 0.99;
  orientation += orientation_speed * dt;
}
