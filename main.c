#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#define ASCII_ZERO 48
#define TEST_FILE "../test.txt"
#define INPUT_FILE "../input.txt"

char* read_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("fp is null");
        return nullptr;
    }
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* contents = malloc(sizeof(char) * length + 1);
    fread(contents, 1, length, fp);
    contents[length] = '\n';
    return contents;
};

typedef struct {
    char* key;
    int value;
} item;

typedef struct {
    int found;
    int val;
} num;

item items[] = {
{"zero", 0},
{"one", 1},
{"two", 2},
{"three", 3},
{"four", 4},
{"five", 5},
{"six", 6},
{"seven", 7},
{"eight", 8},
{"nine", 9},
};

int main() {
    int sum = 0;
    num first_num = {.found=0};
    num last_num = {.found=0};
    char* contents = read_file(INPUT_FILE);
    for (int idx = 0; contents[idx] != '\0'; idx++) {
        if (contents[idx] == '\n') {
            char str[3];
            str[0] = first_num.val;
            str[1] = last_num.val;
            str[2] = '\0';
            int num = atoi(str);
            sum += num;
            //printf("first: %d, last: %d, num: %d, sum: %d \n", first_num.val-ASCII_ZERO, last_num.val-ASCII_ZERO, num, sum);
            first_num.found = 0;
            first_num.val = 0;
            last_num.val = 0;
        }
        if (isdigit(contents[idx])) {
            if (first_num.found == 0) {
                first_num.found = 1;
                first_num.val = contents[idx];
            }
            last_num.val = contents[idx];
        }
    }
    printf("%d\n", sum);
    free(contents);
}