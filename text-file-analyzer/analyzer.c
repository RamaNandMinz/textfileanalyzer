/**
 * @file analyzer.c
 * @brief Implementation of the core text analysis engine.
 *
 * This file contains the logic for opening, reading, and processing a text
 * file to gather character, word, and line counts, as well as frequency data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "analyzer.h"

// Define a maximum length for words to prevent buffer overflows.
#define MAX_WORD_LEN 100

int analyze_file(AppStats *stats) {
    FILE *file = fopen(stats->filename, "r");
    if (file == NULL) {
        // perror is used for system-level errors as it prints a more
        // informative message (e.g., "Error opening file: No such file or directory").
        perror("Error opening file");
        return -1; // Signal failure to the caller.
    }

    char word_buffer[MAX_WORD_LEN];
    int word_buffer_index = 0;
    int in_word = 0; // Flag for the basic whitespace-based word count.

    int c;
    while ((c = fgetc(file)) != EOF) {
        stats->char_count++;

        if (c == '\n') {
            stats->line_count++;
        }

        // Increment frequency for the character read. Cast to unsigned char
        // is good practice to handle all possible values safely as array indices.
        stats->char_freq[(unsigned char)c]++;

        // This is a simple word counter based on whitespace separation.
        if (isspace(c)) {
            in_word = 0;
        } else {
            if (in_word == 0) {
                stats->word_count++;
                in_word = 1;
            }
        }

        // This is the more sophisticated word-building logic for frequency analysis.
        // It considers only alphabetic characters to form words.
        if (isalpha(c)) {
            if (word_buffer_index < MAX_WORD_LEN - 1) {
                // Convert to lowercase for case-insensitive counting.
                word_buffer[word_buffer_index++] = tolower(c);
            }
        } else {
            // A non-alphabetic character signals the end of a word.
            if (word_buffer_index > 0) {
                word_buffer[word_buffer_index] = '\0'; // Null-terminate the string.
                insert_word(stats->word_counts, word_buffer);
                word_buffer_index = 0; // Reset buffer for the next word.
            }
        }
    }

    // After the loop, there might be a final word in the buffer if the file
    // did not end with a non-alphabetic character. This handles that edge case.
    if (word_buffer_index > 0) {
        word_buffer[word_buffer_index] = '\0';
        insert_word(stats->word_counts, word_buffer);
    }

    fclose(file);
    return 0; // Signal success.
}