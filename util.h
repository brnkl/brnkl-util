#ifndef UTIL_H
#define UTIL_H

#include "legato.h"

#define HIGH 1
#define LOW 0

// ioutil
le_result_t readFromFile(const char* path,
                         void* value,
                         int (*scanCallback)(FILE* f, void* val));
int scanIntCallback(FILE* f, void* value);
int scanDoubleCallback(FILE* f, void* value);
le_result_t ioutil_readIntFromFile(const char* path, int* value);
le_result_t ioutil_readDoubleFromFile(const char* filePath, double* value);
le_result_t ioutil_writeToFile(const char* path,
                               void* value,
                               size_t size,
                               size_t count);

uint64_t GetCurrentTimestamp(void);
time_t util_getMTime(char* path);
int util_getUnixDatetime();
le_result_t util_flattenRes(le_result_t* res, int nRes);
bool util_fileExists(const char* path);

#endif
