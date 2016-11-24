/* === Macros === */

#if defined(ENDEBUG)
#define DEBUG(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG(...)
#endif

/* === Prototypes === */

static void error_exit (const char *fmt, ...);

static void usage(void);

static void cleanup_resources(void);

static void copy_contents (FILE *from, FILE *to);
