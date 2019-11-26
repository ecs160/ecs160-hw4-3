#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define MAX_LINE_SIZE 1024
# define MAX_FILE_SIZE 20000

typedef struct TweeterEntry {
    char *name;
    int count;
} TweeterEntry;

/* print error message and exit(1) */
void terminate() {
    printf("%s\n", "Invalid Input Format");
    exit(1);
}

/* update the count of tweeter in the map */
void countTweeter(TweeterEntry *tweeter_counts, const char *name,
                  int *num_tweeters) {
    for (int i = 0; i < *num_tweeters; i++) {
        if (strcmp(tweeter_counts[i].name, name) == 0) {
            tweeter_counts[i].count++;
            return;
        }
    }

    /* if name not found set a new entry */
    tweeter_counts[*num_tweeters].name =
        (char *)malloc(strlen(name) * sizeof(char));
    strcpy(tweeter_counts[*num_tweeters].name, name);
    tweeter_counts[*num_tweeters].count = 1;
    (*num_tweeters)++;
}

/* print the counts of tweeters nicely */
void printTweeters(TweeterEntry *tweeter_counts, int num_tweeters) {
    int n = num_tweeters < 10 ? num_tweeters : 10; // number of names to print
    for (int i = 0; i < n; i++)
        printf("%s: %d\n", tweeter_counts[i].name, tweeter_counts[i].count);
}

/* compare the counts of tweeters in the map for sorting */
int comparator(const void *a, const void *b) {
    return ((TweeterEntry*)a)->count - ((TweeterEntry*)b)->count;
}

/* given a string exit the program if dangling quotes are found */
void checkToken(const char *token) {
    char first_char = token[0];
    char last_char = token[strlen(token) - 1];

    if (first_char == '\"' && last_char != '\"')
        terminate();

    if (first_char != '\"' && last_char != '\"')
        terminate();
}

int main(int argc, char *argv[])
{
    /* open tweets file */
    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        terminate();

    /* parse the header */
    char header[MAX_LINE_SIZE];
    if (fgets(header, MAX_LINE_SIZE, fp) == NULL)
        terminate();

    int name_pos = 0; // index of the 'name' column
    char *token = strtok(header, ",\n"); // get the first token
    while (token != NULL) {
        checkToken(token);

        if (strcmp(token, "name") == 0 || strcmp(token, "\"name\"") == 0)
            break;

        name_pos++;
        token = strtok(NULL, ",\n");
    }

    /* find tweeter counts */
    TweeterEntry tweeter_counts[MAX_FILE_SIZE];
    int num_tweeters = 0;
    char line[MAX_LINE_SIZE];

    while (fgets(line, MAX_LINE_SIZE, fp) != NULL) {
        /* iterate through tokens until name column reached */
        const char *token = strtok(line, ",\n"); // get the first token
        for (int i = 1; i <= name_pos; i++) {
            checkToken(token);
            token = strtok(NULL, ",\n");
        }

        countTweeter(tweeter_counts, token, &num_tweeters);
    }

    qsort(tweeter_counts, num_tweeters, sizeof(TweeterEntry), comparator);
    printTweeters(tweeter_counts, num_tweeters);

    /* close the file */
    fclose(fp);

    return 0;
}
