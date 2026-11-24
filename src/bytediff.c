/* Find contiguous byte differences without loading either file into memory. */
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

enum { BLOCK = 65536 };

static int compare(FILE *left, FILE *right) {
    unsigned char a[BLOCK], b[BLOCK];
    uintmax_t offset = 0, start = 0, changed = 0, ranges = 0;
    int in_range = 0;
    for (;;) {
        size_t na = fread(a, 1, sizeof a, left);
        size_t nb = fread(b, 1, sizeof b, right);
        size_t n = na > nb ? na : nb;
        if (ferror(left) || ferror(right)) {
            fputs("Read error; the comparison is incomplete.\n", stderr);
            return 2;
        }
        if (!n) break;
        if (UINTMAX_MAX - offset < n) {
            fputs("Input exceeds supported offset range.\n", stderr);
            return 2;
        }
        for (size_t i = 0; i < n; i++) {
            int different = i >= na || i >= nb || a[i] != b[i];
            if (different) {
                changed++;
                if (!in_range) {
                    start = offset + i;
                    ranges++;
                    in_range = 1;
                }
            } else if (in_range) {
                printf("%" PRIuMAX "\t%" PRIuMAX "\n", start, offset + i);
                in_range = 0;
            }
        }
        offset += n;
    }
    if (in_range) printf("%" PRIuMAX "\t%" PRIuMAX "\n", start, offset);
    if (fflush(stdout) == EOF || ferror(stdout)) {
        fputs("Could not write comparison output.\n", stderr);
        return 2;
    }
    fprintf(stderr, "%" PRIuMAX " differing bytes in %" PRIuMAX " ranges.\n", changed, ranges);
    return changed ? 1 : 0;
}

int main(int argc, char **argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        puts("Usage: bytediff LEFT RIGHT\nPrints tab-separated zero-based ranges: start, end (exclusive).");
        return 0;
    }
    if (argc != 3) {
        fputs("Usage: bytediff LEFT RIGHT\n", stderr);
        return 2;
    }
    FILE *left = fopen(argv[1], "rb");
    if (!left) { perror(argv[1]); return 2; }
    FILE *right = fopen(argv[2], "rb");
    if (!right) { perror(argv[2]); fclose(left); return 2; }
    int result = compare(left, right);
    fclose(right);
    fclose(left);
    return result;
}
