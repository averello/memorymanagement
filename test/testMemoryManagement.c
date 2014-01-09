//
//  testMemoryManagement.c
//  memorymanagement
//
//  Created by George Boumis on 1/12/13.
//  Copyright (c) 2013 George Boumis <developer.george.boumis@gmail.com>. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>   /* pour le rint */
#include <string.h> /* pour le memcpy */
#include <time.h>   /* chronometrage */
#include <libgen.h> /* pour basename */
#include <sys/stat.h> /* pour mkdir */
#include <sys/time.h> /* gettimeofday */
#include <unistd.h>   /* pour getlogin */
#include <pthread.h>
#include "Point.h"

double my_gettimeofday(){
    struct timeval tmp_time;
    gettimeofday(&tmp_time, NULL);
    return tmp_time.tv_sec + (long)(tmp_time.tv_usec * 1.0e-6L);
}

void *manyRetains(void *arg);
void *manyReleases(void *arg);
void testCopy();

#define TIMES 100000000

int main() {
	Point *p = allocatePoint(5, 6);
	assert(p != NULL);
	assert(p->x == 5);
	assert(p->y == 6);
	
	assert(MEMORY_MANAGEMENT_GET_RETAIN_COUNT(p)==1);
	assert(MEMORY_MANAGEMENT_GET_RETAIN_COUNT(retain(p)) ==2);
	
//	pthread_t threads[2];
//	pthread_create(&(threads[0]), NULL, manyRetains, p);
//	pthread_create(&(threads[1]), NULL, manyReleases, p);
//	pthread_join(threads[0], NULL);
//	pthread_join(threads[1], NULL);
	
//	manyRetains(p);
//	manyReleases(p);
	
//	for (int i=0; i<TIMES; i++) release(allocatePoint(rand(), rand()));
	
	release(p);
	release(retain(p));
	release(p);
	
	testCopy();
	
	memory_management_print_stats();
	return 0;
}

void testCopy() {
	Point *point = allocatePoint(42, 43);
	
	retain(point);
	assert(MEMORY_MANAGEMENT_GET_RETAIN_COUNT(point) == 2);
	
	Point *managedCopyPoint = MEMORY_MANAGEMENT_COPY(point, MemoryManagementDomainManaged);
	assert(managedCopyPoint != NULL);
	assert(MEMORY_MANAGEMENT_ENABLED(managedCopyPoint) != 0);
	assert(MEMORY_MANAGEMENT_GET_RETAIN_COUNT(managedCopyPoint) == 1);
	
	Point *unmanagedCopyPoint = MEMORY_MANAGEMENT_COPY(managedCopyPoint, MemoryManagementDomainUnmanaged);
	assert(unmanagedCopyPoint != NULL);
	assert(MEMORY_MANAGEMENT_ENABLED(unmanagedCopyPoint) == 0);
	
	Point *error = MEMORY_MANAGEMENT_COPY(point, 42);
	assert(error == NULL);
	
	
	Point *tryManagedCopyPoint = MEMORY_MANAGEMENT_COPY(unmanagedCopyPoint, MemoryManagementDomainManaged);
	assert(tryManagedCopyPoint == NULL);
	
	free(unmanagedCopyPoint);
	release(managedCopyPoint);
	release(point);
	release(point);
}

void *manyRetains(void *arg) {
	double start, end;
	Point *p = arg;
	start = my_gettimeofday();
	for (int i=0; i<TIMES; i++)
		retain(p);
	end = my_gettimeofday();
	printf("time of %u retains %lf\n", TIMES, end-start);
	return NULL;
}

void *manyReleases(void *arg) {
	struct timespec time;
	time.tv_nsec = 10e7;
	time.tv_sec = 0;
	nanosleep(&time, NULL);
	double start, end;
	Point *p = arg;
	start = my_gettimeofday();
	for (int i=0; i<TIMES; i++)
		release(p);
	end = my_gettimeofday();
	printf("time of %u release %lf\n", TIMES, end-start);
	return NULL;
}
