/*!
 *  @file memory_management.h
 *  @brief Memory Management Module.
 *  @details This module used to manage memory using a reference count system.
 *
 *  Created by @author George Boumis
 *  @date 2013/10/12.
 *	@version 1.0
 *  @copyright Copyright (c) 2013 George Boumis <developer.george.boumis@gmail.com>. All rights reserved.
 *
 *  @defgroup mm Memory Management Module
 *
 *	@mainpage Memory Management Documentation
 *	@section intro_sec Introduction
 *	A little library in `C` for managing memory based on a reference counting system. See @ref mm for documentation of this library.
 *	@section usage_sec Usage
 *	1. Allocate a structure with memory management enabled:
 *	~~~~~~~~~~~~~~~~~
 *	struct mystruct *ms = MEMORY_MANAGEMENT_ALLOC(sizeof(struct mystruct));
 *	~~~~~~~~~~~~~~~~~
 *	2. Optionnaly specify a dealloc (cleanup) function:
 *	~~~~~~~~~~~~~~~~~
 *	MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION(mystruct, deallocf);
 *	~~~~~~~~~~~~~~~~~
 *	3. When you want to become an owner of this strucure:
 *	~~~~~~~~~~~~~~~~~
 *	retain(mystruct);
 *	~~~~~~~~~~~~~~~~~
 *	4. When you finished with it:
 *	~~~~~~~~~~~~~~~~~
 *	release(mystruct);
 *	~~~~~~~~~~~~~~~~~
 *	If the reference count hits 0 with this call then the dealloc function (if you specified any) will be called and then the structure will be freed.
 *
 */


#ifndef _memory_management_h
#define _memory_management_h

#define _XOPEN_SOURCE 700
#define MEMORY_MANAGEMENT_ENVIRONMENT 1

#include <pthread.h>

#ifdef __cplusplus
//extern "C" {
#endif

#define MEMORY_MANAGEMENT_RETAIN(o) memory_management_retain((o))
#define MEMORY_MANAGEMENT_RELEASE(o) memory_management_release((o))


/*!
 *  @def MEMORY_MANAGEMENT_GET_RETAIN_COUNT(o)
 *	@brief Gets the reference count of an object
 *  @ingroup mm
 *	@returns the reference count
 */
#define MEMORY_MANAGEMENT_GET_RETAIN_COUNT(o) memory_management_get_retain_count((o))
	
/*!
 *  @def MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION(o, function)
 *	@brief Sets a dealloc function for cleanup
 *  @ingroup mm
 *	@returns the allocated instance
 */
#define MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION(o, function) memory_management_attributes_set_dealloc_function((o), (function))

/*!
 *  @def MEMORY_MANAGEMENT_ALLOC(size)
 *	@brief Allocates a new instance with memory management enabled.
 *  @ingroup mm
 *	@returns the allocated instance
 */
#define MEMORY_MANAGEMENT_ALLOC(size) memory_management_alloc((size))

/*!
 *  @def retain(o)
 *	@brief Increment the reference count of an object
 *  @ingroup mm
 *	@returns o
 */
#define retain(o) MEMORY_MANAGEMENT_RETAIN((o))

/*!
 *  @def release(o)
 *	@brief Decrement the reference count of an object
 *  @ingroup mm
 */
#define release(o) MEMORY_MANAGEMENT_RELEASE((o))

/*!
 *  @fn void *memory_management_alloc(size_t size) __attribute__ ((malloc))
 *  @brief Allocates an instance of the specified size.
 *  @ingroup mm
 *	@param[in] size the size to be allocated
 *	@returns  If successful this function return a pointer to allocated memory. If there is an error, they return a `NULL` pointer and set errno to **ENOMEM**.
 */
void *memory_management_alloc(size_t size) __attribute__ ((malloc));

/*!
 *  @fn void *memory_management_retain(void *object) __attribute__((nonnull (1)))
 *  @brief Increments an object's reference count. Use @ref retain or @ref MEMORY_MANAGEMENT_RETAIN instead.
 *  @ingroup mm
 *	@details You use this function with an object when you want to prevent it from being deallocated until you have finished using it.
 *
 *	An object is deallocated automatically when its reference count reaches 0. @ref retain calls increment the reference count, and @ref release messages decrement it.
 *
 *	As a convenience, this function returns the object pointer because it may be used in nested expressions.
 *	@param[in] object the object to increment its reference count
 *	@returns the object pointer
 */
void *memory_management_retain(void *object) __attribute__((nonnull (1)));

/*!
 *  @fn void memory_management_release(void *object) __attribute__((nonnull (1)))
 *  @brief Use @ref release or @ref MEMORY_MANAGEMENT_RELEASE instead. Decrements an object's reference count.
 *  @ingroup mm
 *	@details The object is freed when its reference count reaches 0. If a dealloc function was specified with @ref MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION then you are given a chance to make some cleanup before the pointer is freed.
 *
 *	@param[in] object the object to decrement its reference count
 */
void memory_management_release(void *object) __attribute__((nonnull (1)));

/*!
 *  @fn unsigned int memory_management_get_retain_count(const void *object) __attribute__((nonnull (1)))
 *  @brief Do not use this function. Use @ref MEMORY_MANAGEMENT_GET_RETAIN_COUNT instead.
 *  @ingroup mm
 *	@details This function is of no value in debugging memory management issues. It is very unlikely that you can get useful information from this method.
 *	@param[in] object the object to get its reference count
 *	@returns the object's reference count
 */
unsigned int memory_management_get_retain_count(const void *object) __attribute__((nonnull (1)));

/*!
 *  @typedef typedef void (*deallocf)(void *) __attribute__((nonnull (1)))
 *  @brief The prototype of a dealloc function.
 *  @ingroup mm
 */
typedef void (*deallocf)(void *) __attribute__((nonnull (1)));

/*!
 *  @fn void memory_management_attributes_set_dealloc_function(void *, deallocf) __attribute__((nonnull (1)))
 *  @brief Attributes a deallocation function to the object. Use @ref MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION instead.
 *  @ingroup mm
 *	@details This function sets the dealloc function for the object. The dealloc function is called **immediately** when the reference count reaches 0. The dealloc function should be used to relase any memory retained by the object.
 *	@param[in] object the object
  *	@param[in] function the dealloc function
 */
void memory_management_attributes_set_dealloc_function(void *object, deallocf function) __attribute__((nonnull (1)));
	
#ifdef __cplusplus
//}
#endif /* _cplusplus */
#endif /* _memory_management_h */
