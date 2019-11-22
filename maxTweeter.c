#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_SIZE 1024

static char* error_message = "Invalid Input Format";
static size_t MAX_LINE_NUM = 20000;

typedef struct {
    char* name;
    int count;
} NameEntry;

void addTweeter(NameEntry* tweeters, char* name, int* tweeter_size) {
    for (int i = 0; i < *tweeter_size; i++) {
        if (strcmp(tweeters[i].name, name) == 0) {
            tweeters[i].count++;
        }
    }

    tweeters[*tweeter_size].name = name;
    tweeters[*tweeter_size].count = 1;
    (*tweeter_size)++;
}

int comparator(const void* a, const void* b) {
    return (((NameEntry*)a)->count - ((NameEntry*)b)->count);
}

void printTweeters(NameEntry* tweeters) {
    for (int i = 0; i < 10; i++) {
        printf("%s: %d\n", tweeters[i].name, tweeters[i].count);
    }
}

int main(int argc, char *argv[])
{
    char* filename = argv[1];
    char* header;
    char* line;
    size_t num_read = 0;
    FILE* file_pointer = fopen(filename, "r");
    int pos_of_name = 0;
    char* word;
    NameEntry tweeters[MAX_LINE_NUM];
    int tweeter_size = 0;

    if (file_pointer == NULL) {
        printf("%s\n", error_message);
        return -1;
    }

    // get the header
    if ((num_read = getline(&header, &MAX_LINE_NUM, file_pointer)) != -1) {

        word = strtok (header, ",");
        while (word != NULL)
        {
            if (strcmp(word, "name") == 0 || strcmp(word, "\"name\"")) {
                break;
            }

            pos_of_name++;
            word = strtok (NULL, ",");
        }

    } else {
        printf("%s\n", error_message);
        return -1;
    }

    while((num_read = getline(&line, &MAX_LINE_NUM, file_pointer)) != -1) {
        // empty line
        if (num_read == 0) {
            continue;
        }

        for (int i = 0; i < pos_of_name; i++) {

            if (i == 0) {
                word = strtok (line, ",");
            } else {
                word = strtok (NULL, ",");
            }

            addTweeter(tweeters, word, &tweeter_size);
        }
    }

    qsort(tweeters, tweeter_size, sizeof(NameEntry), comparator);

    printTweeters(tweeters);

    return 0;
}
