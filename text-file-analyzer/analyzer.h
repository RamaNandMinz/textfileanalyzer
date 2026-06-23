/**
 * @file analyzer.h
 * @brief Public interface for the core text analysis engine.
 *
 * Defines the main data container (AppStats) and the primary function
 * for performing the file analysis.
 */

#ifndef ANALYZER_H
#define ANALYZER_H

#include "hashtable.h"

/**
 * @struct AppStats
 * @brief A container for all statistics collected by the analyzer.
 *
 * Bundling all related data into a single struct simplifies passing data
 * between functions and improves code organization.
 */
typedef struct {
    const char *filename;   // The name of the file being analyzed.
    long long char_count;   // Total characters (using long long for large files).
    int word_count;         // Total words (based on whitespace).
    int line_count;         // Total lines (based on newline characters).
    int *char_freq;         // Pointer to an array (size 256) for char frequencies.
    HashTable *word_counts; // Pointer to the hash table for word frequencies.
} AppStats;

/**
 * @brief Performs the core analysis of a text file.
 *
 * This function opens the specified file, reads it character by character,
 * and populates the provided AppStats struct with all collected statistics.
 *
 * @param stats A pointer to an AppStats struct. The `filename`, `char_freq`,
 *        and `word_counts` members must be pre-initialized. The function
 *        will fill in the other members.
 * @return 0 on success, -1 on failure (e.g., if the file cannot be opened).
 */
int analyze_file(AppStats *stats);

#endif // ANALYZER_H