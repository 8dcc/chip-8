
#ifndef UTIL_H_
#define UTIL_H_ 1

#define LENGTH(ARR) (sizeof(ARR) / (sizeof((ARR)[0])))

/* Wrapper for err_msg() */
#define ERR(...) err_msg(__func__, __VA_ARGS__)

#ifdef PRINT_INSTRUCTIONS
#define PRNT_I(...)              \
    do {                         \
        printf("%X:\t", ctx->PC); \
        printf(__VA_ARGS__);     \
        putchar('\n');           \
    } while (0)
#else
#define PRNT_I(...)
#endif

/*----------------------------------------------------------------------------*/

/* Print error message to stderr, call all the relevant SDL functions, and exit
 * the program. */
void die(const char* fmt, ...);

/* Print error message to stderr, along with the function name */
void err_msg(const char* func, const char* fmt, ...);

#endif /* UTIL_H_ */
