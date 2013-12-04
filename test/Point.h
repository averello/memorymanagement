//
//  Point.h
//  memorymanagement
//
//  Created by George Boumis on 1/12/13.
//  Copyright (c) 2013 George Boumis <developer.george.boumis@gmail.com>. All rights reserved.
//

#ifndef memorymanagement_Point_h
#define memorymanagement_Point_h

#include <memory_management/memory_management.h>

typedef struct _point {
	int x, y;
} Point;

Point *allocatePoint(int x, int y);

#endif
