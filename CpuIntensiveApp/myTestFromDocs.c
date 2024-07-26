#include <papi.h>
#include <stdio.h>
#include <stdlib.h>

void handle_error (int retval)
{
    printf("PAPI error %d: %s\n", retval, PAPI_strerror(retval));
    exit(1);
}
	
int main()
{
    int retval, EventSet = PAPI_NULL;
    unsigned int native = 0x0;
    PAPI_event_info_t info;
		 
    /* Initialize the library */
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT)
        handle_error(retval);
	
    /* Create an EventSet */
    retval = PAPI_create_eventset(&EventSet);
    if (retval != PAPI_OK)
        handle_error(retval);
	
    /* Find the first available native event */
    native = PAPI_NATIVE_MASK | 0;
    retval = PAPI_enum_event(&native, PAPI_ENUM_FIRST);
    if (retval != PAPI_OK)
        handle_error(retval);
	
    /* Add it to the eventset */
    retval = PAPI_add_event(EventSet, native);
    if (retval != PAPI_OK)
        handle_error(retval);
	
    /* Executes if all low-level PAPI
    function calls returned PAPI_OK */
    printf("\033[0;32mPASSED\n\033[0m");
    exit(0); 
}
