//
//  circle.h
//  minimalist01
//
//  Created by Robert Dionne on 2/12/13.
//
//

#ifndef __minimalist01__circle__
#define __minimalist01__circle__

#include <iostream>
#include <list>

#include "gameobject.h"
#include "ofMain.h"

class Circle : public GameObject {
public:
  Circle(bool player, float mass, float size, float food, float orientation, ofVec2f position, ofVec2f velocity);
  
  virtual ~Circle();
  
  virtual ofColor membrane_color() const;
  virtual ofColor wall_cell_color() const;
  virtual ofColor interior_cell_color() const;
  
  virtual float reproductivity() const;
  virtual float mortality() const;
  
  virtual void DrawInternal() const;
  
private:
  static constexpr float kReproductivity = 0.001;
  static constexpr float kMortality = 0.002;
  static const ofColor kMembraneColor;
  static const ofColor kWallCellColor;
  static const ofColor kInteriorCellColor;
};

#endif /* defined(__minimalist01__circle__) */
