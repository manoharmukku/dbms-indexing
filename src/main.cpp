#include <iostream>
#include <stdlib.h>
#include <fstream>

#define ull unsigned long long
#define datatype int
#define N 4

using namespace std;

typedef struct treeNode {
	bool isLeaf;
	int keyCount;
	ull offset[N];
	datatype key[N-1];
	ull parent;
} Node;

typedef struct auxTreeNode {
	int keyCount;
	ull offset[N+1];
	datatype key[N];
} auxNode;

Node* findUtil(Node* root, datatype key) {
	// Set result as root node
	Node* result = root;

	// While result is not a leaf node
	while (result->isLeaf == FALSE) {
		// Find smallest number i such that result->key[i] <= key
		int i = 0;
		while (i < result->keyCount && result->key[i] < key)
			i++;
		if (i == result->keyCount)
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
	while (i < leafNode->keyCount && leafNode->key[i] < key)
		i++;
	if (i == leafNode->keyCount || leafNode->key[i] > key)
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
	}
	// Increment keyCount by 1
	leaf->keyCount++;
}

bool insertInParent(Node** root_ptr, Node* nodeLeft, datatype key, Node* nodeRight) {
	// If nodeLeft is the root of the tree
	if (nodeLeft == *root_ptr) {
		// Create a new node containing pointers nodeLeft, nodeRight and value key
		Node* newNode = (Node*)malloc(sizeof(Node));
		newNode->offset[0] = nodeLeft;
		newNode->offset[1] = nodeRight;
		newNode->key[0] = key;
		newNode->keyCount = 1;
		newNode->isLeaf = FALSE;
		// Make newNode the root of the tree
		*root_ptr = newNode;
	}
	else {
		Node* parent = nodeLeft->parent;
		// If parent has less than N pointers
		if (parent->keyCount < N-1) {
			// Insert key, nodeRight in parent just after nodeLeft
			// Find i for which parent->offset[i] = nodeLeft
			int i;
			for (i = 0; i <= parent->keyCount; i++)
				if (parent->offset[i] == nodeLeft) break;
			// Move right all keys and offsets after i by one position
			int j;
			for (j = parent->keyCount; j > i; j--) {
				parent->offset[j+1] = parent->offset[j];
				parent->key[j] = parent->key[j-1];
			}
			// Insert (key, nodeRight) after nodeLeft
			parent->key[i] = key;
			parent->offset[i+1] = nodeRight;
			parent->keyCount++;
		}
		else {
			// Else split parent and recurse
			// Create auxNode
			auxNode* temp = (auxNode*)malloc(sizeof(auxNode));
			// Copy parent to temp
			int i;
			for (i = 0; i < parent->keyCount; i++) {
				temp->offset[i] = parent->offset[i];
				temp->key[i] = parent->key[i];
			}
			temp->offset[i] = parent->key[i];
			temp->keyCount = parent->keyCount;
			// Insert (key, nodeRight) into temp just after nodeLeft
			// Find i for which temp->offset[i] = nodeLeft
			for (i = 0; i <= temp->keyCount; i++)
				if (temp->offset[i] == nodeLeft) break;
			// Move right all keys and offsets after i by one position
			int j;
			for (j = temp->keyCount; j > i; j--) {
				temp->offset[j+1] = temp->offset[j];
				temp->key[j] = temp->key[j-1];
			}
			// Insert (key, nodeRight) after nodeLeft
			temp->key[i] = key;
			temp->offset[i+1] = nodeRight;
			temp->keyCount++;
			// Create new node newParent
			Node* newParent = (Node*)malloc(sizeof(newParent));
			// Copy temp->offset[0] to temp->offset[ceil(n/2)-1] into parent
			for (i = 0; i < ceil(N/2)-1; i++) {
				parent->offset[i] = temp->offset[i];
				parent->key[i] = temp->key[i];
			}
			parent->offset[i] = temp->offset[i];
			parent->keyCount = ceil(N/2) - 1;
			// Let newKey = temp->key[ceil(n/2)-1]
			datatype newKey = temp->key[i];
			// Copy temp->offset[ceil(n/2)] to temp->offset[n] into newParent
			for (i = ceil(N/2); i < N; i++) {
				newParent->offset[i-ceil(N/2)] = temp->offset[i];
				newParent->key[i-ceil(N/2)] = temp->key[i];
			}
			newParent->offset[i-ceil(N/2)] = temp->offset[i];
			newParent->keyCount = N - ceil(N/2);
			newParent->isLeaf = FALSE;
			// Call insert_in_parent recursively for parent and newParent with newKey
			insertInParent(root_ptr, parent, newKey, newParent);
		}
	}
}

bool insert(Node** root_ptr, datatype key, ull recordOffset) {
	if (*root_ptr == NULL) {
		// If tree is empty, create an empty leaf node which is also the root
		Node* newNode = (Node*)malloc(sizeof(Node));
		newNode->isLeaf = TRUE;
		newNode->key[0] = key;
		newNode->offset[0] = recordOffset;
		// Set root to the new node
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
			temp->keyCount = N-1;
			// Now insert into temp the given key and offset
			insertInLeaf(temp, key, recordOffset);

			// Create new leaf node
			Node* newLeafNode = (Node*)malloc(sizeof(Node));
			// Make newLeafNode as next node to leafNode
			newLeafNode->offset[N-1] = leafNode->offset[N-1];
			leafNode->offset[N-1] = newLeafNode;

			// Copy temp->offset[0] to temp->key[ceil(N/2)-1] into leafNode starting at leafNode->offset[0]
			for (i = 0; i <= ceil(N/2)-1; i++) {
				leafNode->key[i] = temp->key[i];
				leafNode->offset[i] = leafNode->offset[i];
				leafNode->keyCount = ceil(N/2);
			}
			// Copy temp->offset[ceil(n/2)] to temp->key[n-1] into newLeafNode starting at newLeafNode->offset[0]
			for (i = ceil(N/2); i <= N-1; i++) {
				newLeafNode->offset[i-ceil(N/2)] = temp->offset[i];
				newLeafNode->key[i-ceil(N/2)] = temp->key[i];
			}
			newLeafNode->keyCount = N - ceil(N/2);
			newLeafNode->isLeaf = TRUE;
			// Free temp
			free(temp);
			// Insert in parent the smallest key value of newLeafNode
			insertInParent(root_ptr, leafNode, newLeafNode->key[0], newLeafNode);
		}
	}
}

int main() {

	return 0;
}