/**
 * @file mygzip.c
 * @author Martin Weise <e1429167@student.tuwien.ac.at>
 * @date 22.11.2016
 *
 * @brief Main program module.
 *
 * This program demonstrates Doxygen and prints among a few unimportant lines of ASCII text
 * "Hello World" out on stdout.
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <sys/wait.h>
#include <stdarg.h>

/*
 *  ---------                ---------
 * | Parent  |              | Child   |
 * | pipe[0] |<=========    | pipe[0] |<~~~
 * | pipe[1] |>~       =    | pipe[1] |>= ~
 *  ---------  ~       =     ---------  = ~
 *             ~       =     ============ ~
 *    ~~~~~~~~~~       ======x========    ~
 *    ~ ======================       =    ~
 *    ~ =  _______________________   =    ~
 *    ~ =>/                    /==\===    ~
 *    ~~>|                    |~~~|~~~~~~~~
 *        .___________________\___/
 */

char *progname;

#define MESSAGE "hello world"

/* === Prototypes === */

/**
 * @copyright OS UE Team (server.c)
 * @param fmt
 */
void error_exit (const char *fmt, ...);

void usage();

void parse_args(int argc, char **argv, char **filename);

/**
 * Program entry point.
 * @brief The program executed starts here. TODO
 * @details TODO
 * global variables:
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return EXIT_SUCCESS | EXITFAILURE when operated like required.
 */
int main(int argc, char **argv) {
    char *filename = NULL;
    FILE *file;
    pid_t pid;
    int pipe1[2], pipe2[2];
    //        ^ 0 = read end
    //          1 = write end

    parse_args(argc, argv, &filename);

    if(pipe(pipe1) < 0 || pipe(pipe2) < 0){
        error_exit ("Can't create pipes.");
    }

    for (int i = 0; i < 2; i++) {
        printf("Child: %d\n", i+1);
        switch (pid = fork()) {
            case -1:
                error_exit("Cannot fork!");
            case 0:
                if (i == 0) {
                    // child
                    close (pipe1[0]);
                    if (dup2 (pipe1[1], STDIN_FILENO) < 0) {
                        error_exit ("Could not apply stdin to pipe1.");
                    }
//                    write (pipe1[1], "hello", sizeof("hello")+1);
                    close (pipe2[1]);
                    if (dup2 (pipe2[0], STDOUT_FILENO) < 0) {
                        error_exit ("Could not apply stdout to pipe2.");
                    }
                    /* stdout is closed indirect */
                    execlp("gzip", "gzip", "-cf", (char *)NULL);
//                    exit (EXIT_SUCCESS);
                } else {
                    // child 2
                    close (pipe1[0]);
                    close (pipe1[1]);
                    close (pipe2[1]);
                    char buffer[1024];

                    if (filename == NULL) {
                        file = stdout;
                        fprintf(file, "--->output\n");
                    } else {
                        // append binary
                        file = fopen(filename, "ab");
                        while (read(pipe2[0], buffer, sizeof buffer) < 0) {
                            fprintf (file, "%s", buffer);
                        }
                    }
                    exit (EXIT_SUCCESS);
                }
                exit(EXIT_SUCCESS);
                break;
            default:
                // parent
                if (i == 0) {
                    // child 1
                    // note: close (pipe1[0]) = read end would affect childs!
                    char buffer[1024];
                    /* read data from stdin */
                    while(fgets(buffer, sizeof buffer, stdin) != NULL) {
                        fprintf(stdout, "Read: %s", buffer);
                        write(pipe1[1], buffer, strlen(buffer)+1);
                    }
                    close (pipe1[1]);
                    /* write data to first pipe (gzip) */
                } else {
                    // from child 2
                    close (pipe2[0]);
                    close (pipe2[1]);
                }
                wait(NULL); // important
        }
    }

    return EXIT_SUCCESS;
}

void parse_args (int argc, char **argv, char **filename) {
    if (argc > 2) {
        usage();
    }
    progname = argv[0];
    if (argc == 2) {
        *filename = argv[1];
    }
}

void error_exit (const char *fmt, ...) {
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
}

void usage() {
    fprintf (stderr, "USAGE: %s [file]\n", progname);
    exit (EXIT_FAILURE);
}
