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

// TODO verify this is working
le_result_t gpio_exportPin(char* pin);
le_result_t gpio_unexportPin(char* pin);
void getGpioPath(char* outputStr, char* pin, char* subDir);
le_result_t gpio_setDirection(char* pin, char* direction);
le_result_t gpio_setInput(char* pin);
le_result_t gpio_setOutput(char* pin);
le_result_t gpio_setActiveState(char* pin, bool isActiveLow);
le_result_t gpio_isActive(char* pin, bool* isActive);
le_result_t gpio_setValue(char* pin, bool state);
le_result_t gpio_setLow(char* pin);
le_result_t gpio_setHigh(char* pin);
le_result_t gpio_setPull(char* pin, char* pull);
le_result_t gpio_pullDown(char* pin);
le_result_t gpio_pullUp(char* pin);
#endif
