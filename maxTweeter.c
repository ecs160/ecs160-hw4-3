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
void countTweeter(TweeterEntry *tweeter_counts, const char *name, int *num_tweeters) {
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

/* get position of name in the header */
int getNamePos(FILE *fp) {
    /* parse the header */
    char header[MAX_LINE_SIZE];
    int name_pos = 0;   // position of name

    if (fgets(header, MAX_LINE_SIZE, fp) != NULL) {
        const char* token = strtok(header, ",\n");

        while(token) {

            if (strcmp(token, "name") == 0 || strcmp(token, "\"name\"") == 0) {
                // we found the name position
                return name_pos;
            }

            name_pos++;
            token = strtok(NULL, ",\n");
        }
    } else {
        terminate();
    }

    return -1;
}

/* populate an array of tweeters along with their count */
void getTweeters(FILE *fp, TweeterEntry *tweeter_counts, int *num_tweeters, int name_pos) {
    // find tweeter counts
    char line[MAX_LINE_SIZE];
    while (fgets(line, MAX_LINE_SIZE, fp) != NULL) {
        // go to position of the name
        const char* token = strtok(line, ",\n");
        for (int i = 0; i < name_pos && token; i++)
            token = strtok(NULL, ",\n");

        printf("name %s\n", token);

        countTweeter(tweeter_counts, token, num_tweeters);
    }

    // TODO check the validity of each entry (no commas inside)
}

int main(int argc, char *argv[])
{
    /* open tweets file */
    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL) {
        terminate();
    }

    // TODO check the validity of each token in the header (dangling quotes)

    int name_pos = getNamePos(fp);

    if (name_pos != -1) {
        // found position of name in header
        TweeterEntry tweeter_counts[MAX_FILE_SIZE];
        int num_tweeters = 0;
        getTweeters(fp, tweeter_counts, &num_tweeters, name_pos);

        qsort(tweeter_counts, num_tweeters, sizeof(TweeterEntry), comparator);
        printTweeters(tweeter_counts, num_tweeters);
    } else {
        // never found position of name in header
        terminate();
    }

    // close the file
    fclose(fp);

    return 0;
}
