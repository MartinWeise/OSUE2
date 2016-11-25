/**
 * @file mygzip.c
 * @author Martin Weise <e1429167@student.tuwien.ac.at>
 * @date 22.11.2016
 *
 * @brief Main program module.
 *
 *
 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <assert.h>


/* === Constants === */

/** The buffer size. */
#define BUFFER_SIZE 512

/* === Macros === */

/**
 * @brief Provides a debugging function to output status messages
 * @details Activate/Deactivate by adding/removing -DENDEBUG to DEFS in Makefile.
 */
#if defined(ENDEBUG)
#define DEBUG(...) do { fprintf(stderr, __VA_ARGS__); } while(0)
#else
#define DEBUG(...)
#endif

/* === Prototypes === */

/**
 * @brief Exit the program with a message, free resources and append an error
 * message if available.
 * @param fmt Format string like used in printf(fmt,...)
 */
static void error_exit (const char *fmt, ...);

/**
 * @brief Shows the information how to use the program properly when used wrong
 */
static void usage(void);

/**
 * @brief Frees resources being used by the program
 * @details globals: input_writer, output, output_reader, pipe1, pipe2
 */
static void cleanup_resources(void);

/**
 * @brief Safely copy the content from @p from to @p to
 * @details This is done in buckets of size BUFFER_SIZE, append last few bytes as well
 * @param from File descriptor of source file.
 * @param to File descriptor of target file.
 */
static void copy_contents (FILE *from, FILE *to);

/* === Global Variables === */

/** @brief The program name, mygzip by default. */
static char *progname = "mygzip";
/** @brief The file descriptor to write output to. */
static FILE *output = NULL;
/** @brief Writer that is used for attaching stdin to pipe1.  */
static FILE *input_writer = NULL;
/** @brief Reader that is used for attaching pipe2 to the output file. */
static FILE *output_reader = NULL;
/** @brief The input pipe. */
static int pipe1[2];
/** @brief The output pipe. */
static int pipe2[2];
/** @brief Process ID of child 1. */
static pid_t child1;
/** @brief Process ID of child 2. */
static pid_t child2;

/* === Implementations === */

/**
 * The program entry point.
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return EXIT_SUCCESS | EXIT_FAILURE depending on the usage against the specification.
 */
int main(int argc, char **argv) {

    /* check arguments of program */
    progname = argv[0];
    output = stdout;
    if (argc > 2) {
        usage();
    }
    if (argc == 2) {
        /* try open the file */
        if ((output = fopen(argv[1], "w")) == NULL) {
            error_exit("Couldn't open the file %s.", argv[1]);
        }
    }

    /* create pipe 1 */
    if (pipe(&pipe2[0]) == -1) {
        error_exit("Couldn't create second pipe.");
    }

    /* fork for second child */
    switch (child2 = fork()) {
        case -1:
            error_exit ("Couldn't fork second child.");
            break;
        case 0:
            // child 2
            DEBUG("Child 2 was born.\n");
            close (pipe2[1]);

            if ((output_reader = fdopen(pipe2[0], "r")) == NULL) {
                error_exit("Couldn't fdopen pipe2.");
            }

            copy_contents(output_reader, output);

            if (fclose(output) == EOF) {
                error_exit("Couldn't close output file.");
            }
            fclose (output_reader);
            close (pipe2[0]);

            exit (EXIT_SUCCESS);
            break;
        default:
            /* parent, continue from here with fork of child 1 */
            break;
    }

    /* IMPORTANT: Create pipe here not above with other! */
    if (pipe(&pipe1[0]) == -1) {
        error_exit("Couldn't create first pipe.");
    }

    /* fork for first child */
    switch (child1 = fork()) {
        case -1:
            error_exit ("Couldn't fork first child.");
            break;
        case 0:
            // child 1
            DEBUG("Child 1 was born.\n");
            close (pipe1[1]);
            close (pipe2[0]);
            if (dup2(pipe1[0], fileno(stdin)) == -1) {
                error_exit("Couldn't bind stdin to pipe1.");
            }
            if (dup2(pipe2[1], fileno(stdout)) == -1) {
                error_exit("Couldn't bind stdin to pipe1.");
            }
            DEBUG("Starting GZIP.\n");
            execlp("gzip", "gzip", "-cf", (char *) 0);
            error_exit("Couldn't execute with execlp.");
            break;
        default:
            /* parent, continue from here */
            break;
    }

    /* parent process exclusive */
    if (getpid() == child1 || getpid() == child2) {
        error_exit("Something went wrong. Child ran away to parent section.");
    }

    close (pipe1[0]);
    close (pipe2[0]);
    close (pipe2[1]);

    if ((input_writer = fdopen(pipe1[1], "w")) == NULL) {
        error_exit("Couldn't open first pipe for writing.");
    }

    copy_contents(stdin, input_writer);

    if (fclose (input_writer) == EOF) {
        error_exit("Couldn't close input writer.");
    }
    input_writer = NULL;
    close (pipe1[1]);

    /* wait for children to come home to parent */
    int status;
    pid_t pid;

    /* check exit codes of children */
    for (int i = 1; i <= 2; i++) {
        pid = wait(&status);
        if (pid == child1 && WEXITSTATUS(status) != EXIT_SUCCESS) {
            error_exit("Child 1 returned with an error (gzip).");
        } else if (pid == child2 && WEXITSTATUS(status) != EXIT_SUCCESS) {
            error_exit("Child 2 returned with an error (pipes).");
        } else if (pid != child1 && pid != child2) {
            error_exit("Couldn't wait for children.");
        }
    }

    return EXIT_SUCCESS;
}

static void copy_contents (FILE *from, FILE *to) {
    uint8_t buffer[BUFFER_SIZE];
    int length;

    /* read buffer-wise from source file and write buffer-wise to target file */
    while ((length = fread(&buffer, sizeof buffer[0], BUFFER_SIZE, from)) == BUFFER_SIZE) {
        if (fwrite(&buffer, sizeof (buffer[0]), BUFFER_SIZE, to) < BUFFER_SIZE) {
            error_exit("Couldn't write full buffer to target.");
        }
    }
    /* check for errors in stream */
    if (ferror(from) != 0) {
        error_exit("Error in source stream.");
    }

    /* got EOF but continue to write last bytes */
    if (fwrite(&buffer, sizeof buffer[0], length, to) < length) {
        error_exit("Couldn't write to target.");
    }

    /* now force write of buffered data */
    if (fflush(to) == EOF) {
        error_exit("Couldn't write buffered data with fflush.");
    }

}

static void error_exit (const char *fmt, ...) {
    va_list ap;

    (void) fprintf(stderr, "%s: ", progname);
    if (fmt != NULL) {
        va_start(ap, fmt);
        (void) vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    if (errno != 0) {
        (void) fprintf(stderr, ": %s", strerror(errno));
    }
    (void) fprintf(stderr, "\n");

    DEBUG("Shutting down now.\n");
    cleanup_resources();
    exit (EXIT_FAILURE);
}

static void usage(void) {
    fprintf (stderr, "USAGE: %s [file]\n", progname);
    exit (EXIT_FAILURE);
}

static void cleanup_resources(void) {
    DEBUG("Freeing resources\n");
    if (output != NULL) {
        fclose(output);
    }
    if (input_writer != NULL) {
        fclose (input_writer);
    }
    if (output_reader != NULL) {
        fclose (input_writer);
    }
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[0]);
    close(pipe2[1]);
}
