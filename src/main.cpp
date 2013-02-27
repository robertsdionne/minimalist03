#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

constexpr unsigned int kWidth = 1920;
constexpr unsigned int kHeight = 1200;

int main(const int argument_count, const char **arguments) {
  ofAppGlutWindow window;
  ofSetupOpenGL(&window, kWidth, kHeight, OF_FULLSCREEN);
  ofRunApp(new testApp());
  return 0;
}
