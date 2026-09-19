#ifndef EXEC_H
#define EXEC_H

#include "../readline/parser.h"

#ifdef __cplusplus
extern "C" {
#endif

void exec_pipeline(pipeline_t *pipeline);

#ifdef __cplusplus
}
#endif

#endif
