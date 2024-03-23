#include <stdio.h>
#include <string.h>

#include "Init.h"

char Greeting[] = "Hello world";

static void MyConstructor1(void) {
    strncpy(Greeting, "Hello mom!", sizeof(Greeting));
}

static void MyConstructor2(void) {
    strncpy(Greeting, "Late bird!", sizeof(Greeting));
}

static void MyConstructor3(void) {
    strncpy(Greeting, "Sad guy!", sizeof(Greeting));
}

REGISTER_DEPENDENT_CONSTRUCTOR(MyConstructor3, MyConstructor1, MyConstructor2);

REGISTER_DEPENDENT_CONSTRUCTOR(MyConstructor2, MyConstructor1);

REGISTER_INDEPENDENT_CONSTRUCTOR(MyConstructor1);


int main(void) {
    RunInitializationFunctions();

    printf("%s\n", Greeting);

    assert(strcmp(Greeting, "Sad guy!") == 0);

    return 0;
}