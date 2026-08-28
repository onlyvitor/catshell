#include "utils.h"

void free_args(char **args){
    if(args == NULL){
        return;
    }

    for (int i = 0;args[i]!=NULL;i++){
        free(args[i]);
    }

    free(args);
}