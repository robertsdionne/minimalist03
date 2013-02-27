//
//  food.h
//  minimalist01
//
//  Created by Robert Dionne on 2/27/13.
//
//

#ifndef __minimalist01__food__
#define __minimalist01__food__

#include "gameobject.h"

class Food : public GameObject {
public:
  static constexpr float kArea = 10.0;
  static constexpr float kAreaToFood = 10.0;
  static constexpr float kImpulse = 0.01;
  
  Food();
  
  virtual ~Food() {}
  
  virtual void Draw() const;
  
  void Update(float dt);
  
private:
  void Wrap();
};

#endif /* defined(__minimalist01__food__) */
