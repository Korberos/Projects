#pragma once

#include "vec3.h"
#include "MapController.h"

bool LineSegmentTriangleCheck(vec3f &Return, unsigned int &TriangleIndex, MapController* Map, const vec3f &LineStart, const vec3f &LineEnd);