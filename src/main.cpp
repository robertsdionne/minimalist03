#include "critters.h"
#include "ofAppGlutWindow.h"
#include "ofMain.h"

constexpr unsigned int kWidth = 1024;
constexpr unsigned int kHeight = 768;

int main(const int argument_count, const char **arguments) {
  ofAppGlutWindow window;
  window.setGlutDisplayString("rgba double depth alpha samples>=4");
  ofSetupOpenGL(&window, kWidth, kHeight, OF_WINDOW);
  ofRunApp(new Critters());
  return 0;
}
