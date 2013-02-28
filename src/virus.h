//
//  virus.h
//  minimalist01
//
//  Created by Robert Dionne on 2/27/13.
//
//

#ifndef __minimalist01__virus__
#define __minimalist01__virus__

#include "gameobject.h"
#include "ofMain.h"

class Virus : public GameObject {
public:
  static constexpr float kArea = 5.0;
  static constexpr float kAreaToVirus = 5.0;
  static const ofColor kColor;
  static constexpr float kGrowthRate = 0.01;
  static constexpr float kImpulse = 0.01;
  static constexpr float kInfectionRate = 0.01;
  static constexpr float kMortality = 0.001;
  
  Virus();
  
  virtual ~Virus() {}
  
  virtual void Draw() const;
  
  void Update(float dt);
  
private:
  void Wrap();
};

#endif /* defined(__minimalist01__virus__) */
