#include <stdio.h>

#define LOG(level, format, ...) 	printf("[%s] [%s][%d] " format "\n", level, __FILE__, __LINE__, ##__VA_ARGS__);
#define LOG_NOTE(format, ...) 	LOG("note", format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) 	LOG("error", format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...)	LOG("warning", format, ##__VA_ARGS__)

#define CheckError(cond, format, ...) \
if ((cond)) { \
LOG_ERROR(format, ##__VA_ARGS__); \
} \

#define CheckErrorAndReturn(cond, ret, format, ...) \
if ((cond)) { \
LOG_ERROR(format, ##__VA_ARGS__); \
return ret; \
} \

#define CheckErrorAndHandle(cond, handle, format, ...) \
if ((cond)) { \
LOG_ERROR(format, ##__VA_ARGS__); \
handle(); \
} \

#define CheckErrorAndHandleAndReturn(cond, handle, ret, format, ...) \
if ((cond)) { \
LOG_ERROR(format, ##__VA_ARGS__); \
handle(); \
return ret; \
} \
