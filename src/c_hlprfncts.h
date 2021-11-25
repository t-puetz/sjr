#ifndef C_HLPRFNCTS_H
#define C_HLPRFNCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

/* #################################################################### */
/* ======= START BASIC HELPER FUNCTION PROTOTYPES FOR CODING ========== */
/* #################################################################### */
void free_valid_ptr(void* ptr);
void exit_w_one_on_failed_xalloc(void* ptr);
/* #################################################################### */
/* ======= END BASIC HELPER FUNCTION PROTOTYPES FOR CODING ============ */
/* #################################################################### */
#endif
