#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define MAX_LINE_SIZE 1024
# define MAX_LINE_NUM 20000

static int hasQuotes[MAX_LINE_SIZE];
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
    // check out array if the name already exists
    for (int i = 0; i < *num_tweeters; i++) {
        // if the name exists in our array, just increment the count
        if (strcmp(tweeter_counts[i].name, name) == 0) {
            tweeter_counts[i].count++;
            return;
        }
    }

    /* if name not found set a new entry */
    tweeter_counts[*num_tweeters].name = (char *)malloc(strlen(name) * sizeof(char));
    strcpy(tweeter_counts[*num_tweeters].name, name);
    tweeter_counts[*num_tweeters].count = 1;
    (*num_tweeters)++;
}

/* print the counts of tweeters nicely */
void printTweeters(TweeterEntry *tweeter_counts, int num_tweeters) {
    int n = num_tweeters < 10 ? num_tweeters : 10; // number of names to print
    for (int i = 0; i < n; i++) {
        printf("%s: %d\n", tweeter_counts[i].name, tweeter_counts[i].count);
    }
}

/* compare the counts of tweeters in the map for sorting */
int comparator(const void *a, const void *b) {
    return ((TweeterEntry*)b)->count - ((TweeterEntry*)a)->count;
}

/* given a string exit the program if dangling quotes are found */
int validToken(char *token, int pos, int *first, int *last, int test_file_should_have_quote) {
    if (token == NULL || strlen(token) == 0) {
        return 1;
    }

    int first_quote_index = 0;
    int last_quote_index = strlen(token) - 1;

    for ( ; first_quote_index < strlen(token); first_quote_index++) {
        if (token[first_quote_index] == '\"') {
            break;
        }
    }

    for ( ; last_quote_index >= 0; last_quote_index--) {
        if (token[last_quote_index] == '\"') {
            break;
        }
    }

    // valid open and close quote found
    if (first_quote_index < last_quote_index) {

        if (test_file_should_have_quote) {
            if (hasQuotes[pos] == 0) {
                // it shouldn't have quote
                terminate();
            }
        } else {
            // header is calling this function
            // set initial value
            hasQuotes[pos] = 1;
        }

        if (first != NULL) {
            *first = first_quote_index;
        }

        if (last != NULL) {
            *last = last_quote_index;
        }

        return 1;
    }

    // no quotes at all
    if (first_quote_index == strlen(token) && last_quote_index == -1) {

        if (test_file_should_have_quote) {
            if (hasQuotes[pos] == 1) {
                // it should have quote
                terminate();
            }
        } else {
            // header is calling this function
            // set initial value
            hasQuotes[pos] = 0;
        }

        if (first != NULL) {
            *first = -1;
        }

        if (last != NULL) {
            *last = -1;
        }

        return 1;
    }

    // we found some mismatch if we reached here
    return 0;
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
        
        // increment line length for every non new line
        if (c != '\n') {
            line_length++;
        }

        // make sure line is within bound
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

            // remove starting and trailing whitespace
            char processed_word[MAX_LINE_SIZE];
            char *tok = strtok(word, " ");
            while (tok != NULL)
            {
                strcat(processed_word, tok);
                tok = strtok(NULL, " ");

                if (tok != NULL) {
                    strcat(processed_word, " ");
                }
            }

            if (validToken(processed_word, numFields - 1, NULL, NULL, 0)) {
                if (strncmp(processed_word, "\"name\"", 6) == 0 || strncmp(processed_word, "name", 4) == 0) {
                    // we found the name position
                    found_name = 1;
                }
            } else {
                // word has some mismatch quotes if any
                terminate();
            }

            // reset word variable and index
            memset(word, 0, MAX_LINE_SIZE);
            memset(processed_word, 0, MAX_LINE_SIZE);
            i = 0;

            // stop looking here
            if (c == '\n') {

                // didn't find a name header
                if (found_name == 0) {
                    return -1;
                }

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

/* populate an array of tweeters along with their count */
void getTweeters(FILE *fp, TweeterEntry *tweeter_counts, int *num_tweeters, int name_pos) {
    // find tweeter counts
    char word[MAX_LINE_SIZE];
    char c;
    int pos = -1;   // current field
    int i = 0;
    int line_length = 0;

    while ((c = fgetc(fp)) != EOF) {

        // increment line length for every non new line
        if (c != '\n') {
            line_length++;
        }

        // make sure line is within bound
        if (line_length == MAX_LINE_SIZE) {
            // line is too big
            terminate();
        }

        if (c == ',' || c == '\n') {

            pos++;

            char processed_word[MAX_LINE_SIZE];
            strcat(processed_word, word);

            // get the value to remove quotes later if any
            int first_quote_index = 0;
            int last_quote_index = 0;

            if (validToken(processed_word, pos, &first_quote_index, &last_quote_index, 1)) {
                // word has valid open and close quotes

                if (first_quote_index != -1 && last_quote_index != -1) {
                    
                    // remove closing quote first
                    memmove(&processed_word[last_quote_index], 
                        &processed_word[last_quote_index + 1], 
                        strlen(processed_word) - last_quote_index);

                    // remove opening quote last
                    memmove(&processed_word[first_quote_index], 
                        &processed_word[first_quote_index + 1], 
                        strlen(processed_word) - first_quote_index);


                    // null terminate the string depending on the position of name
                    if (pos == numFields - 1) {
                        processed_word[strlen(processed_word) - 1] = '\0';
                    }
                }

            } else {
                // word has some mismatch quotes if any
                terminate();
            }

            if (pos == name_pos) {
                countTweeter(tweeter_counts, processed_word, num_tweeters);
            }

            // reset word variable and index
            memset(processed_word, 0, MAX_LINE_SIZE);
            memset(word, 0, MAX_LINE_SIZE);
            i = 0;

            if (c == '\n') {
                // check if we have too many lines
                if (lineNum == MAX_LINE_NUM) {
                    terminate();
                }

                lineNum++;
                line_length = 0;

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
        TweeterEntry tweeter_counts[MAX_LINE_NUM];
        int num_tweeters = 0;

        // get all the names from the file
        getTweeters(fp, tweeter_counts, &num_tweeters, name_pos);

        // sort the names based on count
        qsort(tweeter_counts, num_tweeters, sizeof(TweeterEntry), comparator);

        // print the top 10 tweeters
        printTweeters(tweeter_counts, num_tweeters);

        for (int i = 0; i < num_tweeters; i++) {
            free(tweeter_counts[i].name);
        }
    } else {
        // never found position of name in header
        terminate();
    }

    // close the file
    fclose(fp);

    return 0;
}