/* * * * * * * * *
* Dynamic hash table using a combination of extendible hashing and cuckoo
* hashing with a single keys per bucket, resolving collisions by switching keys
* between two tables with two separate hash functions and growing the tables
* incrementally in response to cycles
*
* created for COMP20007 Design of Algorithms - Assignment 2, 2017
* by ...
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "xuckoo.h"

/******************************************************************************/
#define TABLE1 1
#define TABLE2 2

// macro to calculate the rightmost n bits of a number x
#define rightmostnbits(n, x) (x) & ((1 << (n)) - 1)

// a bucket stores a single key (full=true) or is empty (full=false)
// it also knows how many bits are shared between possible keys, and the first
// table address that references it
typedef struct bucket {
	int id;		// a unique id for this bucket, equal to the first address
				// in the table which points to it
	int depth;	// how many hash value bits are being used by this bucket
	bool full;	// does this bucket contain a key
	int64 key;	// the key stored in this bucket
} Bucket;

// an inner table is an extendible hash table with an array of slots pointing
// to buckets holding up to 1 key, along with some information about the number
// of hash value bits to use for addressing
typedef struct inner_table {
	Bucket **buckets;	// array of pointers to buckets
	int size;			// how many entries in the table of pointers (2^depth)
	int depth;			// how many bits of the hash value to use (log2(size))
	int nkeys;			// how many keys are being stored in the table
} InnerTable;

// a xuckoo hash table is just two inner tables for storing inserted keys
struct xuckoo_table {
	InnerTable *table1;
	InnerTable *table2;
	int time;		// how much CPU time has been used to insert/lookup keys
					// in this table
};

/******************************************************************************/

// create a new bucket first referenced from 'first_address', based on 'depth'
// bits of its keys' hash values
static Bucket *new_bucket(int first_address, int depth) {
	Bucket *bucket = malloc(sizeof *bucket);
	assert(bucket);

	bucket->id = first_address;
	bucket->depth = depth;
	bucket->full = false;

	return bucket;
}

/******************************************************************************/
// initialise an extendible cuckoo hash table
XuckooHashTable *new_xuckoo_hash_table() {
	XuckooHashTable *table = malloc(sizeof (struct xuckoo_table));
	assert(table);
	/*allocating memory for the 2 inner tables*/
	table->table1=malloc(sizeof(InnerTable));
	table->table2=malloc(sizeof(InnerTable));
	assert(table->table1);
	assert(table->table2);
	/*create a bucket array for each table*/
	table->table1->buckets=malloc(sizeof *table->table1->buckets);
	table->table2->buckets=malloc(sizeof *table->table2->buckets);
	assert(table->table1->buckets);
	assert(table->table2->buckets);
	table->table1->buckets[0] = new_bucket(0, 0);
	table->table2->buckets[0] = new_bucket(0, 0);

	table->time = 0;

	/*initialise variables*/
	table->table1->size = 1;
	table->table2->size = 1;

	table->table1->depth = 0;
	table->table2->depth = 0;

	table->table1->nkeys = 0;
	table->table2->nkeys = 0;


	return table;
}

/******************************************************************************/
// free all memory associated with 'table'
void free_xuckoo_hash_table(XuckooHashTable *table) {
	assert(table);

	// loop backwards through the array of pointers, freeing buckets only as we
	// reach their first reference
	// (if we loop through forwards, we wouldn't know which reference was last)
	int i;
	/*free each bucket from both the tables*/
	for (i = table->table1->size-1; i >= 0; i--) {
		if (table->table1->buckets[i]->id == i) {
			free(table->table1->buckets[i]);
		}
	}
	for (i = table->table2->size-1; i >= 0; i--) {
		if (table->table2->buckets[i]->id == i) {
			free(table->table2->buckets[i]);
		}
	}
	/*free the array of buckets*/
	free(table->table1->buckets);
	free(table->table2->buckets);

	/*free both the tables*/
	free(table->table1);
	free(table->table2);

	/*free the main table*/
	free(table);


}

/******************************************************************************/
// reinsert a key into the hash table after splitting a bucket --- we can assume
// that there will definitely be space for this key because it was already
// inside the hash table previously
// use 'xuckoo_hash_table_insert()' instead for inserting new keys
static void double_table(InnerTable *table) {
	int size = table->size * 2;
	assert(size < MAX_TABLE_SIZE && "error: table has grown too large!");

	// get a new array of twice as many bucket pointers, and copy pointers down
	table->buckets = realloc(table->buckets, (sizeof *table->buckets) * size);
	assert(table->buckets);
	int i;
	for (i = 0; i < table->size; i++) {
		table->buckets[table->size + i] = table->buckets[i];
	}

	// finally, increase the table size and the depth we are using to hash keys
	table->size = size;
	table->depth++;
}

/******************************************************************************/

// reinsert a key into the hash table after splitting a bucket --- we can assume
// that there will definitely be space for this key because it was already
// inside the hash table previously
// use 'xuckoo_hash_table_insert()' instead for inserting new keys
static void reinsert_key(InnerTable *table, int64 key, int table_no) {
	int hash;
	if (table_no==1){
		hash=h1(key);
	}else{
		hash=h2(key);
	}
	int address = rightmostnbits(table->depth, hash);
	table->buckets[address]->key = key;
	table->buckets[address]->full = true;
}

/******************************************************************************/

// split the bucket in 'table' at address 'address', growing table if necessary
static void split_bucket(InnerTable *table, int address, int table_no) {

	// FIRST,
	// do we need to grow the table?
	if (table->buckets[address]->depth == table->depth) {
		// yep, this bucket is down to its last pointer
		double_table(table);
	}
	// either way, now it's time to split this bucket


	// SECOND,
	// create a new bucket and update both buckets' depth
	Bucket *bucket = table->buckets[address];
	int depth = bucket->depth;
	int first_address = bucket->id;

	int new_depth = depth + 1;
	bucket->depth = new_depth;

	// new bucket's first address will be a 1 bit plus the old first address
	int new_first_address = 1 << depth | first_address;
	Bucket *newbucket = new_bucket(new_first_address, new_depth);

	// THIRD,
	// redirect every second address pointing to this bucket to the new bucket
	// construct addresses by joining a bit 'prefix' and a bit 'suffix'
	// (defined below)

	// suffix: a 1 bit followed by the previous bucket bit address
	int bit_address = rightmostnbits(depth, first_address);
	int suffix = (1 << depth) | bit_address;

	// prefix: all bitstrings of length equal to the difference between the new
	// bucket depth and the table depth
	// use a for loop to enumerate all possible prefixes less than maxprefix:
	int maxprefix = 1 << (table->depth - new_depth);

	int prefix;
	for (prefix = 0; prefix < maxprefix; prefix++) {

		// construct address by joining this prefix and the suffix
		int a = (prefix << new_depth) | suffix;

		// redirect this table entry to point at the new bucket
		table->buckets[a] = newbucket;
	}

	// FINALLY,
	// filter the key from the old bucket into its rightful place in the new
	// table (which may be the old bucket, or may be the new bucket)

	// remove and reinsert the key
	int64 key = bucket->key;
	bucket->full = false;
	reinsert_key(table, key, table_no);
}

/******************************************************************************/

// insert 'key' into 'table', if it's not in there already
// returns true if insertion succeeds, false if it was already in there
bool xuckoo_hash_table_insert(XuckooHashTable *table, int64 key) {
	assert(table);
	int start_time = clock(); // start timing

	InnerTable *table1=table->table1;
	InnerTable *table2=table->table2;
	/*variable to count the number of swaps*/
	int loop=0;
	/*variable to toogle between the tables*/
	int flag=1;

	int hash;
	int address;
	int64 temp_key;
	if (table1->nkeys>table2->nkeys){
		flag=2;
	}
	/*checking if the key is already in the table*/
	if (xuckoo_hash_table_lookup(table, key)){
		table->time += clock() - start_time; // add time elapsed
		return(false);
	}
	while(flag<3){
		loop++;
		if (flag==1){
			flag=2;
			hash=h1(key);
			address = rightmostnbits(table1->depth, hash);
			if (table1->buckets[address]->full){
				/*checking if its time to split the bucket*/
				if (loop>((table2->nkeys)+(table1->nkeys)+10)){
					loop=0;
					split_bucket(table1, address, TABLE1);
					flag=1;
				}else{
					/*swapping keys*/
					temp_key=table1->buckets[address]->key;
					table1->buckets[address]->key = key;
					key=temp_key;
					flag=2;

				}
			}else{
				/*entering the key into the bucket*/
				table1->buckets[address]->key = key;
				table1->buckets[address]->full = true;
				table1->nkeys=table1->nkeys+1;
				table->time += clock() - start_time; // add time elapsed
				return(true);
			}
		}else{
			flag=1;
			hash=h2(key);
			address = rightmostnbits(table2->depth, hash);
			if (table2->buckets[address]->full){
				/*checking if its time to split the bucket*/
				if (loop>((table2->nkeys)+(table1->nkeys)+10)){
					loop=0;
					split_bucket(table2, address,TABLE2);
					flag=2;
				}else{
					/*swapping keys*/
					temp_key=table2->buckets[address]->key;
					table2->buckets[address]->key = key;
					key=temp_key;
					flag=1;
				}

			}else{
				/*entering the key into the bucket*/
				table2->buckets[address]->key = key;
				table2->buckets[address]->full = true;
				table2->nkeys=table2->nkeys+1;
				table->time += clock() - start_time; // add time elapsed
				return(true);
			}
		}
	}
	table->time += clock() - start_time; // add time elapsed
	return false;
}

/******************************************************************************/
// lookup whether 'key' is inside 'table'
// returns true if found, false if not
bool xuckoo_hash_table_lookup(XuckooHashTable *table, int64 key) {
	assert(table);
	int start_time = clock(); // start timing
	/*checking table 1*/
	// calculate table address for this key
	int address = rightmostnbits(table->table1->depth, h1(key));

	// look for the key in that bucket (unless it's empty)
	if (table->table1->buckets[address]->full) {
		// found it?
		if (table->table1->buckets[address]->key == key){
			table->time += clock() - start_time; // add time elapsed
			return(true);
		}
	}
	/*checking table 2*/
	address = rightmostnbits(table->table2->depth, h2(key));

	// look for the key in that bucket (unless it's empty)
	if (table->table2->buckets[address]->full) {
		// found it?
		if (table->table2->buckets[address]->key == key){
			table->time += clock() - start_time; // add time elapsed
			return(true);
		}
	}
	table->time += clock() - start_time; // add time elapsed
	return false;
}

/******************************************************************************/
// print the contents of 'table' to stdout
void xuckoo_hash_table_print(XuckooHashTable *table) {
	assert(table != NULL);

	printf("--- table ---\n");

	// loop through the two tables, printing them
	InnerTable *innertables[2] = {table->table1, table->table2};
	int t;
	for (t = 0; t < 2; t++) {
		// print header
		printf("table %d\n", t+1);

		printf("  table:               buckets:\n");
		printf("  address | bucketid   bucketid [key]\n");

		// print table and buckets
		int i;
		for (i = 0; i < innertables[t]->size; i++) {
			// table entry
			printf("%9d | %-9d ", i, innertables[t]->buckets[i]->id);

			// if this is the first address at which a bucket occurs, print it
			if (innertables[t]->buckets[i]->id == i) {
				printf("%9d ", innertables[t]->buckets[i]->id);
				if (innertables[t]->buckets[i]->full) {
					printf("[%llu]", innertables[t]->buckets[i]->key);
				} else {
					printf("[ ]");
				}
			}

			// end the line
			printf("\n");
		}
	}
	printf("--- end table ---\n");
}

/******************************************************************************/

// print some statistics about 'table' to stdout
void xuckoo_hash_table_stats(XuckooHashTable *table) {
	/*prints out the number of keys stored in each table*/
	printf("Number of keys stored in Table 1: %d\n", table->table1->nkeys);
	printf("Number of keys stored in Table 2: %d\n", table->table2->nkeys);
	// also calculate CPU usage in seconds and print this
	float seconds = table->time * 1.0 /CLOCKS_PER_SEC;
	printf("    CPU time spent: %.6f sec\n", seconds);

	return;
}
