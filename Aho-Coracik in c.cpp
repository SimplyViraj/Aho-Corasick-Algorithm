#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

#define MAXS 500 // Max number of states in the matching machine.
#define MAXC 26  // Maximum number of characters in input alphabet

// Node structure for Trie
typedef struct TrieNode {
    struct TrieNode *children[MAXC];
    int out;
    struct TrieNode *failure;
} TrieNode;

// Queue node structure for BFS
typedef struct QueueNode {
    TrieNode *data;
    struct QueueNode *next;
} QueueNode;

// Queue structure for BFS
typedef struct Queue {
    QueueNode *front, *rear;
} Queue;

// Function to initialize a new Trie node
TrieNode* newTrieNode() {
    TrieNode *node = (TrieNode *)malloc(sizeof(TrieNode));
    if (node) {
        node->out = 0;
        node->failure = NULL;
        for (int i = 0; i < MAXC; ++i)
            node->children[i] = NULL;
    }
    return node;
}

// Function to initialize a new queue
Queue* createQueue() {
    Queue *q = (Queue *)malloc(sizeof(Queue));
    if (q) {
        q->front = q->rear = NULL;
    }
    return q;
}

// Function to enqueue a Trie node into the queue
void enqueue(Queue *q, TrieNode *node) {
    QueueNode *qNode = (QueueNode *)malloc(sizeof(QueueNode));
    if (qNode) {
        qNode->data = node;
        qNode->next = NULL;
        if (!q->rear) {
            q->front = q->rear = qNode;
        } else {
            q->rear->next = qNode;
            q->rear = qNode;
        }
    }
}

// Function to dequeue a Trie node from the queue
TrieNode* dequeue(Queue *q) {
    if (!q->front) return NULL;
    QueueNode *temp = q->front;
    q->front = q->front->next;
    if (!q->front) q->rear = NULL;
    TrieNode *node = temp->data;
    free(temp);
    return node;
}

// Function to build the Trie for the given patterns
TrieNode* buildMatchingMachine(char **patterns, int k) {
    TrieNode *root = newTrieNode();
    Queue *q = createQueue();
    
    // Construct the Trie
    for (int i = 0; i < k; ++i) {
        TrieNode *curr = root;
        char *pattern = patterns[i];
        for (int j = 0; pattern[j]; ++j) {
            int index = pattern[j] - 'a';
            if (!curr->children[index])
                curr->children[index] = newTrieNode();
            curr = curr->children[index];
        }
        curr->out = (1 << i);
    }
    
    // Initialize failure function and enqueue root's children
    for (int i = 0; i < MAXC; ++i) {
        if (root->children[i]) {
            root->children[i]->failure = root;
            enqueue(q, root->children[i]);
        } else {
            root->children[i] = root;
        }
    }
    
    // Construct failure function using BFS
    while (q->front) {
        TrieNode *curr = dequeue(q);
        for (int i = 0; i < MAXC; ++i) {
            if (curr->children[i]) {
                TrieNode *failure = curr->failure;
                while (failure->children[i] == NULL)
                    failure = failure->failure;
                failure = failure->children[i];
                curr->children[i]->failure = failure;
                curr->children[i]->out |= failure->out;
                enqueue(q, curr->children[i]);
            }
        }
    }
    
    free(q);
    return root;
}

// Function to find next state in the Trie given the current state and input character
TrieNode* findNextState(TrieNode *currentState, char nextInput) {
    int ch = nextInput - 'a';
    while (currentState->children[ch] == NULL)
        currentState = currentState->failure;
    return currentState->children[ch];
}

// Function to search for patterns in the given text using Aho-Corasick algorithm
void searchWords(char **patterns, int k, char *text) {
    TrieNode *root = buildMatchingMachine(patterns, k);
    TrieNode *currentState = root;
    
    // Traverse the text and find matches
    for (int i = 0; text[i]; ++i) {
        currentState = findNextState(currentState, text[i]);
        if (currentState->out == 0) continue;
        
        // Match found, print all matching words
        for (int j = 0; j < k; ++j) {
            if (currentState->out & (1 << j)) {
                printf("Word %s appears from %d to %d\n", patterns[j], i - (int)strlen(patterns[j]) + 1, i);
            }
        }
    }
}

int main() {
    char *patterns[100]; // Assuming maximum 100 patterns
    char text[1000]; // Assuming maximum 1000 characters in text
    
    // Read patterns from file
    FILE *patternFile = fopen("patterns.txt", "r");
    if (!patternFile) {
        perror("Error opening patterns file");
        return EXIT_FAILURE;
    }
    int k = 0; // Number of patterns read
    char pattern[100];
    while (fscanf(patternFile, "%s", pattern) == 1) {
        patterns[k] = strdup(pattern);
        k++;
    }
    fclose(patternFile);
    
    // Read text from file
    FILE *textFile = fopen("text.txt", "r");
    if (!textFile) {
        perror("Error opening text file");
        return EXIT_FAILURE;
    }
    fgets(text, sizeof(text), textFile);
    fclose(textFile);
    
    searchWords(patterns, k, text);
    
    // Free allocated memory for patterns
    for (int i = 0; i < k; ++i) {
        free(patterns[i]);
    }

    return EXIT_SUCCESS;
}

