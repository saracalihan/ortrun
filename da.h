

#ifndef DA_H
#define DA_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DA_INIT(da, size) \
do{ \
    (da).items    = malloc(sizeof((*(da).items)) * (size)); \
    if (!(da).items) { \
            printf("Memory allocation failed\n"); \
            exit(EXIT_FAILURE); \
        } \
    (da).capacity = size; \
    (da).count    = 0; \
}while(0);

#define DA_EXPAND(da, time) \
do { \
    (da).capacity = (da).capacity == 0 ? 1 : (da).capacity*(time); \
    (da).items = realloc((da).items, (da).capacity * sizeof(*(da).items)); \
    memset(&(da).items[(da).count], 0, sizeof(*(da).items) * ((da).capacity - (da). count)); \
    if (!(da).items) { \
        printf("Memory allocation failed\n"); \
        exit(EXIT_FAILURE); \
    } \
} while(0);

#define DA_PUSH(da, data) \
do{\
  if ((da).count >= (da).capacity) { \
      DA_EXPAND((da),2) \
      if (!(da).items) { \
          printf("Memory allocation failed\n"); \
          exit(EXIT_FAILURE); \
      } \
  } \
  (da).items[(da).count++] = data; \
} while(0);

#define DA_POP(da) ((da).items[--((da).count)])

#define DA_SHIFT(da) \
({ \
    typeof((da).items[0]) res =(da).items[0]; \
    if((da).count > 0){ \
        memmove((da).items, (da).items + 1, --(da).count * sizeof(*(da).items));\
    }\
    res; \
})

#define DA_UNSHIFT(da, item) \
do { \
    DA_EXPAND(da,2); \
    memmove((da).items + 1, (da).items, (da).count * sizeof(*(da).items)); \
    (da).items[0] = (item); \
    (da).count++; \
} while(0)

#define DA_REMOVE(da, index) \
do { \
    if ((size_t)(index) < (da).count) { \
        memmove((da).items + (index), (da).items + (index) + 1, ((da).count - (index) - 1) * sizeof(*(da).items)); \
        (da).count--; \
    } \
} while(0)

#define DA_CONCAT(dest, src) \
do { \
    if((src).count >0){ \
      /* if dest smaller then src, realloc dest */ \
      if((dest).count + (dest).capacity < (src).count){ \
        (dest).capacity = (dest).count + (src).count; \
        (dest).items = realloc((dest).items, sizeof(*((dest).items)) * (dest).capacity); \
      } \
      memcpy( \
        (dest).items + ((dest).count == 0 ? 0 : (dest).count), \
        (src).items, \
        sizeof(*((dest).items))*(src).count \
      ); \
      (dest).count+= (src).count; \
    } \
} while(0)
#endif

#define DA_GET(da, index) ((da).items[index])

#define DA_GET_LAST(da) ((da).items[(da).count -1])

#define DA_FREE(da) \
do { \
    if((da).items != NULL){ \
        free((da).items); \
    } \
} while(0)
