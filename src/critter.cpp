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
#include "virus.h"

const ofColor Critter::kMembraneColor = ofColor(255.0, 102.0, 102.0);
const ofColor Critter::kWallCellColor = ofColor(0.0, 0.0, 255.0);
const ofColor Critter::kInteriorCellColor = ofColor(128.0, 0, 128.0);

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
: GameObject(mass, area, position, velocity), player(player), neighbors(), food(food), infection(0), immunity(0), age(0), parity(ofRandomuf() < 0.5 ? -1.0 : 1.0) {}

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
  ofBeginShape();
  for (unsigned int i = 0; i < 26; ++i) {
    ofVertex(cos(i * 2.0 *  M_PI / 25.0), sin(i * 2.0 * M_PI / 25.0));
  }
  ofEndShape();
}


void Critter::Draw() const {
  ofPushStyle();
  ofSetColor(ofColor::white, 128);
  std::for_each(connected.begin(), connected.end(), [&] (Critter *const neighbor) {
    ofLine(position, neighbor->position);
  });
  ofPopStyle();
  ofPushMatrix();
  ofTranslate(position);
  ofScale(radius(), radius());
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
  membrane.setSaturation(255 * (1-age * age * age));
  ofSetColor(membrane / (21.0 / radius()));
  ofNoFill();
  ofSetLineWidth(area * kLineWidthScaleFactor);
  DrawInternal();
  ofPopStyle();
  ofPopMatrix();
  ofPushMatrix();
  ofPushStyle();
  ofTranslate(position);
  if (infection > 0) {
    Virus virus;
    virus.area = infection * Virus::kAreaToVirus;
    ofSetColor(ofColor::white);
    ofFill();
    ofCircle(0, 0, virus.radius());
  }
  ofPopStyle();
  ofPopMatrix();
}

void Critter::MaybeReproduce(std::list<Critter *> &group, std::list<Virus *> &virii) {
  if (radius() <= kBreederSize && food >= 0.5 && ofRandomuf() < reproductivity() && group.size() < kMaxPopulation) {
    area *= kChildScaleFactor;
    food -= 0.5;
    age = 0;
    infection /= 2.0;
    const ofVec2f epsilon = ofVec2f(0.1, 0.1);
    Critter *critter = new Critter(player, 0, mass, area, position + epsilon, velocity());
    critter->infection = infection;
    group.push_back(critter);
  }
  const float cell_mortality = radius() <= kBreederSize ? mortality() : kWallMortality;
  if (ofRandomuf() < cell_mortality * age * age * age) {
    area = 0;
  }
  if (area <= infection * Virus::kAreaToVirus) {
    area = 0;
    for (int i = 0; i < infection; ++i) {
      Virus *virus = new Virus();
      virus->position = position;
      virus->previous_position = previous_position;
      virii.push_back(virus);
    }
  }
}

void Critter::Update(float dt) {
  UpdateInternal(dt);
  Accelerate(dt);
  Inertia();
}

void Critter::UpdateInternal(float dt) {
  std::for_each(connected.begin(), connected.end(), [&] (Critter *const neighbor) {
    const ofVec2f r = position - neighbor->position;
    const float actual_distance = r.length();
    const float colliding_distance = radius() + neighbor->radius();
    if ((radius() > 12.0 && neighbor->radius() > 12.0 && actual_distance > colliding_distance * 8.0)
        || actual_distance > colliding_distance * 1.3) {
      connected.erase(neighbor);
    }
  });
  std::for_each(neighbors.begin(), neighbors.end(), [&] (Critter *const neighbor) {
    if ((parity == neighbor->parity && connected.size() < 2) || (radius() > 12.0)) {
      connected.insert(neighbor);
    }
  });
  if (radius() < kBreederSize - kGrowthRate) {
    area += kGrowthRate * ofRandomuf();
  }
  if (age <= 1.0 - kAgeRate) {
    age += kAgeRate * ofRandomuf();
  }
  if (infection > 0 && infection < 1.0 && immunity < 1.0 - kImmunityGrowthRate) {
    immunity += kImmunityGrowthRate * ofRandomuf();
  }
  if (infection >= 1.0 && ofRandomuf() < Virus::kGrowthRate * age * age * age - immunity) {
    infection += 1;
  }
  force += -kDrag * velocity();
}
