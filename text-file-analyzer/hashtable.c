/**
 * @file hashtable.c
 * @brief Implementation of the chained hash table for word frequency counting.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

// A prime number used as a multiplier in the hash function.
// Using primes helps in distributing keys more uniformly.
#define HASH_MULTIPLIER 31

/**
 * @brief The djb2 hash function.
 * A simple and effective string hashing algorithm.
 * @param word The string to hash.
 * @return An unsigned integer hash value.
 */
static unsigned int hash(const char *word) {
    unsigned long hash = 5381;
    int c;

    while ((c = *word++)) {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

HashTable *create_hash_table(int size) {
    if (size < 1) {
        return NULL;
    }

    // Allocate memory for the main HashTable structure.
    HashTable *ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return NULL;
    }

    // Allocate memory for the array of buckets (Node pointers).
    // Use calloc to initialize all bucket pointers to NULL automatically.
    ht->table = calloc(size, sizeof(Node *));
    if (ht->table == NULL) {
        free(ht); // Clean up partially allocated structure.
        return NULL;
    }

    ht->size = size;
    return ht;
}

void insert_word(HashTable *ht, const char *word) {
    if (ht == NULL || word == NULL) {
        return;
    }

    // Calculate the index for the word. Use modulo to fit within table size.
    unsigned int index = hash(word) % ht->size;

    // Traverse the linked list at the calculated index to check for duplicates.
    Node *current = ht->table[index];
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            // Word already exists, increment its count and we are done.
            current->count++;
            return;
        }
        current = current->next;
    }

    // If we reach here, the word is new. Create a new node.
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        // In a real-world app, might have more robust error handling.
        return;
    }

    // Duplicate the word string for storage in the node.
    // strdup allocates memory and copies the string.
    new_node->word = strdup(word);
    if (new_node->word == NULL) {
        free(new_node); // Clean up if strdup fails.
        return;
    }

    new_node->count = 1;
    new_node->next = ht->table[index]; // Point the new node to the old head.
    ht->table[index] = new_node;       // The new node is now the head.
}

void free_hash_table(HashTable *ht) {
    if (ht == NULL) {
        return;
    }

    // Iterate through every bucket in the hash table.
    for (int i = 0; i < ht->size; i++) {
        Node *current = ht->table[i];
        // Free every node in the linked list (chain) for the current bucket.
        while (current != NULL) {
            Node *temp = current;
            current = current->next; // Move to the next node before freeing the current one.
            free(temp->word);    // Free the dynamically allocated word string.
            free(temp);          // Free the node structure itself.
        }
    }

    // Finally, free the array of buckets and the main table structure.
    free(ht->table);
    free(ht);
}