#include "critter.h"
#include "testApp.h"

constexpr unsigned int testApp::kNumGameObjects;

void testApp::setup() {
  ofEnableSmoothing();
  mouse_position = ofVec2f(ofGetWidth() / 2, ofGetHeight() / 2);
  reproduce_type = 0;
  enemy_target_angle = 0;
  for (unsigned int i = 0; i < kNumGameObjects; ++i) {
    CreateShape(critters, true, ofVec2f(ofRandomWidth(), ofRandomHeight()));
  }
  for (unsigned int i = 0; i < kNumGameObjects; ++i) {
    CreateShape(enemy_critters, false, ofVec2f(ofRandomWidth(), ofRandomHeight()));
  }
}

void testApp::update() {
  RemoveDeadIndividuals(critters);
  RemoveDeadIndividuals(enemy_critters);
  UpdateGroup(critters, statistics, mouse_position, statistics.overlap.mean < 0.5, true);
  enemy_target_angle += ofSignedNoise(ofGetElapsedTimef() / 5.0) * 0.05;
  const float radius = ofGetHeight() / 3.0;
  enemy_center_of_mass = FindCenterOfMass(enemy_critters);
  enemy_target = ofVec2f(radius * cos(enemy_target_angle), radius * sin(enemy_target_angle)) + enemy_center_of_mass;
  Wrap(enemy_target);
  UpdateGroup(enemy_critters, enemy_statistics, enemy_target, enemy_statistics.overlap.mean < 0.5, false);
  old_circle_key_down = circle_key_down;
}

void testApp::UpdateGroup(std::list<Critter *> &group, Statistics &statistics, ofVec2f target, bool move, bool player) {
  if (move) {
    SteerGroup(group, target);
  }
  Collide(group, statistics);
  if (player && !old_circle_key_down && circle_key_down) {
    Launch(group);
  }
  for (auto individual : group) {
    individual->Update(1.0 / ofGetFrameRate());
    if (individual->food >= 0.0001) {
      individual->food -= 0.0001;
    }
    individual->MaybeReproduce(group);
  }
}

ofVec2f testApp::FindCenterOfMass(std::list<Critter *> &group) {
  ofVec2f center_of_mass;
  float total_mass;
  std::for_each(group.begin(), group.end(), [&center_of_mass, &total_mass] (Critter *const individual) {
    center_of_mass += individual->size * individual->position;
    total_mass += individual->size;
  });
  center_of_mass /= total_mass;
  return center_of_mass;
}

void testApp::Launch(std::list<Critter *> &group) {
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
      individual->force += desired_velocity - individual->velocity;
      break;
    }
  }
}

void testApp::Wrap(ofVec2f &position) {
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

void testApp::Collide(std::list<Critter *> &group, Statistics &statistics) {
  statistics = Statistics();
  std::for_each(group.begin(), group.end(), [&] (Critter *const individual0) {
    std::list<Critter *> overlapping;
    std::for_each(group.begin(), group.end(), [&] (Critter *const individual1) {
      if (individual0 != individual1) {
        const ofVec2f r = individual1->position - individual0->position;
        const float actual_distance = r.length();
        const float colliding_distance = individual0->size + individual1->size;
        if (actual_distance < colliding_distance) {
          overlapping.push_back(individual1);
          const float overlap = colliding_distance - actual_distance;
          statistics.overlap.min = std::min(statistics.overlap.min, overlap);
          statistics.overlap.max = std::max(statistics.overlap.max, overlap);
          statistics.overlap.total += overlap / 2;
          individual0->force -= 5.0 * r.normalized() * pow(overlap, 1);
          individual1->force += 5.0 * r.normalized() * pow(overlap, 1);
        }
      }
    });
    const float size_diffusion_amount = 0.01 * overlapping.size();
    const float food_diffusion_amount = 0.01;
    individual0->neighbors = overlapping;
    std::for_each(overlapping.begin(), overlapping.end(), [&] (Critter *const individual1) {
      if (ofRandomuf() < 0.1 && individual0->size > Critter::kMinSize
          && individual1->size < Critter::kMaxSize) {
        individual0->size -= size_diffusion_amount;
        individual1->size += size_diffusion_amount;
      }
      if (ofRandomuf() < individual0->food && individual0->food > 0 && individual1->food < 1) {
        individual0->food -= food_diffusion_amount;
        individual1->food += food_diffusion_amount;
      }
    });
    statistics.food.total += individual0->food;
  });
  statistics.overlap.mean = statistics.overlap.total / group.size();
  statistics.food.mean = statistics.food.total / group.size();
}

void testApp::RemoveDeadIndividuals(std::list<Critter *> &group) {
  group.remove_if([] (const Critter *const individual) -> bool {
    if (individual->size <= 0) {
      delete individual;
      return true;
    } else {
      return false;
    }
  });
}

void testApp::draw() {
  ofBackground(0.0, 0.0, 0.0);
  DrawGroup(critters);
  DrawGroup(enemy_critters);
  ofCircle(enemy_target, 2);
  ofCircle(enemy_center_of_mass, 5);
  std::stringstream overlap;
  overlap << critters.size() << std::endl << enemy_critters.size();
  ofDrawBitmapString(overlap.str(), 10, 10);
}

void testApp::DrawGroup(std::list<Critter *> &group) {
  for (auto individual : group) {
    individual->Draw();
  }
}

void testApp::keyPressed(int key) {
  switch (key) {
    case 'w':
      reproduce_type = 0;
      break;
    case ' ':
      circle_key_down = true;
      reproduce_type = 1;
      break;
    case 'a':
      square_key_down = true;
      reproduce_type = 2;
      break;
    case 'x':
      shift_key_down = true;
      break;
    default:
      break;
  }
}

void testApp::keyReleased(int key) {
  switch (key) {
    case 'w':
      break;
    case ' ':
      circle_key_down = false;
      break;
    case 'a':
      square_key_down = false;
      break;
    case 'x':
      shift_key_down = false;
      break;
    default:
      break;
  }
}

void testApp::mouseMoved(int x, int y) {
  mouse_position = ofVec2f(x, y);
}

void testApp::mouseDragged(int x, int y, int button) {
  std::for_each(critters.begin(), critters.end(), [this, x, y] (Critter *const individual) {
    const ofVec2f r = individual->position - ofVec2f(x, y);
    const float actual_distance = r.length();
    const float colliding_distance = individual->size;
    if (actual_distance < colliding_distance) {
      if (shift_key_down) {
        individual->size = 0;
      } else if (statistics.food.total < critters.size()) {
        individual->food += 10;
      }
    }
  });
  std::for_each(enemy_critters.begin(), enemy_critters.end(), [this, x, y] (Critter *const individual) {
    const ofVec2f r = individual->position - ofVec2f(x, y);
    const float actual_distance = r.length();
    const float colliding_distance = individual->size;
    if (actual_distance < colliding_distance) {
      if (shift_key_down) {
        individual->size = 0;
      } else if (enemy_statistics.food.total < enemy_critters.size()) {
        individual->food += 10;
      }
    }
  });
}

void testApp::mousePressed(int x, int y, int button) {
  mouse_down = true;
  mouseDragged(x, y, button);
}

void testApp::SteerGroup(std::list<Critter *> &group, ofVec2f target) {
  std::for_each(group.begin(), group.end(), [target] (Critter *const individual) {
    ofVec2f desired_velocity = target - individual->position;
    if (desired_velocity.length() < 1000.0) {
      desired_velocity.scale(1000.0);
    }
    individual->force += desired_velocity - individual->velocity;
  });
}

void testApp::mouseReleased(int x, int y, int button) {
  mouse_down = false;
}

void testApp::windowResized(int width, int height) {

}

void testApp::gotMessage(ofMessage msg) {

}

void testApp::dragEvent(ofDragInfo dragInfo) { 

}

void testApp::CreateShape(std::list<Critter *> &group, bool player, ofVec2f at) {
  constexpr float mass = 1.0;
  const float size = 10;
  const float orientation = 2.0 * M_PI * ofRandomuf();
  const ofVec2f velocity = ofVec2f();
  group.push_back(new Critter(player, mass, size, 0.0, orientation, at, velocity));
}
