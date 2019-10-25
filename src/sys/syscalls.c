#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
#include <sys/stat.h> // S_IFCHR
#include "hal.h"

#define PRINTF_STREAM_DEVICE (BaseSequentialStream*)&SD3

__attribute__((used))
int _write(int file, char *ptr, int len) {
   if ((file != STDOUT_FILENO) && (file != STDERR_FILENO)) {
      errno = EBADF;
      return -1;
   }

   int txferred = streamWrite(PRINTF_STREAM_DEVICE, ptr, len);

   return txferred;
}

__attribute__((used))
int _read(int file, char *ptr, int len) {
  return 0;
}

__attribute__((used))
int _close_r(struct _reent *r, int file) {
  return 0;
}

__attribute__((used))
caddr_t _sbrk_r(struct _reent *r, int incr) {
#if CH_CFG_USE_MEMCORE
  void *p;

  chDbgCheck(incr >= 0);

  p = chCoreAlloc((size_t)incr);
  if (p == NULL) {
    __errno_r(r) = ENOMEM;
    return (caddr_t)-1;
  }
  return (caddr_t)p;
#else
  (void)incr;
  __errno_r(r) = ENOMEM;
  return (caddr_t)-1;
#endif
}

__attribute__((used))
int _fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

__attribute__((used))
int _isatty(int file) {
	return 1;
}

__attribute__((used))
int _lseek(int file, int ptr, int dir) {
	return 0;
}
