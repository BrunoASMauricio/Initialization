#include "Init.h"

extern char ExternalState[];
extern int StateSize;

REGISTER_INDEPENDENT_CONSTRUCTOR(MyConstructor1);

void MyConstructor1(void) {
    strncpy(ExternalState, "Hello mom!", StateSize);
}
