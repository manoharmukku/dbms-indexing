#include <iostream>
#include <stdlib.h>
#include <fstream>

#define ull unsigned long long
#define datatype int
#define N 4

using namespace std;

typedef struct treeNode {
	bool isLeaf;
	int keysCount;
	ull offset[N];
	datatype key[N-1];
}Node;

typedef struct auxTreeNode {
	ull offset[N+1];
	datatype key[N];
}auxNode;

Node* findUtil(Node* root, datatype key) {
	// Set result as root node
	Node* result = root;

	// While result is not a leaf node
	while (result->isLeaf == FALSE) {
		// Find smallest number i such that result->key[i] <= key
		int i = 0;
		while (i < result->keysCount && result->key[i] < key)
			i++;
		if (i == result->keysCount)
			result = result->offset[i];
		else if (key == result->key[i])
			result = result->offset[i+1];
		else
			result = result->offset[i];
	}

	// Return the leaf node 'result' which may contain the given key
	return result;
}

ull find(Node* root, datatype key) {
	Node* leafNode = findUtil(root, key);

	// Find the least i such that leafNode->key[i] = key
	int i = 0;
	while (i < leafNode->keysCount && leafNode->key[i] < key)
		i++;
	if (i == leafNode->keysCount || leafNode->key[i] > key)
		return 0; // Record not found
	else
		return leafNode->offset[i]; // Record number in the database file
}

bool insertInLeaf(Node* leaf, datatype key, ull recordOffset) {
	if (key < leaf->key[0]) {
		// Move all the keys and offsets one positions to the right
		int i;
		for (i = leaf->keyCount-1; i >= 0; i--) {
			leaf->key[i+1] = leaf->key[i];
			leaf->offset[i+1] = leaf->offset[i];
		}
		// Now insert given key and recordOffset at the first locations
		leaf->key[0] = key;
		leaf->offset[0] = recordOffset;
		// Increase keyCount by 1
		leaf->keyCount++;
	}
	else {
		// Find largest i such that leaf->key[i] < key and insert after that
		int i = 0;
		while (i < leaf->keyCount && leaf->key[i] < key)
			i++;
		if (i == leaf->keyCount) {
			leaf->key[i] = key;
			leaf->offset[i] = recordOffset;
		}
		else {
			int j;
			for (j = leaf->keyCount; j > i; j--) {
				leaf->key[j] = leaf->key[j-1];
				leaf->offset[j] = leaf->offset[j-1];
			}
			leaf->key[i] = key;
			leaf->offset[i] = recordOffset;
		}
		// Increment keyCount by 1
		leaf->keyCount++;
	}
}

bool insert(Node** root_ptr, datatype key, ull recordOffset) {
	// If tree is empty, create an empty leaf node which is also the root
	Node* newNode = (Node*)malloc(sizeof(Node));
	if (*root_ptr == NULL) {
		newNode->isLeaf = TRUE;
		newNode->key[0] = key;
		newNode->offset[0] = recordOffset;
		*root_ptr = newNode;
	}
	else {
		// Else find the leaf node that should contain the given key
		Node* leafNode = findUtil(*root_ptr, key);

		// If this leaf node contains n-1 key values then insert in this leaf node
		if (leafNode->keyCount < N-1) {
			insertInLeaf(leafNode, key, recordOffset);
		}
		else {
			// Else leafNode had n-1 key values already, split it
			// Copy leafNode into auxNode which can hold n (pointer, key) values
			auxNode* temp = (auxNode*)malloc(sizeof(auxNode));
			int i;
			for (i = 0; i < N-1; i++) {
				temp->key[i] = leafNode->key[i];
				temp->offset[i] = leafNode->offset[i];
			}
		}
	}
}

int main() {

	return 0;
}