/**
 * @file main.c
 * @brief The main entry point and control center for the Text File Analyzer.
 *
 * This file is responsible for:
 * - Parsing command-line arguments and options.
 * - Setting up the necessary data structures.
 * - Delegating the core analysis to the analyzer module.
 * - Orchestrating the report generation.
 * - Ensuring all resources are properly freed before exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "analyzer.h"

// The default size of the hash table.
#define HASH_TABLE_SIZE 4096

/**
 * @struct AnalysisOptions
 * @brief A container for command-line options to control program behavior.
 */
typedef struct {
    bool show_overall_stats;
    bool show_char_freq;
    bool show_word_freq;
    char *output_filename;
} AnalysisOptions;

// --- Function Prototypes ---
void print_report(const AppStats *stats, const AnalysisOptions *options, FILE *output_stream);
void print_char_frequency(int counts[], FILE *output_stream);
static void print_usage(const char *prog_name);

int main(int argc, char *argv[]) {
    // --- 1. Argument Parsing ---
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    AnalysisOptions options = { false, false, false, NULL };
    bool any_option_set = false;
    char *input_filename = NULL;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];

        if (strcmp(arg, "-c") == 0 || strcmp(arg, "-w") == 0 || strcmp(arg, "-l") == 0) {
            options.show_overall_stats = true;
            any_option_set = true;
        } else if (strcmp(arg, "--freq") == 0) {
            options.show_char_freq = true;
            options.show_word_freq = true;
            any_option_set = true;
        } else if (strcmp(arg, "-o") == 0) {
            if (i + 1 < argc) {
                options.output_filename = argv[i + 1];
                i++; // IMPORTANT: Consume the filename argument to prevent it from being processed again.
            } else {
                fprintf(stderr, "Error: -o option requires a filename argument.\n");
                return EXIT_FAILURE;
            }
        } else if (arg[0] == '-') {
            fprintf(stderr, "Error: Unknown option '%s'\n", arg);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (input_filename != NULL) {
                fprintf(stderr, "Error: Multiple input filenames provided. Please specify only one.\n");
                return EXIT_FAILURE;
            }
            input_filename = arg;
        }
    }

    if (input_filename == NULL) {
        fprintf(stderr, "Error: No input filename specified.\n");
        return EXIT_FAILURE;
    }

    // If the user did not specify any display options, default to showing everything.
    if (!any_option_set) {
        options.show_overall_stats = true;
        options.show_char_freq = true;
        options.show_word_freq = true;
    }

    // --- 2. Setup Data Structures ---
    HashTable *word_counts = create_hash_table(HASH_TABLE_SIZE);
    if (word_counts == NULL) {
        fprintf(stderr, "Fatal: Could not create hash table.\n");
        return EXIT_FAILURE;
    }

    int main_char_freq[256] = {0}; // Stack-allocated, no free needed.

    AppStats stats = {0};
    stats.filename = input_filename;
    stats.char_freq = main_char_freq;
    stats.word_counts = word_counts;

    // --- 3. Delegate to Analysis Engine ---
    if (analyze_file(&stats) != 0) {
        fprintf(stderr, "Analysis failed for file: %s\n", input_filename);
        free_hash_table(stats.word_counts); // CRITICAL: Cleanup on error path.
        return EXIT_FAILURE;
    }

    // --- 4. Prepare Output Stream and Generate Report ---
    FILE *output_stream = stdout;
    if (options.output_filename != NULL) {
        output_stream = fopen(options.output_filename, "w");
        if (output_stream == NULL) {
            perror("Error opening output file");
            free_hash_table(stats.word_counts); // CRITICAL: Cleanup on error path.
            return EXIT_FAILURE;
        }
    }

    print_report(&stats, &options, output_stream);

    // --- 5. Final Cleanup ---
    if (output_stream != stdout) {
        fclose(output_stream);
    }

    free_hash_table(stats.word_counts); // The primary cleanup for the success path.

    return EXIT_SUCCESS;
}

/**
 * @brief Prints the final, formatted analysis report to the given stream.
 * The function is controlled by the options struct to display only the
 * requested sections of the report.
 * @param stats A pointer to the populated AppStats struct containing all data.
 * @param options A pointer to the AnalysisOptions struct with user choices.
 * @param output_stream The stream (stdout or a file) to write the report to.
 */
void print_report(const AppStats *stats, const AnalysisOptions *options, FILE *output_stream) {
    fprintf(output_stream, "--- Analysis Report for %s ---\n\n", stats->filename);

    if (options->show_overall_stats) {
        fprintf(output_stream, "Overall Statistics:\n");
        fprintf(output_stream, "Total Characters:\t%lld\n", stats->char_count);
        fprintf(output_stream, "Total Words:\t\t%d\n", stats->word_count);
        fprintf(output_stream, "Total Lines:\t\t%d\n\n", stats->line_count);
    }

    if (options->show_char_freq) {
        fprintf(output_stream, "Character Frequency:\n");
        print_char_frequency(stats->char_freq, output_stream);
        fprintf(output_stream, "\n");
    }

    if (options->show_word_freq) {
        fprintf(output_stream, "Word Frequency:\n");
        fprintf(output_stream, "  %-20s %s\n", "Word", "Count");
        fprintf(output_stream, "  %-20s %s\n", "--------------------", "-----");

        for (int i = 0; i < stats->word_counts->size; i++) {
            Node *current = stats->word_counts->table[i];
            while (current != NULL) {
                fprintf(output_stream, "  %-20s %d\n", current->word, current->count);
                current = current->next;
            }
        }
    }
}

/**
 * @brief Prints the character frequency table to the given stream.
 * @param counts The array of character frequency counts.
 * @param output_stream The stream to write to.
 */
void print_char_frequency(int counts[], FILE *output_stream) {
    fprintf(output_stream, "  %-10s %-10s\n", "Character", "Count");
    fprintf(output_stream, "  %-10s %-10s\n", "---------", "-----");
    for (int i = 0; i < 256; i++) {
        if (counts[i] > 0 && isprint(i)) {
            fprintf(output_stream, "  %-10c %-10d\n", i, counts[i]);
        }
    }
}

/**
 * @brief Prints the usage message for the program.
 * @param prog_name The name of the program executable (from argv[0]).
 */
static void print_usage(const char *prog_name) {
    fprintf(stderr, "Usage: %s [options] <filename>\n", prog_name);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -c, -w, -l    Show overall statistics (characters, words, lines).\n");
    fprintf(stderr, "  --freq          Show character and word frequency tables.\n");
    fprintf(stderr, "  -o <file>       Write the report to <file> instead of the console.\n");
    fprintf(stderr, "If no options are specified, the full report is shown.\n");
}