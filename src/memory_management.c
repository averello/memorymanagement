//
//  memory_management.c
//  xfig
//
//  Created by George Boumis on 12/10/13.
//  Copyright (c) 2013 George Boumis. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "memory_management.h"

#define MEMORY_MANAGEMENT_PROTOTYPE_INTERAL (memory_management_prototype_internal)
#define _MEMORY_MANAGEMENT_INTERNAL_TYPE struct _memory_management_attributes_internal
#define _MEMORY_MANAGEMENT_INTERNAL_CAST(o) ((_MEMORY_MANAGEMENT_INTERNAL_TYPE *)(o))
#define _MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(var) _MEMORY_MANAGEMENT_INTERNAL_TYPE *var

static const unsigned long long _invalidRetainCount = (unsigned long long)(-1UL);

static void _initMemoryManagement() __attribute__ ((constructor));

_MEMORY_MANAGEMENT_INTERNAL_TYPE {
	unsigned long long retainCount;
	void (*dealloc)(void *);
};

_MEMORY_MANAGEMENT_INTERNAL_TYPE MEMORY_MANAGEMENT_PROTOTYPE_INTERAL = {
	1UL,
	NULL
};

MEMORY_MANAGEMENT_TYPE MEMORY_MANAGEMENT_PROTOTYPE;

static void _initMemoryManagement() {
	memcpy(&MEMORY_MANAGEMENT_PROTOTYPE, &MEMORY_MANAGEMENT_PROTOTYPE_INTERAL, sizeof(_MEMORY_MANAGEMENT_INTERNAL_TYPE));
}

void *memory_management_retain(void *o) {
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	if (NULL == object)
		return NULL;
	__sync_fetch_and_add(&(object->retainCount), 1);
	return object;
}

void memory_management_release(void *o) {
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	if (NULL == object)
		return;
	
	unsigned long long result = __sync_sub_and_fetch(&(object->retainCount), 1);
	assert(result != _invalidRetainCount);
	if ( result == 0) {
		if (NULL != object->dealloc)
			object->dealloc(object);
		object->retainCount = _invalidRetainCount;
		free(o);
		return;
	}
}

void memory_management_attributes_set_dealloc_function(void *o, void (*deallocf)(void *)) {
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	object->dealloc = deallocf;
}

unsigned long long memory_management_get_retain_count(void *o) {
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	return object->retainCount;
}

#undef _MEMORY_MANAGEMENT_INTERNAL_TYPE
#undef _MEMORY_MANAGEMENT_INTERNAL_CAST
#undef _MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE
