#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int _close(int file) {
    return -1;
}

int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int file) {
    return 1;
}

int _lseek(int file, int ptr, int dir) {
    return 0;
}

int _read(int file, char *ptr, int len) {
    errno = EIO;
    return -1;
}

int _write(int file, char *ptr, int len) {
    // сюда можно добавить передачу в UART, если нужно
    return len;
}

void _exit(int status) {
    while(1);
}

int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

int _getpid(void) {
    return 1;
}
