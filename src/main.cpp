#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <assert.h>

#define ull unsigned long long
#define datatype int
#define N 4
#define NODE_SIZE (N * 8 + (N-1) * sizeof(datatype))

using namespace std;

typedef struct {
	bool isLeaf;
	int keyCount;
	ull offset[N];
	datatype key[N-1];
} Node;

typedef struct {
	int keyCount;
	ull offset[N+1];
	datatype key[N];
} auxNode;

typedef struct {
	ull node;
	ull parent;
	bool end;
} Parent;

Parent* parentsList = (Parent*)calloc(25*N, sizeof(Parent));

Node* findUtil(Node* root, datatype key, FILE* file) {
	// Set result as root node
	Node* result = root;
	// Save parent information for root node in parentsList
	int i = 0;
	parentsList[i].node = result;
	parentsList[i].parent = 0;
	parentsList[i].end = FALSE;

	// While result is not a leaf node
	while (result->isLeaf == FALSE) {
		// Find smallest number i such that result->key[i] <= key
		int j = 0;
		while (j < result->keyCount && result->key[j] < key)
			j++;
		if (j == result->keyCount) {
			// Save parent information
			++i;
			parentsList[i].node = result->offset[j];
			parentsList[i].parent = result;
			parentsList[i].end = FALSE;

			result = result->offset[j];
		}
		else if (key == result->key[j]) {
			// Save parent information
			++i;
			parentsList[i].node = result->offset[j+1];
			parentsList[i].parent = result;
			parentsList[i].end = FALSE;

			result = result->offset[j+1];
		}
		else {
			// Save parent information
			++i;
			parentsList[i].node = result->offset[j];
			parentsList[i].parent = result;
			parentsList[i].end = FALSE;

			result = result->offset[j];
		}
	}

	// Set the last node in parentsList as end
	parentsList[i].end = TRUE;

	// Return the leaf node 'result' which may contain the given key
	return result;
}

ull find(Node* root, datatype key, FILE* file) {
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

ull getParent(ull node) {
	int i = 0;
	while (parentsList[i].end == FALSE) {
		if (parentsList[i].node == node)
			return parentsList[i].parent;
		++i;
	}
	if (parentsList[i].node == node)
		return parentsList[i].parent;
	return 0;
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
		Node* newNode = (Node*)calloc(1, sizeof(Node));
		newNode->offset[0] = nodeLeft;
		newNode->offset[1] = nodeRight;
		newNode->key[0] = key;
		newNode->keyCount = 1;
		newNode->isLeaf = FALSE;
		// Make newNode the root of the tree
		*root_ptr = newNode;
	}
	else {
		ull parent = getParent(nodeLeft);
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
			auxNode* temp = (auxNode*)calloc(1, sizeof(auxNode));
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
			Node* newParent = (Node*)calloc(1, sizeof(newParent));
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

bool insertUtil(Node** root_ptr, datatype key, ull recordOffset, FILE* indexFile) {
	if (*root_ptr == NULL) {
		// If tree is empty, create an empty leaf node which is also the root
		Node* newNode = (Node*)calloc(1, sizeof(Node));
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
			auxNode* temp = (auxNode*)calloc(1, sizeof(auxNode));
			int i;
			for (i = 0; i < N-1; i++) {
				temp->key[i] = leafNode->key[i];
				temp->offset[i] = leafNode->offset[i];
			}
			temp->keyCount = N-1;
			// Now insert into temp the given key and offset
			insertInLeaf(temp, key, recordOffset);

			// Create new leaf node
			Node* newLeafNode = (Node*)calloc(1, sizeof(Node));
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

void write_index_metadata_to_file(FILE* indexFile) {
	ull no_records = 0; // Initially no records
	int size_of_node = NODE_SIZE; // Size of each node in the tree
	ull root_node_number = 0; // Initially there is no root

	fwrite(&no_records, sizeof(ull), 1, indexFile);
	fwrite(&size_of_node, sizeof(int), 1, indexFile);
	fwrite(&root_node_number, sizeof(ull), 1, indexFile);
}

ull getRootNodeNumber(FILE* indexFile) {
	ull root_node_number;
	fseek(indexFile, sizeof(ull) + sizeof(int), SEEK_SET);
	fread(&root_node_number, sizeof(ull), 1, indexFile);
	return root_node_number;
}

long getPositionToSeekInFile(ull record_number) {
	// Sizeof(root_node_number) + sizeof(no_records) + sizeof(size_of_node_variable) +  
	long position = 2 * sizeof(ull) + sizeof(int) + (record_number - 1) * NODE_SIZE;
	return position;
}

void load_record_from_file_into_variable(Node** node, size_t size, FILE* indexFile, ull record_number) {
	long position = getPositionToSeekInFile(record_number);
	// Seek to the positon in file from beginning
	fseek(indexFile, position, SEEK_SET);
	// Read record at the position into the variable
	fread(*node, size, 1, indexFile);
}

bool indexInsert(datatype key, ull recordOffset, char* fileName) {
	FILE *indexFile = fopen(fileName, "wb+x");

	// If file already exists, open in read/update more
	if (indexFile == NULL) {
		indexFile = fopen(fileName, "rb+");
		assert(indexFile != NULL);
	}
	else {
		// If file does not already exist, write metadata to newly created file
		write_index_metadata_to_file(indexFile);
	}

	// Get the root node number from the file
	ull root_node_number = getRootNodeNumber(indexFile);

	// Create memory for root node and extract that node from the file
	Node* root;
	if (root_node_number == 0) {
		root = NULL;
	}
	else {
		root = (Node*)calloc(1, sizeof(Node));
		load_record_from_file_into_variable(&root, sizeof(Node), indexFile, root_node_number);
	}

	insertUtil(root, key, recordOffset);
}

int main() {

	return 0;
}