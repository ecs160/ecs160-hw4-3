#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define MAX_LINE_SIZE 1024
# define MAX_LINE_NUM 20000

static int hasQuotes = 0;
static int numFields = 0;
static int lineNum = 0;

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

    char c;
    char word[MAX_LINE_SIZE];
    memset(word, 0, MAX_LINE_SIZE);
    int i = 0;
    int line_length = 0;
    int name_pos = -1;   // position of name
    int found_name = 0; // track if we found name pos yet

    while ((c = fgetc(fp)) != EOF) {
        
        if (c != '\n') {
            line_length++;
        }

        if (line_length == MAX_LINE_SIZE) {
            // line is too big
            terminate();
        }

        if (c == ',' || c == '\n') {

            // increment numFields every time we see a comma
            numFields++;

            // increment name_pos if we didn't find name header yet
            if (found_name == 0) {
                name_pos++;
            }

            if (strncmp(word, "name", 4) == 0) {
                // we found the name position
                hasQuotes = 0;
                found_name = 1;
            } else if (strncmp(word, "\"name\"", 6) == 0) {
                // we found the name position
                hasQuotes = 1;
                found_name = 1;
            }

            // reset word variable and index
            memset(word, 0, MAX_LINE_SIZE);
            i = 0;

            // stop looking here
            if (c == '\n') {
                lineNum++;
                return name_pos;
            }

        } else {
            word[i] = c;
            i++;
        }
    }

    return name_pos;
}

/* given a string exit the program if dangling quotes are found */
int validToken(char *token) {
    if (token == NULL || strlen(token) == 0)
        return 0;

    char first_char = token[0];
    char last_char = token[strlen(token) - 1];

    if (first_char == '\"' && last_char != '\"')
        terminate();

    if (first_char != '\"' && last_char == '\"')
        terminate();

    if (first_char == '\"' && last_char == '\"')
        return 1;

    return 0;
}

/* populate an array of tweeters along with their count */
void getTweeters(FILE *fp, TweeterEntry *tweeter_counts, int *num_tweeters, int name_pos) {
    // find tweeter counts
    char word[MAX_LINE_SIZE];
    char c;
    int pos = -1;   // current field
    int i = 0;
    int line_length = 0;

    while ((c = fgetc(fp)) != EOF) {

        if (c != '\n') {
            line_length++;
        }

        if (line_length == MAX_LINE_SIZE) {
            // line is too big
            terminate();
        }
        if (c == ',' || c == '\n') {

            pos++;

            if (strlen(word) > 0 && pos == name_pos) {

                // remove whitespace
                char processed_word[MAX_LINE_SIZE];
                sscanf(word, "%s", processed_word);

                if (hasQuotes) {
                    if (validToken(processed_word)) {
                        // word has valid open and close quotes

                        // remove quotes
                        char word_without_quotes[MAX_LINE_SIZE];
                        int b = 0;
                        for (int a = 1; a < strlen(processed_word) - 1; a++, b++) {
                            word_without_quotes[b] = processed_word[a];
                        }

                        memcpy (processed_word, word_without_quotes, MAX_LINE_SIZE); 
                        memset(word_without_quotes, 0, MAX_LINE_SIZE);

                    } else {
                        // no matching open and closed quotes so invalid
                        terminate();
                    }
                }

                countTweeter(tweeter_counts, processed_word, num_tweeters);

                memset(processed_word, 0, MAX_LINE_SIZE);
            }

            // reset word variable and index
            memset(word, 0, MAX_LINE_SIZE);
            i = 0;

            if (c == '\n') {
                // check if we have too many lines
                if (lineNum == MAX_LINE_NUM) {
                    terminate();
                }

                lineNum++;

                // check if number of fields in the row matches the header count
                if ((pos + 1) == numFields) {
                    pos = -1;
                } else {
                    terminate();
                }
            }

        } else {
            word[i] = c;
            i++;
        }
    }
}

int main(int argc, char *argv[])
{
    /* open tweets file */
    char *fname = argv[1];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL) {
        terminate();
    }

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