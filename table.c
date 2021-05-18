/* table.c
 * Judson Cooper
 * MP7
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "table.h"


// prototypes for private functions in this file
int h (hashkey_t K, int M);
int p (hashkey_t K, int M, int type);
int TriangleNum (int action);
int PowerOfTwo (int num);


/* Construct a new hash table.
 *
 * table_size - size of the table
 * probe_type - the probe type to be used for searching the table
 * 
 * Returns a pointer to the header block of the table.
 */
table_t *table_construct(int table_size, int probe_type) {
	int i = 0;

	// allocate the header block
	table_t* newTable = (table_t*) malloc(sizeof(table_t));

	// initialize each element in the header
	newTable->size = table_size;
	newTable->probingType = probe_type;
	newTable->numKeys = 0;
	newTable->numRecentProbes = 0;
	newTable->oa = (table_entry_t*) malloc(table_size * sizeof(table_entry_t));

	// initialize all entries in table to empty
	for (i = 0; i < table_size; i++) {
		newTable->oa[i].key = EMPTY;
		newTable->oa[i].data_ptr = NULL;
	}

	return newTable;
}

/* Sequentially remove each table entry (K, I) and insert into a new
 * empty table with size new_table_size.  Free the memory for the old table
 * and return the pointer to the new table.  The probe type should remain
 * the same.
 *
 * T - the table to be rehashed
 * new_table_size - the size of the new table to be made
 *
 * returns a pointer to the header block of the rehashed table.
 */
table_t *table_rehash(table_t *T, int new_table_size) {
	table_t* newTable = table_construct(new_table_size, T->probingType);
	int insertReturn = 0;
	int i = 0;

	for (i = 0; i < T->size; i++) {
		if ((T->oa[i].key != EMPTY) && (T->oa[i].key != DELETED)) {
			// found valid entry, transfer it
			insertReturn = table_insert(newTable, T->oa[i].key, T->oa[i].data_ptr);
			assert(insertReturn != -1); // make sure entry inserted
			
			// now remove entry from old table
			T->oa[i].key = DELETED;
			T->oa[i].data_ptr = NULL;
		}
	}

	table_destruct(T);
	return newTable;
}

/* returns number of entries in the table 
 *
 * table - the table to determine the number of entries in.
*/
int table_entries(table_t* table) {
	return table->numKeys;
}

/* returns 1 if table is full and 0 if not full. 
 * 
 * table - the table to be checked for if it is full.
 */
int table_full(table_t* table) {
	if (table->numKeys < (table->size - 1)) {
		return 0; // not full
	} else {
		return 1; // full
	}
}

/* returns the number of table entries marked as deleted 
 *
 * table - the table to be searched for deleted keys.
 */
int table_deletekeys(table_t* table) {
	int numDeleted = 0;
	int i = 0;

	// increase numDeleted count for each marked entry in table
	for (i = 0; i < table->size; i++) {
		if (table->oa[i].key == DELETED) {
			numDeleted++;
		}
	}

	return numDeleted;
}
   
/* Insert a new table entry (K, I) into the table provided the table is not
 * already full.  
 *
 * table - the table to be inserted into
 * K - the key of the new entry
 * I - the data of the new entry
 *
 * Return:
 *      0 if (K, I) is inserted, 
 *      1 if an older (K, I) is already in the table (update with the new I), or 
 *     -1 if the (K, I) pair cannot be inserted.
 */
int table_insert(table_t *table, hashkey_t K, data_t I) {
	int firstHash = 0; // will hold the first hash value index
	int index = 0; // for each subsequent hash
	int foundDeleted = 0, delIndex = 0;

	// this array will keep track of which positions in the table have been
	// checked, so that the search loop will know when to stop. 
	// 0 for not checked, 1 for checked. (it's char to save memory space)
	char *checkedKeys = (char*) calloc(table->size, sizeof(char));
	int numChecked = 0; // count of how many positions checked

	table->numRecentProbes = 0; // reset this value

	firstHash = h(K, table->size);
	table->numRecentProbes++;

	index = firstHash;

	while (table->oa[index].key != EMPTY) {
		// insertion can go in empty or deleted spot if found
		if (checkedKeys[index] == 0) {
			// this position not yet checked, mark it
			checkedKeys[index] = 1;
			numChecked++;
		}
		if ((index == firstHash) && (table->probingType == QUAD)) {
			// make sure the table size is valid for quad if loop around
			if (!PowerOfTwo(table->size)) {
				// quad requires table size that is power of two, table failed
				// so exit (catastrophic error)
				printf("Poorly designed probe sequence and table size\n");
				table_destruct(table);
				free(I);
				free(checkedKeys);
				exit(1);
			}
		}

		if (table->oa[index].key == DELETED) {
			// found a deleted entry, store location for later but don't put
			// entry here yet because need to keep searching in case of duplicate
			if (!foundDeleted) {
				// only store the first deleted entry found
				foundDeleted = 1;
				delIndex = index;
			}
		}
		if (table->oa[index].key == K) {
			// found matching entry, replace
			free(table->oa[index].data_ptr);
			table->oa[index].data_ptr = I;
			free(checkedKeys);
			return 1;
		}
		else {
			// change index by necessary probe decrement to continue search
			index -= p(K, table->size, table->probingType);
			table->numRecentProbes++;
			while (index < 0) {
				// wrap around
				index += table->size;
			}
			if (((index == firstHash) && (table->probingType == DOUBLE)) &&
			(numChecked != table->size)) {
				if (table_full(table) == 0) {
					// table wasn't full but no spot found still
					// this is error in table design, abort
					printf("Poorly designed probe sequence and table size\n");
					table_destruct(table);
					free(I);
					free(checkedKeys);
					exit(1);
				}

			}
			else if (numChecked == table->size) {
				// no empty spot open (seeing first hash again isn't necessarily
				// issue in quad)
				if (!foundDeleted) {
					// no deleted spot either
					free(checkedKeys);
					return -1;
				}
				else {
					// go back to the deleted spot and store there
					if (table_full(table) == 0) {
						// make sure this isn't the last spot
						table->oa[delIndex].key = K;
						table->oa[delIndex].data_ptr = I;
						table->numKeys++;
						free(checkedKeys);
						return 0;
					}
					else {
						// the table is full, can't fill that deleted spot
						free(checkedKeys);
						return -1;
					}
				}
			}
		}
	}

	// found an empty spot to put entry in

	if (foundDeleted) {
		// but the previously found deleted spot takes precedence
		// (the previous loop had to continue to check for duplicate)
		table->oa[delIndex].key = K;
		table->oa[delIndex].data_ptr = I;
		table->numKeys++;
		// was no need to check if table is full, because there are two known
		// open spots, this deleted one and the empty one found in the loop
		free(checkedKeys);
		return 0;
	}

	if ((table->oa[index].key == EMPTY) && (table_full(table) == 1)) {
		// and don't fill last empty spot if table full
		free(checkedKeys);
		return -1;
	}
	
	// now empty spot is safe to fill
	table->oa[index].key = K;
	table->oa[index].data_ptr = I;
	table->numKeys++;
	free(checkedKeys);
	return 0;
}

/* Delete the table entry (K, I) from the table.  
 *
 * table - the table to be searched.
 * K - key of the entry to be deleted.
 *
 * Return:
 *     pointer to I, or
 *     null if (K, I) is not found in the table.  
 */
data_t table_delete(table_t *table, hashkey_t K) {
	int firstHash = 0; // will hold the first hash value index
	int index = 0; // for each subsequent hash
	data_t delData = NULL; // to hold the data to return
	
	char *checkedKeys = (char*) calloc(table->size, sizeof(char));
	int numChecked = 0; // count of how many positions checked

	table->numRecentProbes = 0; // reset this value

	firstHash = h(K, table->size);
	table->numRecentProbes++;
	
	index = firstHash;

	while (table->oa[index].key != EMPTY) {
		if (checkedKeys[index] == 0) {
			// this position not yet checked, mark it
			checkedKeys[index] = 1;
			numChecked++;
		}
		if ((index == firstHash) && (table->probingType == QUAD)) {
			// make sure the table size is valid for quad if loop around
			if (!PowerOfTwo(table->size)) {
				// quad requires table size that is power of two, table failed
				// so exit (catastrophic error)
				printf("Poorly designed probe sequence and table size\n");
				table_destruct(table);
				free(checkedKeys);
				exit(1);
			}
		}
		
		if (table->oa[index].key == K) {
			// key has been found, delete it
			delData = table->oa[index].data_ptr;
			table->oa[index].key = DELETED;
			table->oa[index].data_ptr = NULL;
			table->numKeys--;
			free(checkedKeys);
			return delData;
		}
		else {
			// change index by necessary probe decrement to continue search
			index -= p(K, table->size, table->probingType);
			table->numRecentProbes++;
			while (index < 0) {
				// wrap around
				index += table->size;
			}
			if (((index == firstHash) && (table->probingType == DOUBLE)) &&
			(numChecked != table->size)) {
				// searched whole table, no match found
				if (table_full(table) == 0) {
					// table wasn't full but no spot found still
					// this is error in table design, abort
					printf("Poorly designed probe sequence and table size\n");
					table_destruct(table);
					free(checkedKeys);
					exit(1);
				}
			}
			else if (numChecked == table->size) {
				free(checkedKeys);
				return NULL;
			}
		}
	}

	// found empty spot, stop search (nothing to delete)
	free(checkedKeys);
	return NULL;

}

/* Given a key, K, retrieve the pointer to the information, I, from the table,
 * but do not remove (K, I) from the table.  Return NULL if the key is not
 * found.
 *
 * table - the table to be searched
 * K - the key to be searched for
 *
 * Returns pointer to the data of the found entry. NULL if not found.
 */
data_t table_retrieve(table_t *table, hashkey_t K) {
	int firstHash = 0; // will hold the first hash value index
	int index = 0; // for each subsequent hash
	
	char *checkedKeys = (char*) calloc(table->size, sizeof(char));
	int numChecked = 0; // count of how many positions checked

	table->numRecentProbes = 0; // reset this value

	firstHash = h(K, table->size);
	table->numRecentProbes++;
	
	index = firstHash;

	while (table->oa[index].key != EMPTY) {
		if (checkedKeys[index] == 0) {
			// this position not yet checked, mark it
			checkedKeys[index] = 1;
			numChecked++;
		}
		if ((index == firstHash) && (table->probingType == QUAD)) {
			// make sure the table size is valid for quad if loop around
			if (!PowerOfTwo(table->size)) {
				// quad requires table size that is power of two, table failed
				// so exit (catastrophic error)
				printf("Poorly designed probe sequence and table size\n");
				table_destruct(table);
				free(checkedKeys);
				exit(1);
			}
		}
		
		if (table->oa[index].key == K) {
			// key has been found, return info ptr
			free(checkedKeys);
			return table->oa[index].data_ptr;
		}
		else {
			// change index by necessary probe decrement to continue search
			index -= p(K, table->size, table->probingType);
			table->numRecentProbes++;
			while (index < 0) {
				// wrap around
				index += table->size;
			}
			if (((index == firstHash) && (table->probingType == DOUBLE)) &&
			(numChecked != table->size)) {
				// looped around whole table without checking all entries
				if (table_full(table) == 0) {
					// table wasn't full but no spot found still
					// this is error in table design, abort
					printf("Poorly designed probe sequence and table size\n");
					table_destruct(table);
					free(checkedKeys);
					exit(1);
				}
			}
			else if (numChecked == table->size) {
				// checked all entries, no match
				free(checkedKeys);
				return NULL;

			}
		}
	}

	// found empty spot, stop search (key not found in table)
	free(checkedKeys);
	return NULL;
}

/* Free all information in the table, the table itself, and any additional
 * headers or other supporting data structures.  
 *
 * table - the table to be destroyed
 */
void table_destruct(table_t *table) {
	int i = 0;

	// free all data in the table
	for (i = 0; i < table->size; i++) {
		free(table->oa[i].data_ptr);
	}

	// free table array and the header
	free(table->oa);
	free(table);
}

/* The number of probes for the most recent call to table_retrieve,
 * table_insert, or table_delete 
 *
 * table - the table to be checked.
 *
 * Returns the number of recent probes
 */
int table_stats(table_t *table) {
	return table->numRecentProbes;
}

/* This function is for testing purposes only.  Given an index position into
 * the hash table return the value of the key if data is stored in this 
 * index position.
 *
 * T - the table to be searched.
 * index - the index of the table to be accessed.
 *
 * Returns the key value if the entry contains valid data, returns
 * 0 if the entry doesn't contain data.
 */
hashkey_t table_peek(table_t *T, int index) {
	assert((0 <= index) && (index < T->size));

	if ((T->oa[index].key == EMPTY) || (T->oa[index].key == DELETED)) {
		// the entry doesn't contain valid data
		return 0;
	}
	else {
		return T->oa[index].key;
	}
}

/* Print the table position and keys in a easily readable and compact format.
 * Only useful when the table is small.
 *
 * T - the table to be printed
 */
void table_debug_print(table_t *T) {
	int i = 0;

	// index through whole table array, print the key in each
	for (i = 0; i < T->size; i++) {
		if (T->oa[i].key == EMPTY) {
			printf("%d\tkey: EMPTY\n", i);
		}
		else if (T->oa[i].key == DELETED) {
			printf("%d\tkey: DEL\n", i);
		}
		else {
			printf("%d\tkey: %d\n", i, T->oa[i].key);
		}
	}
}



/*** Private Functions ***/

/* Executes the hash function for this table.
 *
 * K - the key value of the entry.
 * M - the size of the table.
 *
 * Returns the hash location for the given key.
 */
int h (hashkey_t K, int M) {
	TriangleNum(0); // reset this for new hash, in case method is quadratic
	return (K % M);
}

/* Executes the probe function for this table.
 *
 * K - the key value of the entry.
 * M - the size of the table.
 * type - the probe type to be used.
 *
 * Returns the probe decrement for the given key.
 */
int p (hashkey_t K, int M, int type) {
	if (type == LINEAR) {
		return 1;
	}
	else if (type == DOUBLE) {
		// p(K) = max(1, (K/M) % M)
		if (((K/M) % M) > 1) {
			return ((K/M) % M);
		}
		else {
			return 1;
		}
	}
	else if (type == QUAD) {
		return TriangleNum(1);
	}
	else {
		fprintf(stderr, "Invalid probe type\n");
		exit(1); // nothing can be done about this
	}
}

/* Stores the triangular number increment required for quadratic hashing, 
 * and allows it to be reset for a new hash insert/delete/search
 *
 * action - what action the function should take.
 * 	0 - reset the triangular number to zero
 * 	1 - return the next triangular number
 *
 * Returns the value of the triangular number
 */
int TriangleNum (int action) {
	static int counter = 0;

	if (action == 0) {
		counter = 0;
		return counter;
	}
	else if (action == 1) {
		counter++;
		return counter;
	}
	else {
		// invalid input
		return -1;
	}
}

/* Returns whether a number is a power of 2
 *
 * num - the number to be checked for if it is a power of 2.
 *
 * Returns 0 if not power of 2, 1 if power of 2.
 */
int PowerOfTwo (int num) {
	// uses boolean logic to determine if the number is a power of 2,
	// and excludes 0
	int isPower = ((num & (num-1)) == 0) && (num != 0);
	return isPower;
}












