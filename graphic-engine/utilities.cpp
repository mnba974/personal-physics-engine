#include <cmath>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <algorithm> 
#include "utilities.h"

bool cmp(object *obj1, object *obj2) {
	return obj1->position.x < obj2->position.x;
}