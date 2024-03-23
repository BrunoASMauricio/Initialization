#include "Init.h"

extern char ExternalState[];
extern int StateSize;

REGISTER_DEPENDENT_CONSTRUCTOR(MyConstructor3, Constructor1ID, Constructor2ID);

void MyConstructor3(void) {
    strncpy(ExternalState, "Sad guy!", StateSize);
}
