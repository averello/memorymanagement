//
//  Point.c
//  memorymanagement
//
//  Created by George Boumis on 1/12/13.
//  Copyright (c) 2013 George Boumis <developer.george.boumis@gmail.com>. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "Point.h"

Point *allocatePoint(int x, int y) {
	Point *point = calloc(1, sizeof(Point));
	MEMORY_MANAGEMENT_INITIALIZE(point);
	point->x = x, point->y = y;
	return point;
}
