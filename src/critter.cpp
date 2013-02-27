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

constexpr unsigned int Critter::kMaxPopulation;
constexpr float Critter::kMinSize;
constexpr float Critter::kBreederSize;
constexpr float Critter::kMaxSize;
constexpr float Critter::kChildScaleFactor;
constexpr float Critter::kDrag;
constexpr float Critter::kGrowthRate;
constexpr float Critter::kLineWidthScaleFactor;
constexpr float Critter::kMaxComponentOfVelocity;

Critter::Critter(
    bool player, float mass, float size, float food, float orientation, ofVec2f position, ofVec2f velocity)
: player(player), neighbors(), food(food), mass(mass), size(size), poison(0), age(0), orientation(orientation), position(position), velocity(velocity), force() {
  parity = ofRandomuf() < 0.5 ? -1.0 : 1.0;
}

Critter::~Critter() {}

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


void Critter::Draw() {
  //  ofSetColor(ofColor::gray);
  //  std::for_each(neighbors.begin(), neighbors.end(), [&] (Critter *const neighbor) {
  //    ofLine(position, neighbor->position);
  //  });
  if (!player) {
    ofPushStyle();
    ofSetColor(ofColor::white);
    std::for_each(connected.begin(), connected.end(), [&] (Critter *const neighbor) {
      ofLine(position, neighbor->position);
    });
  }
  ofPopStyle();
  ofPushMatrix();
  ofTranslate(position);
  ofScale(size, size);
  ofRotateZ(ofRadToDeg(orientation));
  ofPushStyle();
  if (food > 0) {
    ofEnableAlphaBlending();
    ofSetColor(interior_cell_color().getLerped(wall_cell_color(), (size - 10) / 10.0), 255 * food);
    ofFill();
    ofSetLineWidth(0);
    DrawInternal();
    ofDisableAlphaBlending();
  }
  ofColor membrane = membrane_color();
  membrane.setSaturation(255 * (1-age * age * age));
//  if (parity < 0) {
//    membrane.setHue(std::fmod(membrane.getHue() + 50.0, 255.0));
//  }
  ofSetColor(membrane / (21.0 / size));
  ofNoFill();
  ofSetLineWidth(size * kLineWidthScaleFactor);
  DrawInternal();
  ofPopStyle();
  ofPopMatrix();
  if (player) {
    ofPushStyle();
    ofSetColor(ofColor::white);
    std::for_each(connected.begin(), connected.end(), [&] (Critter *const neighbor) {
      ofLine(position, neighbor->position);
    });
    ofPopStyle();
  }
}

void Critter::MaybeReproduce(std::list<Critter *> &group) {
  if (size <= kBreederSize && food >= 0.5 && ofRandomuf() < reproductivity() && group.size() < kMaxPopulation) {
    size *= kChildScaleFactor;
    food -= 0.5;
    age = 0;
    const ofVec2f epsilon = ofVec2f(0.1, 0.1);
    group.push_back(new Critter(player, mass, size, 0, orientation, position + epsilon, velocity));
  }
  const float cell_mortality = size <= kBreederSize ? mortality() : kWallMortality;
  if (ofRandomuf() < cell_mortality * age * age * age) {
    size = 0;
  }
}

void Critter::Update(float dt) {
  UpdateInternal(dt);
  position += velocity * dt;
  velocity += force / mass * dt;
  force = ofVec2f();
}

void Critter::UpdateInternal(float dt) {
  std::for_each(connected.begin(), connected.end(), [&] (Critter *const neighbor) {
    const ofVec2f r = position - neighbor->position;
    const float actual_distance = r.length();
    const float colliding_distance = size + neighbor->size;
    if ((size > 12.0 && neighbor->size > 12.0 && actual_distance > colliding_distance * 8.0)
        || actual_distance > colliding_distance * 1.3) {
      connected.erase(neighbor);
    }
  });
  std::for_each(neighbors.begin(), neighbors.end(), [&] (Critter *const neighbor) {
    if ((parity == neighbor->parity && connected.size() < 2) || (size > 12.0 && neighbor->size > 12.0)) {
      connected.insert(neighbor);
    }
  });
  orientation += ofDegToRad(ofRandomf());
  if (size < kBreederSize - kGrowthRate) {
    size += kGrowthRate * ofRandomuf();
  }
  if (age <= 1.0 - kAgeRate) {
    age += kAgeRate * ofRandomuf();
  }
  force += -kDrag * velocity;
}
