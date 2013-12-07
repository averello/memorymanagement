//
//  memory_management.h
//
//  Created by George Boumis on 12/10/13.
//  Copyright (c) 2013 George Boumis. All rights reserved.
//

#ifndef _memory_management_h
#define _memory_management_h

#define _XOPEN_SOURCE 700
#define MEMORY_MANAGEMENT_ENVIRONMENT 1

#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MEMORY_MANAGEMENT_RETAIN(o) memory_management_retain((o))
#define MEMORY_MANAGEMENT_RELEASE(o) memory_management_release((o))
#define MEMORY_MANAGEMENT_GET_RETAIN_COUNT(o) memory_management_get_retain_count((o))
	
/* Use this to set the dealloc function */
#define MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION(o, function) memory_management_attributes_set_dealloc_function((o), (function))

/* Use this allocate memory */
#define MEMORY_MANAGEMENT_ALLOC(size) memory_management_alloc((size))
/* Use this to retain (increment) a reference */
#define retain(o) MEMORY_MANAGEMENT_RETAIN((o))
/* Use this to release (decrement) a reference */
#define release(o) MEMORY_MANAGEMENT_RELEASE((o))
	
	void *memory_management_alloc(size_t size) __attribute__ ((malloc));
	void *memory_management_retain(void *) __attribute__((nonnull (1)));
	void memory_management_release(void *) __attribute__((nonnull (1)));
	unsigned long long memory_management_get_retain_count(const void *) __attribute__((nonnull (1)));
	
	/* Use this to set a custom destructor */
	typedef void (*deallocf)(void *) __attribute__((nonnull (1)));
	void memory_management_attributes_set_dealloc_function(void *, deallocf) __attribute__((nonnull (1)));
	
#ifdef __cplusplus
}
#endif /* _cplusplus */
#endif /* _memory_management_h */
