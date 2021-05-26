memorymanagement
================

A little library in C for managing memory based on a reference counting system.

Documentation
-------------
This project uses [doxygen](http://www.stack.nl/~dimitri/doxygen/index.html)
(http://www.stack.nl/~dimitri/doxygen/index.html) for the code documentation.
Just point doxygen to the doc/Doxyfile and the html documentation will be
generated.

Command line exemple :
```bash
cd doc
doxygen Doxyfile
```

Now open the file html/index.html

Build
-----
Use `make` directly in the root of the project.  
The resulting files will be placed in the directory `lib`.


Usage
-----
1) Allocate a structure with memory management enabled:
```c
struct mystruct *ms = MEMORY_MANAGEMENT_ALLOC(sizeof(struct mystruct));
```
2) Optionnaly specify a dealloc (cleanup) function:
```c
MEMORY_MANAGEMENT_ATTRIBUTE_SET_DEALLOC_FUNCTION(mystruct, deallocf);
```
3) When you want to become an owner of this strucure:
```c
retain(mystruct);
```
4) When you finished with it:
```c
release(mystruct);
```
If the reference count hits 0 with this call then the dealloc function (if you specified any) will be called and then the structure will be freed.

