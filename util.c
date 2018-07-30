#include "util.h"
#include "legato.h"

le_result_t readFromFile(const char* path,
                         void* value,
                         int (*scanCallback)(FILE* f, void* val)) {
  FILE* f = fopen(path, "r");
  if (f == NULL) {
    LE_WARN("Couldn't open '%s' - %m", path);
    return LE_IO_ERROR;
  }
  int numScanned = scanCallback(f, value);
  if (numScanned != 1)
    return LE_FORMAT_ERROR;
  fclose(f);
  return LE_OK;
}

int scanIntCallback(FILE* f, void* value) {
  int* val = value;
  return fscanf(f, "%d", val);
}

int scanDoubleCallback(FILE* f, void* value) {
  double* val = value;
  return fscanf(f, "%lf", val);
}

le_result_t ioutil_readIntFromFile(const char* path, int* value) {
  return readFromFile(path, value, scanIntCallback);
}

le_result_t ioutil_readDoubleFromFile(const char* path, double* value) {
  return readFromFile(path, value, scanDoubleCallback);
}

le_result_t writeToFile(const char* path,
                        void* value,
                        size_t size,
                        size_t count,
                        const char* openMode) {
  LE_INFO("Attempting to write to %s (%d elements of size %d)", path, size,
          count);
  FILE* f = fopen(path, openMode);
  if (f == NULL) {
    LE_WARN("Failed to open %s for writing", path);
    return LE_IO_ERROR;
  }
  ssize_t nWritten = fwrite(value, size, count, f);
  fflush(f);
  LE_INFO("Wrote %d bytes", nWritten);
  fclose(f);
  return LE_OK;
}

le_result_t ioutil_writeToFile(const char* path,
                               void* value,
                               size_t size,
                               size_t count) {
  return writeToFile(path, value, size, count, "w");
}

le_result_t ioutil_appendToFile(const char* path,
                                void* value,
                                size_t size,
                                size_t count) {
  return writeToFile(path, value, size, count, "a");
}

le_result_t util_flattenRes(le_result_t* res, int nRes) {
  for (int i = 0; i < nRes; i++) {
    if (res[i] != LE_OK)
      return res[i];
  }
  return LE_OK;
}

int util_getUnixDatetime() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec;
}

/**
 * Get the last modified datetime
 * of the file at path
 */
time_t util_getMTime(char* path) {
  struct stat st;
  if (stat(path, &st) != 0)
    return -1;
  else
    return st.st_mtime;
}

bool util_fileExists(const char* path) {
  struct stat st;
  return stat(path, &st) == 0;
}

bool util_alreadyMounted(const char* devPath) {
  char content[2048];
  FILE* f = fopen("/proc/mounts", "r");
  if (f == NULL) {
    return false;
  }
  fread(content, sizeof(char), sizeof(content), f);
  // if this ref is non-null, we found a match
  return strstr(content, devPath) != NULL;
}

uint64_t GetCurrentTimestamp(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t utcMilliSec =
      (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
  return utcMilliSec;
}
//
// TODO verify this is working
le_result_t gpio_exportPin(char* pin) {
  int len = strlen(pin);
  LE_INFO("pin length str: %d", len);
  return ioutil_writeToFile("/sys/class/gpio/export", &pin, sizeof(char), len);
}

le_result_t gpio_unexportPin(char* pin) {
  int len = strlen(pin);
  return ioutil_writeToFile("/sys/class/gpio/unexport", &pin, sizeof(char),
                            len);
}

void getGpioPath(char* outputStr, char* pin, char* subDir) {
  sprintf(outputStr, "/sys/class/gpio/gpio%s/%s", pin, subDir);
}

le_result_t gpio_setDirection(char* pin, char* direction) {
  char path[100];
  getGpioPath(path, pin, "direction");
  return ioutil_writeToFile(path, direction, sizeof(char), strlen(direction));
}

le_result_t gpio_setPull(char* pin, char* pull) {
  char path[100];
  getGpioPath(path, pin, "pull");
  return ioutil_writeToFile(path, pull, sizeof(char), strlen(pull));
}

le_result_t gpio_pullDown(char* pin) {
  return gpio_setPull(pin, "down");
}

le_result_t gpio_pullUp(char* pin) {
  return gpio_setPull(pin, "up");
}

le_result_t gpio_setInput(char* pin) {
  return gpio_setDirection(pin, "in");
}

le_result_t gpio_setOutput(char* pin) {
  return gpio_setDirection(pin, "out");
}

le_result_t gpio_setActiveState(char* pin, bool isActiveLow) {
  // Any non zero value toggles the existing value
  // so we must read the existing value first
  char path[100];
  int isActiveLowSet;
  getGpioPath(path, pin, "active_low");
  le_result_t readRes = ioutil_readIntFromFile(path, &isActiveLowSet);
  le_result_t writeRes = LE_OK;
  if (isActiveLow != isActiveLowSet) {
    writeRes = ioutil_writeToFile(path, "1", sizeof(char), 1);
  }
  return readRes == LE_OK && writeRes == LE_OK ? LE_OK : LE_FAULT;
}

le_result_t gpio_isActive(char* pin, bool* isActive) {
  char path[50];
  getGpioPath(path, pin, "value");
  int val;
  le_result_t readRes = ioutil_readIntFromFile(path, &val);
  *isActive = val;
  return readRes;
}

le_result_t gpio_setValue(char* pin, bool state) {
  char path[50];
  getGpioPath(path, pin, "value");
  char* strState = state ? "1" : "0";
  return ioutil_writeToFile(path, &strState, sizeof(char), strlen(strState));
}

le_result_t gpio_setLow(char* pin) {
  return gpio_setValue(pin, LOW);
}

le_result_t gpio_setHigh(char* pin) {
  return gpio_setValue(pin, HIGH);
}

void util_mapGeneric(Functional* f, bool shouldFilter) {
  f->args.i = 0;
  while (f->args.i < f->n) {
    // if shouldFilter is false,
    // we automatically consider it to be in
    // the collection
    bool inCollection = f->callback(&f->args) || !shouldFilter;
    if (inCollection) {
      f->args.i++;
    } else {
      (f->n)--;
    }
  }
}

void util_map(Functional* f) {
  util_mapGeneric(f, false);
}

void util_filter(Functional* f) {
  util_mapGeneric(f, true);
}

void* util_find(Functional* f) {
  f->args.i = 0;
  while (f->args.i < f->n) {
    if (f->callback(&f->args)) {
      return f->derefCallback(f->args.i, f->args.arr);
    }
    f->args.i++;
  }
  return NULL;
}
