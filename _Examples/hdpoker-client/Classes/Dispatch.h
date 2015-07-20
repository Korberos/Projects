#pragma once

#include <functional>

// One of the few global objects outside of cocos
// Used to usher calls back to UI thread

void dispatch_main(const std::function<void()>& handler);
void dispatch();