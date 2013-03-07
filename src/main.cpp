#include "critters.h"
#include "ofAppGlutWindow.h"
#include "ofMain.h"

constexpr unsigned int kWidth = 1440;
constexpr unsigned int kHeight = 900;

int main(const int argument_count, const char **arguments) {
  ofAppGlutWindow window;
  window.setGlutDisplayString("rgba double depth alpha samples>=4");
  ofSetupOpenGL(&window, kWidth, kHeight, OF_FULLSCREEN);
  ofRunApp(new Critters());
  return 0;
}
