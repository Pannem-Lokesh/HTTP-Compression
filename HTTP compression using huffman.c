#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 100
#define MAX_CHAR 256

// Huffman Tree Node
struct MinHeapNode {
    char data;
    unsigned freq;
    struct MinHeapNode *left, *right;
};

// MinHeap (Priority Queue)
struct MinHeap {
    unsigned size;
    unsigned capacity;
    struct MinHeapNode** array;
};

// Function to create a new node
struct MinHeapNode* newNode(char data, unsigned freq) {
    struct MinHeapNode* temp = (struct MinHeapNode*)malloc(sizeof(struct MinHeapNode));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// Function to create a Min Heap
struct MinHeap* createMinHeap(unsigned capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct MinHeapNode*)malloc(capacity * sizeof(struct MinHeapNode));
    return minHeap;
}

// Swap function for heap
void swapMinHeapNode(struct MinHeapNode** a, struct MinHeapNode** b) {
    struct MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Heapify function
void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;
    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Extract the minimum node from heap
struct MinHeapNode* extractMin(struct MinHeap* minHeap) {
    struct MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Insert a new node into Min Heap
void insertMinHeap(struct MinHeap* minHeap, struct MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Build the Huffman Tree
struct MinHeap* buildMinHeap(char data[], int freq[], int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        minHeap->array[i] = newNode(data[i], freq[i]);
    minHeap->size = size;
    
    for (int i = (minHeap->size - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
    
    return minHeap;
}

// Build Huffman Tree and generate codes
struct MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) {
    struct MinHeapNode *left, *right, *top;
    struct MinHeap* minHeap = buildMinHeap(data, freq, size);

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// Store Huffman codes
void storeCodes(struct MinHeapNode* root, char codes[MAX_CHAR][MAX_TREE_HT], int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, codes, arr, top + 1);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, codes, arr, top + 1);
    }
    if (!root->left && !root->right) {
        codes[(unsigned char)root->data][top] = '\0';
        for (int i = 0; i < top; i++)
            codes[(unsigned char)root->data][i] = arr[i] + '0';
    }
}

// Compute frequency of characters in a string
void computeFrequencies(char *input, char *data, int *freq, int *size) {
    int count[MAX_CHAR] = {0};
    for (int i = 0; input[i] != '\0'; i++)
        count[(unsigned char)input[i]]++;
    
    *size = 0;
    for (int i = 0; i < MAX_CHAR; i++) {
        if (count[i] > 0) {
            data[*size] = (char)i;
            freq[*size] = count[i];
            (*size)++;
        }
    }
}

// Decode the binary string using Huffman tree
void decodeString(struct MinHeapNode* root, char *encoded) {
    struct MinHeapNode* curr = root;
    printf("\nDecoded String: ");
    for (int i = 0; encoded[i] != '\0'; i++) {
        curr = (encoded[i] == '0') ? curr->left : curr->right;
        if (!curr->left && !curr->right) {
            printf("%c", curr->data);
            curr = root;
        }
    }
    printf("\n");
}

// Encode the input string using Huffman codes
void encodeString(char *input, char *data, int *freq, int size) {
    struct MinHeapNode* root = buildHuffmanTree(data, freq, size);
    int arr[MAX_TREE_HT], top = 0;
    char codes[MAX_CHAR][MAX_TREE_HT] = {0};

    // Store Huffman Codes
    storeCodes(root, codes, arr, top);

    printf("\nHuffman Codes:\n");
    for (int i = 0; i < size; i++)
        printf("%c: %s\n", data[i], codes[(unsigned char)data[i]]);
    
    printf("\nEncoded Binary String: ");
    char encoded[1024] = "";
    for (int i = 0; input[i] != '\0'; i++)
        strcat(encoded, codes[(unsigned char)input[i]]);
    
    printf("%s\n", encoded);

    // Decode the encoded string
    decodeString(root, encoded);
}

// Free Huffman tree memory
void freeHuffmanTree(struct MinHeapNode* root) {
    if (root == NULL) return;
    freeHuffmanTree(root->left);
    freeHuffmanTree(root->right);
    free(root);
}

// Driver function
int main() {
    char input[1024];
    printf("Enter the string to encode: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    
    char data[MAX_CHAR];
    int freq[MAX_CHAR], size;
    
    computeFrequencies(input, data, freq, &size);
    
    encodeString(input, data, freq, size);

return 0;
}