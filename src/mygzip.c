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

 void error_exit (char *message, ...);

 void usage(char **argv);

 void parse_args(int argc, char **argv, char **filename);

/**
 * Program entry point.
 * @brief The program executed starts here. TODO
 * @details TODO
 * global variables:
 * @param argc The argument counter.
 * @param argv The argument vector.
 * @return Returns EXIT_SUCCESS when operated like required.
 */
int main(int argc, char **argv) {
    char *filename;
    pid_t pid1, pid2;
    int pipe1[2], pipe2[2];
    //        ^ 0 = read end
    //          1 = write end
    FILE *file, *fdpipe;

    parse_args(argc, argv, &filename);

    if(pipe(pipe1) < 0 || pipe(pipe2) < 0){
        error_exit ("Can't create pipes.");
    }

    switch (pid1 = fork()) {
        case -1:
            error_exit ("Cannot fork!");
        case 0:
            // child 1
            printf ("child 1.\n");
            /* close write end */
            break;
        default:
            // parent
            switch (pid2 = fork()) {
                case -1:
                    error_exit ("Cannot fork!");
                case 0:
                    // child 2
                    printf ("child 2.\n");
                    break;
                default:
                    // still parent
                    printf ("Parent 2.\n");
                    break;
            }
            // Parent
            printf("Parent.\n");
            break;
    }

    return EXIT_SUCCESS;
}

void parse_args(int argc, char **argv, char **filename) {
    if (argc > 2) {
        usage(argv);
    }
    printf("OK\n");
}

void error_exit (char *message, ...) {
    fprintf (stderr, "%s\n", message);
    exit (EXIT_FAILURE);
}

/**
 * useage information
 * @brief This function writes helpful usage information about the program to stderr and exites then.
 * @details global variables: argv
 */
void usage(char **argv) {
    fprintf (stderr, "USAGE: %s [file]\n", argv[0]);
    exit (EXIT_FAILURE);
}
