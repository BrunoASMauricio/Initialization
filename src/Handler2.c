#include "Init.h"

extern char ExternalState[];
extern int StateSize;

REGISTER_DEPENDENT_CONSTRUCTOR(MyConstructor2, Constructor1ID);

void MyConstructor2(void) {
    strncpy(ExternalState, "Late bird!", StateSize);
}
