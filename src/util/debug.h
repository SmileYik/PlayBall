#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG_FLAG

#include <stdio.h>
#define DEBUG_INFO(MSG) printf("[DEBUG] [INFO] %s\n", MSG)
#define DEBUG_WARN(MSG) printf("[DEBUG] [WARN] %s\n", MSG)
#define DEBUG_ERROR(MSG) printf("[DEBUG] [ERROR] %s\n", MSG)
#define DEBUG_MSG(MSG, ...) printf("[DEBUG] [MSG] " MSG "\n", __VA_ARGS__) 


#else

#define DEBUG_INFO(MSG)
#define DEBUG_WARN(MSG)
#define DEBUG_ERROR(MSG)
#define DEBUG_MSG(MSG, ...)

#endif // DEBUG_FLAG

#endif // DEBUG_H