/**
 * @file hashtable.h
 * @brief Public interface for a simple chained hash table implementation.
 *
 * This file defines the data structures (Node, HashTable) and the public
 * function prototypes for creating, manipulating, and destroying a hash table
 * designed to store word frequencies.
 */

#ifndef HASHTABLE_H
#define HASHTABLE_H

/**
 * @struct Node
 * @brief A node in a linked list, representing a single word and its count.
 *
 * Each Node stores a dynamically allocated word, its frequency count, and a
 * pointer to the next node in the chain (for collision handling).
 */
typedef struct Node {
    char *word;          // The word stored in this node (dynamically allocated).
    int count;           // The frequency of the word.
    struct Node *next;   // Pointer to the next node in the same hash bucket.
} Node;

/**
 * @struct HashTable
 * @brief The main hash table structure.
 *
 * The HashTable consists of an array of pointers to Nodes (the "buckets")
 * and the size of this array.
 */
typedef struct HashTable {
    int size;      // The number of buckets in the table.
    Node **table;  // Pointer to the array of Node pointers.
} HashTable;

/**
 * @brief Creates a new, empty hash table.
 * @param size The number of buckets to allocate for the table.
 * @return A pointer to the newly created HashTable, or NULL on failure.
 */
HashTable *create_hash_table(int size);

/**
 * @brief Inserts a word into the hash table.
 * If the word already exists, its count is incremented. Otherwise, a new
 * node is created for the word with a count of 1.
 * @param ht A pointer to the HashTable.
 * @param word The word to insert.
 */
void insert_word(HashTable *ht, const char *word);

/**
 * @brief Frees all memory associated with a hash table.
 * This includes all nodes, all word strings within the nodes, the bucket
 * array, and the HashTable struct itself.
 * @param ht A pointer to the HashTable to be freed.
 */
void free_hash_table(HashTable *ht);

#endif // HASHTABLE_H