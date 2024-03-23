#include "Init.h"

LIST* InitInfoList = NULL;

static BOOLEAN HandlerDependsOn(INIT_INFORMATION* InitInfo,
                                CONSTRUCTOR_HANDLER Dependency);

static OPAQUE_MEMORY OrganizeInitInformation(void);

static void ReleaseInitInfo(void);


void RegisterConstructor(const char Location[], CONSTRUCTOR_HANDLER Handler, OPAQUE_MEMORY Dependencies){
    ALLOC_STRUCT(INIT_INFORMATION, NewEntry);

    if (InitInfoList == NULL) {
        InitInfoList = AllocateList();
    }

    // Info allocation
    NewEntry->Handler  = Handler;
    NewEntry->Location = Malloc(strlen(Location) + 1);
    Memcpy(NewEntry->Location, Location, strlen(Location) + 1);
    NewEntry->Dependencies = AllocateOpaqueMemory(Dependencies.Size);
    NewEntry->PendingDependencies = Dependencies.Size / sizeof(CONSTRUCTOR_HANDLER);
    CopyOpaqueMemory(NewEntry->Dependencies, &Dependencies);

    DataListInsert(InitInfoList, GENERIC_DATA(pointer, NewEntry));
}

void RunInitializationFunctions(void) {
    uint64_t HandlerAmmount;
    CONSTRUCTOR_HANDLER* HandlerArray;

    // Organize handlers
    OPAQUE_MEMORY SerializedInitHandlers = OrganizeInitInformation();
    HandlerArray = SerializedInitHandlers.Data;

    // Run all handlers
    HandlerAmmount = SerializedInitHandlers.Size / sizeof(CONSTRUCTOR_HANDLER);
    for(uint64_t HandlerInd = 0; HandlerInd != HandlerAmmount; HandlerInd++) {
        HandlerArray[HandlerInd]();
    }

    // Release memory
    ClearOpaqueMemory(&SerializedInitHandlers);

    ReleaseInitInfo();
}

static OPAQUE_MEMORY OrganizeInitInformation(void) {
    OPAQUE_MEMORY SerializedInitHandlers;
    CONSTRUCTOR_HANDLER* HandlerArray;
    INIT_INFORMATION* InitInfo;
    BOOLEAN AddedHandler = TRUE;
    uint64_t InsertedAmmount = 0;

    SetupOpaqueMemory(&SerializedInitHandlers, InitInfoList->Length * sizeof(CONSTRUCTOR_HANDLER));
    HandlerArray = SerializedInitHandlers.Data;
    
    printf("Handler order:\n");
    while(AddedHandler == TRUE) {
        AddedHandler = FALSE;

        ITERATE_PRIMITIVE_DATA_TYPE(InitInfoList, pointer, InitInfo) {
            if (InitInfo->Location == NULL) {
                continue;
            }

            // Check if there are still dependencies not in the array
            if (InitInfo->PendingDependencies == 0) {
                HandlerArray[InsertedAmmount++] = InitInfo->Handler;
                AddedHandler = TRUE;

                printf("[%ld]: %s\n", InsertedAmmount, InitInfo->Location);

                // Instead of removing from list we just "disable" it
                Free(InitInfo->Location);
                FreeOpaqueMemory(InitInfo->Dependencies);
                InitInfo->Location = NULL;
                InitInfo->Dependencies = NULL;
            }
        }

        // Update pending dependencies
        ITERATE_PRIMITIVE_DATA_TYPE(InitInfoList, pointer, InitInfo) {
            if (InitInfo->Dependencies == NULL) {
                continue;
            }
            InitInfo->PendingDependencies = InitInfo->Dependencies->Size/sizeof(CONSTRUCTOR_HANDLER);
            for(uint64_t HandlerInd = 0; HandlerInd != InsertedAmmount; HandlerInd++) {
                if (HandlerDependsOn(InitInfo, HandlerArray[HandlerInd]) == TRUE) {
                    InitInfo->PendingDependencies--;
                }
            }
        }
    }

    // All constructors must have been added
    assert (InsertedAmmount == InitInfoList->Length);

    return SerializedInitHandlers;
}

static BOOLEAN HandlerDependsOn(INIT_INFORMATION* InitInfo, CONSTRUCTOR_HANDLER Dependency) {
    CONSTRUCTOR_HANDLER* HandlerArray = InitInfo->Dependencies->Data;

    uint64_t HandlerAmmount = InitInfo->Dependencies->Size / sizeof(CONSTRUCTOR_HANDLER);
    for(uint64_t HandlerInd = 0; HandlerInd != HandlerAmmount; HandlerInd++) {
        if (HandlerArray[HandlerInd] == Dependency) {
            return TRUE;
        }
    }
    return FALSE;
}

static void ReleaseInitInfo(void) {
    INIT_INFORMATION* InitInfo;

    ITERATE_PRIMITIVE_DATA_TYPE(InitInfoList, pointer, InitInfo) {
        Free(InitInfo);
    }
    FreeDataList(InitInfoList);
}