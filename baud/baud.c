/**
 * Baud
 *
 * Slows down text output to various baud rate speeds.
 */

#include <stdio.h>                      // printf, getchar, putchar
#include <stdlib.h>                     // atoi, EXIT_SUCCESS, EXIT_FAILURE
#include <time.h>                       // clock_gettime
#include <unistd.h>                     // usleep

void usage(char app[]) {
    printf("Usage: %s BAUD [FILE]\n", app);
    printf("Where BAUD is any number up to 112000, but often one of the standard bit rates:\n");
    printf("  50, 110, 300, 600, 1200, 2400, 4800, 9600, 14400, 28800, 33600, 56000, 112000\n");
    printf("If FILE is given, then it is used as the source. Otherwise, STDIN is used.\n");
}

int main(int argc, char *argv[]) {
    int baud, rc;
    long delay, d;
    struct timespec start_clock, end_clock;
    char ch;
    FILE *file = NULL;

    if (argc < 2 || argc > 3) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    baud = atoi(argv[1]);

    if (baud < 1 || baud > 112000) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        file = fopen(argv[2], "r");
        if (file == NULL) {
            printf("Could not open file for reading: %s\n", argv[2]);
            return EXIT_FAILURE;
        }
    }

    delay = 8 * 1000000000L / baud;

    if (clock_gettime(CLOCK_REALTIME, &start_clock) != 0) return EXIT_FAILURE;

    do {
        if (file == NULL) {
            ch = getchar();
            if (ch == EOF) return EXIT_SUCCESS;
        } else {
            ch = fgetc(file);
            if (feof(file)) {
                fclose(file);
                return EXIT_SUCCESS;
            }
        }
        if (clock_gettime(CLOCK_REALTIME, &end_clock) != 0) return EXIT_FAILURE;
        d = start_clock.tv_nsec + delay - end_clock.tv_nsec;
        if (d > 0) rc = usleep(d / 1000);
        if (clock_gettime(CLOCK_REALTIME, &start_clock) != 0) return EXIT_FAILURE;
        putchar(ch);
        fflush(stdout);
    } while (rc == 0);

    if (file != NULL) fclose(file);

    return EXIT_SUCCESS;
}
