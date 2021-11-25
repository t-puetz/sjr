#include "c_hlprfncts.h"

/* #################################################################### */
/* ======= START BASIC HELPER FUNCTION SIGNATURES FOR CODING ========== */
/* #################################################################### */
void free_valid_ptr(void* ptr)
{
    if(ptr)
        free(ptr);
        
}

void exit_w_one_on_failed_xalloc(void* ptr)
{
    if(!ptr) {
        printf("Out of memory!\n Exiting with 1!\n");
        exit(1);
    }
}
/* #################################################################### */
/* ======= START BASIC HELPER FUNCTION SIGNATURES FOR CODING ========== */
/* #################################################################### */
