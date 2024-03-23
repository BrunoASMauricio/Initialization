#include <stdio.h>
#include <string.h>

#include "Init.h"

char ExternalState[] = "Hello world";
int StateSize = sizeof(ExternalState);

int main(void) {
    RunInitializationFunctions();

    printf("%s\n", ExternalState);

    assert(strcmp(ExternalState, "Sad guy!") == 0);

    return 0;
}