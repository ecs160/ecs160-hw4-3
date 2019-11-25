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
void countTweeter(TweeterEntry *tweeter_counts, char *name, int *num_tweeters) {
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

int main(int argc, char *argv[])
{
    /* open tweets file */
    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        terminate();

    /* parse the header */
    char *header;
    size_t bufsize = MAX_LINE_SIZE;
    size_t nchars = getline(&header, &bufsize, fp);

    if (nchars == -1)
        terminate();

    // TODO check the validity of each token in the header (dangling quotes)

    int name_pos = 0; // index of the 'name' column
    char *token = strtok(header, ","); // get the first token
    while (token != NULL) {
        if (strcmp(token, "name") == 0 || strcmp(token, "\"name\"") == 0)
                break;

        name_pos++;
        token = strtok(NULL, ",");
    }

    /* find tweeter counts */
    TweeterEntry tweeter_counts[MAX_FILE_SIZE];
    int num_tweeters = 0;
    char *line;

    // TODO check the validity of each entry (no commas inside)

    while ((nchars = getline(&line, &bufsize, fp)) != -1) {
        if (nchars == 0) // empty line
            continue;

        /* iterate through tokens until name column reached */
        token = strtok(line, ","); // get the first token
        for (int i = 1; i < name_pos; i++)
            token = strtok(NULL, ",");

        token[strlen(token) - 2] = '\0'; // strip new line
        countTweeter(tweeter_counts, token, &num_tweeters);
    }

    qsort(tweeter_counts, num_tweeters, sizeof(TweeterEntry), comparator);
    printTweeters(tweeter_counts, num_tweeters);

    /* free allocated data structures */
    free(header);
    free(line);
    fclose(fp);

    return 0;
}
