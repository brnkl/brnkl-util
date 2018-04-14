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

le_result_t ioutil_writeToFile(const char* path,
                               void* value,
                               size_t size,
                               size_t count) {
  LE_INFO("Attempting to write to %s", path);
  FILE* f = fopen(path, "w");
  if (f == NULL) {
    LE_WARN("Failed to open %s for writing", path);
    return LE_IO_ERROR;
  }
  ssize_t nWritten = fwrite(value, size, count, f);
  LE_INFO("Wrote %d bytes", nWritten);
  fclose(f);
  return LE_OK;
}

le_result_t util_flattenRes(le_result_t* res, int nRes) {
  for (int i = 0; i < nRes; i++) {
    le_result_t* resPtr = res + i;
    if (*resPtr != LE_OK)
      return *resPtr;
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

uint64_t GetCurrentTimestamp(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uint64_t utcMilliSec =
      (uint64_t)(tv.tv_sec) * 1000 + (uint64_t)(tv.tv_usec) / 1000;
  return utcMilliSec;
}
