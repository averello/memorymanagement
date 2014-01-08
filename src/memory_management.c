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
#include <memory_management/memory_management.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


#define _MEMORY_MANAGEMENT_CANARY_VALUE 0xCA11ACAB
#define _MEMORY_MANAGEMENT_CANARY_BAD_VALUE 0xDEADDEAD

#define _MEMORY_MANAGEMENT_CANARY_ATTRIBUTE_NAME canary
#define _MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE_NAME retainCount
#define _MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE_NAME dealloc

#define _MEMORY_MANAGEMENT_PROTOTYPE_INTERNAL _memory_management_prototype_internal
#define _MEMORY_MANAGEMENT_INTERNAL_TYPE struct _memory_management_attributes_internal
#define _MEMORY_MANAGEMENT_INTERNAL_CAST(o) (((_MEMORY_MANAGEMENT_INTERNAL_TYPE *)(o))-1)
#define _MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(var) _MEMORY_MANAGEMENT_INTERNAL_TYPE *var

#define _MEMORY_MANAGEMENT_INITIALIZE(o) ((*o) = _MEMORY_MANAGEMENT_PROTOTYPE_INTERNAL)

#define _MEMORY_MANAGEMENT_CANARY_ATTRIBUTE(o) (o)->_MEMORY_MANAGEMENT_CANARY_ATTRIBUTE_NAME
#define _MEMORY_MANAGEMENT_CHECK_ENABLED(o) (_MEMORY_MANAGEMENT_CANARY_ATTRIBUTE(o) == _MEMORY_MANAGEMENT_CANARY_VALUE)
#define _MEMORY_MANAGEMENT_IS_INVALIDATED(o) (_MEMORY_MANAGEMENT_CANARY_ATTRIBUTE(o) == _MEMORY_MANAGEMENT_CANARY_BAD_VALUE)
#define _MEMORY_MANAGEMENT_INVALIDATE(o) (_MEMORY_MANAGEMENT_CANARY_ATTRIBUTE(o) = _MEMORY_MANAGEMENT_CANARY_BAD_VALUE)

#define _MEMORY_MANAGEMENT_INVALID_RETAIN_COUNT (unsigned int)(-1U)

#define _MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE(o) (o)->_MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE_NAME
#define _MEMORY_MANAGEMENT_ATOMIC_RETAIN(o)  (__sync_fetch_and_add(&(_MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE(o)), 1))
#define _MEMORY_MANAGEMENT_ATOMIC_RELEASE(o) (__sync_sub_and_fetch(&(_MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE(o)), 1))

#define _MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE(o) (o)->_MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE_NAME
#define _MEMORY_MANAGEMENT_CALL_DEALLOC(o) if (NULL != _MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE(o)) _MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE(o)(o+1)

#ifdef DEBUG
#define STATS

size_t _total_live_memory = 0;

size_t _total_memory_allocated = 0;
size_t _total_memory_deallocated = 0;

long _total_allocations = 0;
long _total_deallocations = 0;

pthread_mutex_t guardian = PTHREAD_MUTEX_INITIALIZER;

//__thread size_t _total_thread_live_memory = 0;
#define STATISTIC_DATA size_t size;

#else
#define STATISTIC_DATA
#endif /* DEBUG */

_MEMORY_MANAGEMENT_INTERNAL_TYPE {
	unsigned int _MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE_NAME;
	unsigned int _MEMORY_MANAGEMENT_CANARY_ATTRIBUTE_NAME;
	void (*_MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE_NAME)(void *);
	size_t size;
};

_MEMORY_MANAGEMENT_INTERNAL_TYPE _MEMORY_MANAGEMENT_PROTOTYPE_INTERNAL = {
	1ULL,
	_MEMORY_MANAGEMENT_CANARY_VALUE,
	NULL
#ifdef STATS
	, 0
#endif
};

void *memory_management_retain(void *o) {
	if (NULL==o) return NULL;
	
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	if (!_MEMORY_MANAGEMENT_CHECK_ENABLED(object) || _MEMORY_MANAGEMENT_IS_INVALIDATED(object)) {
		if (_MEMORY_MANAGEMENT_IS_INVALIDATED(object)) {
			assert(0 && "Called retain() on invalided pointer.");
		}
		return errno = EFAULT, o;
	}
	_MEMORY_MANAGEMENT_ATOMIC_RETAIN(object);
	return o;
}

void memory_management_release(void *o) {
	if (NULL==o) return;
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	if (!_MEMORY_MANAGEMENT_CHECK_ENABLED(object) || _MEMORY_MANAGEMENT_IS_INVALIDATED(object)) {
		if (_MEMORY_MANAGEMENT_IS_INVALIDATED(object)) {
			assert(0 && "Called release() on invalided pointer.");
		}
		errno = EFAULT;
		return;
	}
	
	unsigned long long result = _MEMORY_MANAGEMENT_ATOMIC_RELEASE(object);
	assert(result != _MEMORY_MANAGEMENT_INVALID_RETAIN_COUNT && "Sent release() to invalid pointer.");
	if ( result == 0) {
		_MEMORY_MANAGEMENT_CALL_DEALLOC(object);
		_MEMORY_MANAGEMENT_INVALIDATE(object);
#ifdef STATS
		pthread_mutex_lock(&guardian);
		_total_live_memory -= object->size;
		_total_memory_deallocated += object->size;
		_total_deallocations++;
		pthread_mutex_unlock(&guardian);
#endif
		free(object);
		return;
	}
}

void memory_management_attributes_set_dealloc_function(void *o, void (*deallocf)(void *)) {
	if (NULL==o) return;
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	
	if (!_MEMORY_MANAGEMENT_CHECK_ENABLED(object) || _MEMORY_MANAGEMENT_IS_INVALIDATED(object)) {
		errno = EFAULT;
		return;
	}
	
	_MEMORY_MANAGEMENT_DEALLOC_ATTRIBUTE(object) = deallocf;
}

unsigned int memory_management_get_retain_count(const void *o) {
	if (NULL==o) return _MEMORY_MANAGEMENT_INVALID_RETAIN_COUNT;
	_MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE(object) = _MEMORY_MANAGEMENT_INTERNAL_CAST(o);
	
	if (!_MEMORY_MANAGEMENT_CHECK_ENABLED(object) || _MEMORY_MANAGEMENT_IS_INVALIDATED(object)) {
		errno = EFAULT;
		return _MEMORY_MANAGEMENT_INVALID_RETAIN_COUNT;
	}
	
	return _MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE(object);
}

void *memory_management_alloc(size_t size) {
	size_t totalSize = sizeof(_MEMORY_MANAGEMENT_INTERNAL_TYPE) + size;
	_MEMORY_MANAGEMENT_INTERNAL_TYPE *o = calloc(1,  totalSize);
	if (NULL==o) return errno = ENOMEM, (void *)NULL;
	_MEMORY_MANAGEMENT_INITIALIZE(o);
	o->size = totalSize;
#ifdef STATS
	pthread_mutex_lock(&guardian);
	_total_live_memory += totalSize;
	_total_memory_allocated += totalSize;
	_total_allocations++;
	pthread_mutex_unlock(&guardian);
#endif
	return o+1;
}

void memory_management_print_stats() {
#ifdef STATS
	printf("==%d== HEAP SUMMARY:\n", getpid());
	printf("==%d==      in use: %ld bytes\n", getpid(), _total_live_memory);
	printf("==%d==  heap usage: %ld allocs, %ld frees, %ld bytes allocated, %ld bytes deallocated\n", getpid(), _total_allocations, _total_deallocations, _total_memory_allocated, _total_memory_deallocated);
#endif
}


#undef _MEMORY_MANAGEMENT_CANARY_VALUE
#undef _MEMORY_MANAGEMENT_CANARY_BAD_VALUE


#undef _MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE_NAME
#undef _MEMORY_MANAGEMENT_CANARY_ATTRIBUTE_NAME
#undef _MEMORY_MANAGEMENT_DEALLOC_NAME

#undef _MEMORY_MANAGEMENT_PROTOTYPE_INTERNAL
#undef _MEMORY_MANAGEMENT_INTERNAL_TYPE
#undef _MEMORY_MANAGEMENT_INTERNAL_CAST
#undef _MEMORY_MANAGEMENT_DECLARE_INTERNAL_VARIABLE

#undef _MEMORY_MANAGEMENT_CANARY_ATTRIBUTE
#undef _MEMORY_MANAGEMENT_CHECK_ENABLED
#undef _MEMORY_MANAGEMENT_IS_INVALIDATED
#undef _MEMORY_MANAGEMENT_INVALIDATE

#undef _MEMORY_MANAGEMENT_INVALID_RETAIN_COUNT_ATTRIBUTE

#undef _MEMORY_MANAGEMENT_RETAIN_COUNT_ATTRIBUTE
#undef _MEMORY_MANAGEMENT_ATOMIC_RETAIN
#undef _MEMORY_MANAGEMENT_ATOMIC_RELEASE

#undef _MEMORY_MANAGEMENT_CALL_DEALLOC_ATTRIBUTE
#undef _MEMORY_MANAGEMENT_CALL_DEALLOC
