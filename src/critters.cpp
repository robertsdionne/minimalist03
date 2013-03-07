#include "critter.h"
#include "critters.h"
#include "food.h"

constexpr unsigned int Critters::kNumCritters;

void Critters::setup() {
  ofSetFrameRate(60.0);
//  ofEnableSmoothing();
  mouse_position = ofVec2f(kStartCoordinate, kStartCoordinate);
  reproduce_type = 0;
  enemy_target_angle = 0;
  for (unsigned int i = 0; i < kNumCritters; ++i) {
    CreateShape(critters, true, ofVec2f(kStartCoordinate, kStartCoordinate) + ofVec2f(ofRandomf(), ofRandomf()));
    CreateShape(enemy_critters, false, ofVec2f(ofGetWidth() - kStartCoordinate, ofGetHeight() - kStartCoordinate)
                + ofVec2f(ofRandomf(), ofRandomf()));
  }
  for (unsigned int i = 0; i < kNumFood; ++i) {
    food.push_back(new Food());
  }
  debug = false;
  small_plops.loadSound("../../plops_small.wav");
  small_plops.setMultiPlay(true);
  plops1.loadSound("../../plops.wav");
  plops1.setMultiPlay(true);
  plops2.loadSound("../../plops2.wav");
  plops2.setMultiPlay(true);
  ratchet1.loadSound("../../ratchet1.mp3");
  ratchet1.setMultiPlay(true);
  ratchet2.loadSound("../../ratchet3.mp3");
  ratchet2.setMultiPlay(true);
  eat.loadSound("../../eat.wav");
  eat.setMultiPlay(true);
  last_move_time = last_enemy_move_time = ofGetElapsedTimef();
  move_sound_delay = kMoveSoundDelay;
  attack_sound_delay = kAttackSoundDelay;
  overlap_zero = false;
  enemy_overlap_zero = false;
}

void Critters::update() {
  UpdateGroup(critters, statistics, mouse_position, statistics.overlap.mean < kOverlap, true);
  enemy_target_angle += ofSignedNoise(ofGetElapsedTimef() / 5.0) * 0.05;
  const float radius = ofGetHeight() / 3.0;
  enemy_center_of_mass = FindCenterOfMass(enemy_critters);
  enemy_target = ofVec2f(radius * cos(enemy_target_angle), radius * sin(enemy_target_angle)) + enemy_center_of_mass;
  float nearest_food_distance = std::numeric_limits<float>::infinity();
  ofVec2f nearest_food;
  for (auto food : food) {
    const float dx = (food->position - enemy_center_of_mass).length();
    if (dx < nearest_food_distance) {
      nearest_food_distance = dx;
      nearest_food = food->position;
    }
  }
  if (enemy_statistics.attackers.total > 1.1 * statistics.attackers.total) {
    enemy_target = FindCenterOfMass(critters);
  } else {
    float t = 1.0 - ofClamp(enemy_statistics.food.mean / 10.0, 0.0, 1.0);
    enemy_target = (1.0 - t) * enemy_target + t * nearest_food;
  }
  Wrap(enemy_target);
  UpdateFood(food);
  UpdateGroup(enemy_critters, enemy_statistics, enemy_target, enemy_statistics.overlap.mean < kOverlap, false);
  CollideFood(critters, food);
  CollideFood(enemy_critters, food);
  Collide(critters, enemy_critters);
  RemoveDeadFood(food);
  RemoveDeadIndividuals(critters);
  RemoveDeadIndividuals(enemy_critters);
  if (keys[OF_KEY_BACKSPACE] && !previous_keys[OF_KEY_BACKSPACE]) {
    debug = !debug;
  }
  previous_keys = keys;
}

float Critters::FindAverageSpeed(std::list<Critter *> &group) {
  float total = 0.0;
  for (auto critter : group) {
    total += critter->velocity.length();
  }
  return total / group.size();
}

void Critters::UpdateGroup(std::list<Critter *> &group, Statistics &statistics, ofVec2f target, bool move, bool player) {
  if (move) {
    SteerGroup(group, target);
    if (player ? !overlap_zero : !enemy_overlap_zero) {
      player ? overlap_zero : enemy_overlap_zero = true;
    }
    const float prev_move_time = player ? last_move_time : last_enemy_move_time;
    if (player ? overlap_zero : enemy_overlap_zero && ofGetElapsedTimef() - prev_move_time > kMoveSoundDelay) {
      const float choice = ofRandomuf();
      if (choice < 0.33) {
        plops1.play();
      } else if (choice < 0.66) {
        plops2.play();
      } else {
        small_plops.play();
      }
      if (player) {
        last_move_time = ofGetElapsedTimef();
      } else {
        last_enemy_move_time = ofGetElapsedTimef();
      }
      move_sound_delay = kMoveSoundDelay + ofRandomuf();
      player ? overlap_zero : enemy_overlap_zero = false;
    }
  }
  Collide(group, statistics);
  for (auto individual : group) {
    individual->Update(1.0 / ofGetFrameRate());
    if (individual->food >= 0.0001) {
      individual->food -= 0.0001;
    }
    individual->MaybeReproduce(group);
  }
}

void Critters::UpdateFood(std::list<Food *> &group) {
  for (auto mote : group) {
    mote->Update(1.0 / ofGetFrameRate());
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
          individual0->force -= 5.0 * r.normalized() * overlap;
          individual1->force += 5.0 * r.normalized() * overlap;
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
    statistics.food.total += individual0->food;
    statistics.attackers.total += individual0->attacker();
  });
  statistics.attackers.mean = statistics.attackers.total / group.size();
  statistics.overlap.mean = statistics.overlap.total / group.size();
  statistics.food.mean = statistics.food.total / group.size();
}

void Critters::Collide(std::list<Critter *> &critters, std::list<Critter *> &enemy_critters) {
  std::for_each(critters.begin(), critters.end(), [&] (Critter *const critter) {
    std::for_each(enemy_critters.begin(), enemy_critters.end(), [&] (Critter *const enemy_critter) {
      const ofVec2f r = enemy_critter->position - critter->position;
      const float actual_distance = r.length();
      const float colliding_distance = critter->radius() + enemy_critter->radius();
      if (actual_distance < colliding_distance) {
        const float overlap = colliding_distance - actual_distance;
        critter->force -= 10.0 * r.normalized() * sqrt(overlap);
        enemy_critter->force += 10.0 * r.normalized() * sqrt(overlap);
        if (critter->attacker() && ofRandomuf() < Critter::kAttackChance) {
          if (abs(critter->orientation_speed) <= 0.01) {
            const float choice = ofRandomuf();
            if (choice < 0.5) {
              ratchet1.play();
            } else {
              ratchet2.play();
            }
          }
          critter->orientation_speed += 5.0 * ofRandomuf();
          enemy_critter->area /= 2.0;
          if (enemy_critter->area < 50.0) {
            enemy_critter->area = 0;
          }
        }
        if (enemy_critter->attacker() && ofRandomuf() < Critter::kAttackChance) {
          if (abs(enemy_critter->orientation_speed) <= 0.01) {
            const float choice = ofRandomuf();
            if (choice < 0.5) {
              ratchet1.play();
            } else {
              ratchet2.play();
            }
          }
          enemy_critter->orientation_speed += 5.0 * ofRandomuf();
          critter->area /= 2.0;
          if (critter->area < 50.0) {
            critter->area = 0;
          }
        }
      }
    });
  });
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
        eat.play();
      }
    });
  });
}

void Critters::RemoveDeadIndividuals(std::list<Critter *> &group) {
  std::for_each(group.begin(), group.end(), [] (Critter *const individual) {
    std::list<Critter *> new_neighbors;
    std::copy(individual->neighbors.begin(), individual->neighbors.end(), new_neighbors.begin());
    new_neighbors.remove_if([] (const Critter *const individual) -> bool {
      if (individual->area <= 0) {
        delete individual;
        return true;
      } else {
        return false;
      }
    });
    individual->neighbors = new_neighbors;
  });
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

void Critters::draw() {
  ofBackground(0.0, 0.0, 0.0);
  DrawGroup(food);
  DrawGroup(critters);
  DrawGroup(enemy_critters);
  if (debug) {
    ofLine(enemy_target + ofVec2f(0, -5), enemy_target + ofVec2f(0, 5));
    ofLine(enemy_target + ofVec2f(-5, 0), enemy_target + ofVec2f(5, 0));
    ofCircle(enemy_target, 2);
    //ofCircle(enemy_center_of_mass, 5);
    std::stringstream overlap;
    overlap << critters.size() << std::endl << enemy_critters.size() << std::endl
        << statistics.food.mean << std::endl << enemy_statistics.food.mean << std::endl
        << ofGetFrameRate();
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
    if (desired_velocity.length() < 1000.0) {
      desired_velocity.scale(1000.0);
    }
    individual->force += (desired_velocity - individual->velocity);
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
