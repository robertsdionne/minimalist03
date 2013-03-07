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
  static constexpr float kArea = M_PI * 70.0;
  static constexpr float kAreaToFood = 10.0;
  static constexpr float kImpulse = 8.0;
  static constexpr float kLineWidthScaleFactor = 0.004;
  
  Food();
  
  virtual ~Food() {}
  
  virtual void Draw() const;
  
  virtual void DrawInternal() const;
  
  void Update(float dt);
  
private:
  void Wrap();
};

#endif /* defined(__minimalist01__food__) */
