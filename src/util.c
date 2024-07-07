
#include <stdarg.h>
#include <stdio.h>
#include "include/main.h"
#include "include/util.h"

void die(const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    vfprintf(stderr, fmt, va);
    putc('\n', stderr);

    if (g_cpu_ctx != NULL)
        cpu_free(g_cpu_ctx);

    if (g_window != NULL)
        SDL_DestroyWindow(g_window);

    SDL_Quit();
    exit(1);
}

void err_msg(const char* func, const char* fmt, ...) {
    va_list va;
    va_start(va, fmt);

    fprintf(stderr, "%s: ", func);
    vfprintf(stderr, fmt, va);
    fputc('\n', stderr);

    va_end(va);
}
