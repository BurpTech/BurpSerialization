#ifndef BURP_NATIVE
#include <Arduino.h>
#endif

// #define BURP_UNITY_MAX_MODULE 1
// #define BURP_UNITY_MAX_DESCRIBE 3
// #define BURP_UNITY_MAX_POP 2
// #define BURP_UNITY_MAX_TEST 3
// #define BURP_UNITY_MAX_IT 3
// #define BURP_UNITY_MAX_ASYNC_IT 1
// #define BURP_UNITY_MAX_CALLBACK 1
// #define BURP_UNITY_MAX_ASYNC_CALLBACK 1
// #define BURP_UNITY_MAX_STACKED_CALLBACK 1
// #define BURP_UNITY_MAX_STACKED_ASYNC_CALLBACK 1
// #define BURP_UNITY_MAX_SETUP 1
// #define BURP_UNITY_MAX_DEPTH 2
// #define BURP_UNITY_MAX_BEFORE_EACH 1
// #define BURP_UNITY_MAX_ASYNC_BEFORE_EACH 1
// #define BURP_UNITY_MAX_AFTER_EACH 1
// #define BURP_UNITY_MAX_ASYNC_AFTER_EACH 1
// #define BURP_UNITY_MAX_LOOP 1

#include <unity.h>
#include <BurpUnity.hpp>

#include "CStr.hpp"
#include "CBool.hpp"
#include "UInt8.hpp"
#include "Object.hpp"
#include "PWMLevels.hpp"

Runner<5> runner({
    &CStr::tests,
    &CBool::tests,
    &UInt8::tests,
    &Object::tests,
    &PWMLevels::tests
});
Memory memory;
bool running = true;
int status;

void setup() {
  UNITY_BEGIN();
  // BurpUnity::Usage::printParams(memory);
  runner.setup(memory);
}

void loop() {
  if (running) {
    runner.loop();
    if (runner.isFinished()) {
      // BurpUnity::Usage::printUsage(memory, runner);
      // BurpUnity::Usage::printUsageDefines(memory, runner, 1);
      status = UNITY_END();
      running = false;
    }
  }
}

int main() {
  setup();
  while (running) {
    loop();
  }
  return status;
}
