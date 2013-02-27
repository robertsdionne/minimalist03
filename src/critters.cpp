#include "critter.h"
#include "critters.h"
#include "food.h"
#include "virus.h"

constexpr unsigned int Critters::kNumCritters;

void Critters::setup() {
  ofSetFrameRate(60.0);
  ofEnableSmoothing();
  mouse_position = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);
  reproduce_type = 0;
  enemy_target_angle = 0;
  for (unsigned int i = 0; i < kNumCritters; ++i) {
    CreateShape(critters, true, ofVec2f(ofRandomWidth(), ofRandomHeight()));
    CreateShape(enemy_critters, false, ofVec2f(ofRandomWidth(), ofRandomHeight()));
  }
  for (unsigned int i = 0; i < kNumFood; ++i) {
    food.push_back(new Food());
  }
  for (unsigned int i = 0; i < kNumVirii; ++i) {
    virii.push_back(new Virus());
  }
  debug = false;
}

void Critters::update() {
  UpdateGroup(critters, statistics, mouse_position, statistics.overlap.mean < kOverlap, true);
  enemy_target_angle += ofSignedNoise(ofGetElapsedTimef() / 5.0) * 0.05;
  const float radius = ofGetHeight() / 3.0;
  enemy_center_of_mass = FindCenterOfMass(enemy_critters);
  enemy_target = ofVec2f(radius * cos(enemy_target_angle), radius * sin(enemy_target_angle)) + enemy_center_of_mass;
  Wrap(enemy_target);
  UpdateFood(food);
  UpdateVirii(virii);
  UpdateGroup(enemy_critters, enemy_statistics, enemy_target, enemy_statistics.overlap.mean < kOverlap, false);
  RemoveDeadFood(food);
  RemoveDeadVirii(virii);
  RemoveDeadIndividuals(critters);
  RemoveDeadIndividuals(enemy_critters);
  CollideVirii(critters, virii);
  CollideVirii(enemy_critters, virii);
  CollideFood(critters, food);
  CollideFood(enemy_critters, food);
  if (keys['`'] && !previous_keys['`']) {
    debug = !debug;
  }
  previous_keys = keys;
}

void Critters::UpdateGroup(std::list<Critter *> &group, Statistics &statistics, ofVec2f target, bool move, bool player) {
  if (move) {
    SteerGroup(group, target);
  }
  Collide(group, statistics);
  if (player &&  keys[' '] && !previous_keys[' ']) {
    Launch(group);
  }
  for (auto individual : group) {
    individual->Update(1.0 / ofGetFrameRate());
    if (individual->food >= 0.0001) {
      individual->food -= 0.0001;
    }
    individual->MaybeReproduce(group, virii);
  }
}

void Critters::UpdateFood(std::list<Food *> &group) {
  for (auto mote : group) {
    mote->Update(1.0 / ofGetFrameRate());
  }
}

void Critters::UpdateVirii(std::list<Virus *> &group) {
  for (auto virus : group) {
    virus->Update(1.0 / ofGetFrameRate());
  }
}

ofVec2f Critters::FindCenterOfMass(std::list<Critter *> &group) {
  ofVec2f center_of_mass;
  float total_mass;
  std::for_each(group.begin(), group.end(), [&center_of_mass, &total_mass] (Critter *const individual) {
    center_of_mass += individual->mass * individual->position;
    total_mass += individual->mass;
  });
  center_of_mass /= total_mass;
  return center_of_mass;
}

void Critters::Launch(std::list<Critter *> &group) {
  ofVec2f center_of_mass = FindCenterOfMass(group);
  std::list<Critter *>::iterator start = group.begin();
  std::advance(start, ofRandom(group.size()));
  Critter *individual = *start;
  while (true) {
    bool found_closer_neighbor = false;
    std::for_each(individual->neighbors.begin(), individual->neighbors.end(), [this, &individual, &found_closer_neighbor] (Critter *neighbor) {
      if ((mouse_position - neighbor->position).lengthSquared() < (mouse_position - individual->position).lengthSquared()) {
        individual = neighbor;
        found_closer_neighbor = true;
      }
    });
    if (!found_closer_neighbor) {
      ofVec2f desired_velocity = mouse_position - individual->position;
      desired_velocity.scale(100000.0);
      individual->force += desired_velocity - individual->velocity();
      break;
    }
  }
}

void Critters::Wrap(ofVec2f &position) {
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

void Critters::Collide(std::list<Critter *> &group, Statistics &statistics) {
  statistics = Statistics();
  std::for_each(group.begin(), group.end(), [&] (Critter *const individual0) {
    std::list<Critter *> overlapping;
    std::for_each(group.begin(), group.end(), [&] (Critter *const individual1) {
      if (individual0 != individual1) {
        const ofVec2f r = individual1->position - individual0->position;
        const float actual_distance = r.length();
        const float colliding_distance = individual0->radius() + individual1->radius();
        if (actual_distance < colliding_distance) {
          overlapping.push_back(individual1);
          const float overlap = colliding_distance - actual_distance;
          statistics.overlap.min = std::min(statistics.overlap.min, overlap);
          statistics.overlap.max = std::max(statistics.overlap.max, overlap);
          statistics.overlap.total += overlap / 2;
          individual0->force -= 10.0 * r.normalized() * sqrt(overlap);
          individual1->force += 10.0 * r.normalized() * sqrt(overlap);
        }
      }
    });
    const float size_diffusion_amount = overlapping.size();
    const float food_diffusion_amount = 0.01;
    const float virus_diffusion_amount = 1.0;
    individual0->neighbors = overlapping;
    std::for_each(overlapping.begin(), overlapping.end(), [&] (Critter *const individual1) {
      if (ofRandomuf() < 0.1 && individual0->radius() > Critter::kMinSize
          && individual1->radius() < Critter::kMaxSize) {
        individual0->area -= size_diffusion_amount;
        individual1->area += size_diffusion_amount;
      }
      if (ofRandomuf() < individual0->food && individual0->food > 0 && individual1->food < 1) {
        individual0->food -= food_diffusion_amount;
        individual1->food += food_diffusion_amount;
      }
    });
    std::for_each(individual0->connected.begin(), individual0->connected.end(), [&] (Critter *const individual1) {
      if (ofRandomuf() < Virus::kInfectionRate && individual0->infection && individual0->infection >= 2) {
        individual0->infection -= virus_diffusion_amount;
        individual1->infection += virus_diffusion_amount;
      }
    });
    statistics.food.total += individual0->food;
  });
  statistics.overlap.mean = statistics.overlap.total / group.size();
  statistics.food.mean = statistics.food.total / group.size();
}

void Critters::CollideFood(std::list<Critter *> &group, std::list<Food *> &food) {
  std::for_each(group.begin(), group.end(), [&] (Critter *const individual) {
    std::for_each(food.begin(), food.end(), [&] (Food *const food) {
      const ofVec2f r = food->position - individual->position;
      const float actual_distance = r.length();
      const float colliding_distance = individual->radius() + food->radius();
      if (actual_distance < colliding_distance) {
        individual->food += food->area / Food::kAreaToFood;
        food->area = 0;
      }
    });
  });
}

void Critters::CollideVirii(std::list<Critter *> &group, std::list<Virus *> &virii) {
  std::for_each(group.begin(), group.end(), [&] (Critter *const individual) {
    std::for_each(virii.begin(), virii.end(), [&] (Virus *const virus) {
      const ofVec2f r = virus->position - individual->position;
      const float actual_distance = r.length();
      const float colliding_distance = individual->radius() + virus->radius();
      if (actual_distance < colliding_distance) {
        individual->infection += virus->area / Virus::kAreaToVirus;
        virus->area = 0;
      }
    });
  });
}

void Critters::RemoveDeadIndividuals(std::list<Critter *> &group) {
  group.remove_if([] (const Critter *const individual) -> bool {
    if (individual->area <= 0) {
      delete individual;
      return true;
    } else {
      return false;
    }
  });
}

void Critters::RemoveDeadFood(std::list<Food *> &group) {
  std::for_each(group.begin(), group.end(), [] (Food *const food) {
    if (food->area <= 0) {
      *food = Food();
    }
  });
}

void Critters::RemoveDeadVirii(std::list<Virus *> &group) {
  group.remove_if([] (const Virus *const individual) -> bool {
    if (individual->area <= 0) {
      delete individual;
      return true;
    } else {
      return false;
    }
  });
}

void Critters::draw() {
  ofBackground(0.0, 0.0, 0.0);
  DrawGroup(food);
  DrawGroup(virii);
  DrawGroup(critters);
  DrawGroup(enemy_critters);
  if (debug) {
    ofCircle(enemy_target, 2);
    ofCircle(enemy_center_of_mass, 5);
    std::stringstream overlap;
    overlap << critters.size() << std::endl << enemy_critters.size() << std::endl << ofGetFrameRate();
    ofDrawBitmapString(overlap.str(), 10, 10);
  }
}

void Critters::DrawGroup(std::list<Critter *> &group) const {
  for (auto individual : group) {
    individual->Draw();
  }
}

void Critters::DrawGroup(std::list<Food *> &group) const {
  for (auto mote : group) {
    mote->Draw();
  }
}

void Critters::DrawGroup(std::list<Virus *> &group) const {
  for (auto virus : group) {
    virus->Draw();
  }
}

void Critters::keyPressed(int key) {
  keys[key] = true;
}

void Critters::keyReleased(int key) {
  keys[key] = false;
}

void Critters::mouseMoved(int x, int y) {
  mouse_position = ofVec2f(x, y);
}

void Critters::mouseDragged(int x, int y, int button) {
  if (debug) {
    std::for_each(critters.begin(), critters.end(), [this, x, y] (Critter *const individual) {
      const ofVec2f r = individual->position - ofVec2f(x, y);
      const float actual_distance = r.length();
      const float colliding_distance = individual->radius();
      if (actual_distance < colliding_distance) {
        if (keys['x']) {
          individual->area = 0;
        } else if (statistics.food.total < critters.size()) {
          individual->food += 10;
        }
      }
    });
    std::for_each(enemy_critters.begin(), enemy_critters.end(), [this, x, y] (Critter *const individual) {
      const ofVec2f r = individual->position - ofVec2f(x, y);
      const float actual_distance = r.length();
      const float colliding_distance = individual->radius();
      if (actual_distance < colliding_distance) {
        if (keys['x']) {
          individual->area = 0;
        } else if (enemy_statistics.food.total < enemy_critters.size()) {
          individual->food += 10;
        }
      }
    });
  }
}

void Critters::mousePressed(int x, int y, int button) {
  mouseDragged(x, y, button);
}

void Critters::SteerGroup(std::list<Critter *> &group, ofVec2f target) {
  std::for_each(group.begin(), group.end(), [target] (Critter *const individual) {
    ofVec2f desired_velocity = target - individual->position;
    desired_velocity.scale(100.0);
    individual->force += (desired_velocity - individual->velocity());
  });
}

void Critters::mouseReleased(int x, int y, int button) {
}

void Critters::windowResized(int width, int height) {

}

void Critters::gotMessage(ofMessage msg) {

}

void Critters::dragEvent(ofDragInfo dragInfo) { 

}

void Critters::CreateShape(std::list<Critter *> &group, bool player, ofVec2f at) {
  constexpr float mass = 1.0;
  const float orientation = 2.0 * M_PI * ofRandomuf();
  const ofVec2f velocity = ofVec2f();
  group.push_back(new Critter(player, 0.0, mass, GameObject::kArea, at, velocity));
}
