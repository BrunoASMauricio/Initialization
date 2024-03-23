#ifndef INIT
#define INIT

#include "BasicList.h"
#include "Common.h"
#include <stdint.h>
#include <pthread.h> 

#define BEFORE_MAIN __attribute__ ((constructor))

// Handlers' memory location is its' own identifier (memory is shared so addresses are unique)
typedef void (*CONSTRUCTOR_HANDLER)(void);

TYPE_STRUCT(INIT_INFORMATION) {
    CONSTRUCTOR_HANDLER Handler;
    OPAQUE_MEMORY* Dependencies;
    size_t PendingDependencies;
    char* Location; // __func__  __FILE__ __LINE__
};

/** Register a constructor, its' ID and dependent IDs
  * 
  */
// It is assumed that constructors run in a single thread (need to validate this assumption for all cases but how)
void RegisterConstructor(const char Location[], CONSTRUCTOR_HANDLER Handler,
                         OPAQUE_MEMORY Dependencies);

// void RegisterConstructor(CONSTRUCTOR_HANDLER Handler);

#define REGISTER_DEPENDENT_CONSTRUCTOR(Handler, ...)                              \
static void BEFORE_MAIN GLUE1(RegisterConstructor, __COUNTER__)(void) {           \
    RegisterConstructor(                                                          \
      STR(Handler) "(void) from " __FILE__, Handler,                              \
      (OPAQUE_MEMORY){COUNT_ARGUMENTS(__VA_ARGS__) * sizeof(CONSTRUCTOR_HANDLER), \
                      FALSE, (void*[]){__VA_ARGS__}});                            \
}

#define REGISTER_INDEPENDENT_CONSTRUCTOR(Handler)                       \
static void BEFORE_MAIN GLUE1(RegisterConstructor, __COUNTER__)(void) { \
    RegisterConstructor(                                                \
      STR(Handler) "(void) from " __FILE__, Handler,                    \
      (OPAQUE_MEMORY){0, FALSE, NULL});                                 \
}

void RunInitializationFunctions(void);

OPAQUE_MEMORY OrganizeInitInformation(void);

void ReleaseInitInfo(void);

void MyConstructor1(void);
void MyConstructor2(void);
void MyConstructor3(void);

#define Constructor1ID MyConstructor1
#define Constructor2ID MyConstructor2
#define Constructor3ID MyConstructor3

#endif