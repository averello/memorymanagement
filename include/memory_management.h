//
//  memory_management.h
//
//  Created by George Boumis on 12/10/13.
//  Copyright (c) 2013 George Boumis. All rights reserved.
//

#ifndef _memory_management_h
#define _memory_management_h

#define _XOPEN_SOURCE 700

#include <pthread.h>

#ifdef _cplusplus 
extern "C" {
#endif

#define MEMORY_MANAGEMENT_PROTOTYPE (memory_management_prototype)
#define MEMORY_MANAGEMENT_TYPE struct memory_management_attributes
#define MEMORY_MANAGEMENT_INFERED_OPAQUE_SIZE ( (sizeof(unsigned long long) + (1 * sizeof(void*))) / sizeof(void*))
#define MEMORY_MANAGEMENT_ISA(o) ((o)->_isa)
#define MEMORY_MANAGEMENT_RETAIN(o) memory_management_retain((o))
#define MEMORY_MANAGEMENT_RELEASE(o) memory_management_release((o))

/* Use this to set enable memory management on you structure */
#define MEMORY_MANAGEMENT_ENABLE() MEMORY_MANAGEMENT_TYPE _isa
	
/* Use this to initialize the memory management system on you allocated instance */
#define MEMORY_MANAGEMENT_INITIALIZE(o) MEMORY_MANAGEMENT_ISA(o) = MEMORY_MANAGEMENT_PROTOTYPE;

	
/* Use this to retain (increment) a reference */
#define retain(o) MEMORY_MANAGEMENT_RETAIN((o))
/* Use this to release (decrement) a reference */
#define release(o) MEMORY_MANAGEMENT_RELEASE((o))
 
	
//typedef void *(*retainf)(void *);
//typedef void (*releasef)(void *);
	
void *memory_management_retain(void *);
void memory_management_release(void *);
unsigned long long memory_management_get_retain_count(void *);

/* Use this to set a custom destructor */
typedef void (*deallocf)(void *);
void memory_management_attributes_set_dealloc_function(void *, deallocf);

	
MEMORY_MANAGEMENT_TYPE {
	void *_opaque[MEMORY_MANAGEMENT_INFERED_OPAQUE_SIZE];
};

extern MEMORY_MANAGEMENT_TYPE MEMORY_MANAGEMENT_PROTOTYPE;

#undef MEMORY_MANAGEMENT_INFERED_OPAQUE_SIZE
	
#ifdef _cplusplus 
}
#endif /* _cplusplus */
#endif /* _memory_management_h */
