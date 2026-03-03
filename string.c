#include <stdint.h>

uint32_t strlen(const char *s) {
    const char *p = s;
    while (*p) p++;
    return p - s;
}