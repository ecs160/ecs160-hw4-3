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
    if (strlen(token) == 0)
        return;

    char first_char = token[0];
    char last_char = token[strlen(token) - 1];

    if (first_char == '\"' && last_char != '\"')
        terminate();

    if (first_char != '\"' && last_char != '\"')
        terminate();
}

int countCommas(char *header) {
    int num_commas = 0;
    for (int i = 0; i < MAX_LINE_SIZE; i++) {
        if (header[i] == '\0')
            return num_commas;

        if (header[i] == ',')
            num_commas++;
    }
    return num_commas;
}

int main(int argc, char *argv[])
{
    /* open tweets file */
    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        terminate();

    /* parse the header */
    // char header[MAX_LINE_SIZE];
    char *header = (char *)malloc(MAX_LINE_SIZE * sizeof(char));
    if (fgets(header, MAX_LINE_SIZE, fp) == NULL)
        terminate();

    int num_fields = countCommas(header) + 1;
    int name_pos = 0; // index of the 'name' column
    char *token = strsep(&header, ",\n"); // get the first token
    for (int i = 1; i < num_fields; i++) {
        checkToken(token);

        if (strcmp(token, "name") == 0 || strcmp(token, "\"name\"") == 0)
            break;

        name_pos++;
        token = strsep(&header, ",\n");
    }

    printf("num fields: %d\n", num_fields);
    printf("name position: %d\n", name_pos);

    /* find tweeter counts */
    TweeterEntry tweeter_counts[MAX_FILE_SIZE];
    int num_tweeters = 0;
    // char line[MAX_LINE_SIZE];
    char *line = (char *)malloc(MAX_LINE_SIZE * sizeof(char));

    while (fgets(line, MAX_LINE_SIZE, fp) != NULL) {
        /* iterate through tokens until name column reached */
        const char *token = strsep(&line, ",\n"); // get the first token
        for (int i = 1; i <= name_pos; i++) {
            checkToken(token);
            token = strsep(&line, ",\n");
        }

        if (token == NULL)
            continue;

        countTweeter(tweeter_counts, token, &num_tweeters);
    }

    qsort(tweeter_counts, num_tweeters, sizeof(TweeterEntry), comparator);
    printTweeters(tweeter_counts, num_tweeters);

    /* close the file */
    fclose(fp);

    return 0;
}
