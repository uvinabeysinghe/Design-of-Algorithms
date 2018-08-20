/* * * * * * * * *
 * Dynamic hash table using cuckoo hashing, resolving collisions by switching
 * keys between two tables with two separate hash functions
 *
 * created for COMP20007 Design of Algorithms - Assignment 2, 2017
 * by ...
 */
/* we dont check in use when checking if its the same in both inserting and look up*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "cuckoo.h"
/******************************************************************************/
// an inner table represents one of the two internal tables for a cuckoo
// hash table. it stores two parallel arrays: 'slots' for storing keys and
// 'inuse' for marking which entries are occupied
typedef struct inner_table {
	int64 *slots;	// array of slots holding keys
	bool  *inuse;	// is this slot in use or not?
} InnerTable;

// a cuckoo hash table stores its keys in two inner tables
struct cuckoo_table {
	InnerTable *table1; // first table
	InnerTable *table2; // second table
	int size;			//size of each tables
	int load1;			//number of keys in table 2
	int load2;			//number of keys in table 1
	int rep;			// number of repeated inserts
	int time;		// how much CPU time has been used to insert/lookup keys
					// in this table
};
/******************************************************************************/
// malloc and initalize slots
void initialise_slots(CuckooHashTable *tables, int size ) {
	assert(tables);
	/*allocating memory table 1 slots*/
	tables->table1->slots = malloc((sizeof (*tables->table1->slots)) * size);
	assert(tables->table1->slots);
	tables->table1->inuse = malloc((sizeof (*tables->table1->inuse)) * size);
	assert(tables->table1->inuse);

	/*allocating memory table 2 slots*/
	tables->table2->slots = malloc((sizeof *tables->table2->slots) * size);
	assert(tables->table2->slots);
	tables->table2->inuse = malloc((sizeof *tables->table2->inuse) * size);
	assert(tables->table2->inuse);

	/*initializing the tables*/
	int i;
	for (i = 0; i < size; i++) {
		tables->table1->inuse[i] = false;
		tables->table2->inuse[i] = false;
	}

	tables->load1=0;
	tables->load2=0;
}

/******************************************************************************/
// initialise a cuckoo hash table with 'size' slots in each table
CuckooHashTable *new_cuckoo_hash_table(int size) {
	struct cuckoo_table *tables = malloc(sizeof(struct cuckoo_table));
	/*allocating memory for tables*/
	tables->table1=malloc(sizeof(InnerTable));
	tables->table2=malloc(sizeof(InnerTable));
	tables->rep=0;
	tables->time = 0;


	initialise_slots(tables, size);
	/*initializing the slot size*/
	tables->size = size;

	return tables;

}

/******************************************************************************/
// free all memory associated with 'table'
void free_cuckoo_hash_table(CuckooHashTable *table) {
	assert(table != NULL);

	// free the table's arrays
	free(table->table1->slots);
	free(table->table2->slots);
	free(table->table1->inuse);
	free(table->table2->inuse);

	free(table->table1);
	free(table->table2);

	// free the tables struct itself
	free(table);
}

/******************************************************************************/
// double the size of the internal table arrays and re-hash all
// keys in the old tables
void
double_table(struct cuckoo_table *table){
	assert(table);
	InnerTable *table1=table->table1;
	InnerTable *table2=table->table2;

	int oldsize = table->size;
	table->size=oldsize*2;

	assert(MAX_TABLE_SIZE>table->size);
	/*temp pointer to slots before initializing*/
	int64 *oldslots1 = table1->slots;
	bool  *oldinuse1 = table1->inuse;

	int64 *oldslots2 = table2->slots;
	bool  *oldinuse2 = table2->inuse;


	initialise_slots(table, table->size);
	/*re-enter values to the hash table*/
	int i;
	for (i = 0; i < oldsize; i++) {
		if (oldinuse1[i] == true) {
			cuckoo_hash_table_insert(table, oldslots1[i]);
		}
		if (oldinuse2[i] == true) {
			cuckoo_hash_table_insert(table, oldslots2[i]);
		}
	}


}

/******************************************************************************/
// insert 'key' into 'table', if it's not in there already
// returns true if insertion succeeds, false if it was already in there
bool cuckoo_hash_table_insert(CuckooHashTable *table, int64 key) {
	assert(table);
	int start_time = clock(); // start timing
	/*steeing pointers for the 2 tables*/
	InnerTable *table1=table->table1;
	InnerTable *table2=table->table2;
	int size=table->size;
	/*Variable used to toogle between the tables*/
	int flag=1;
	int64 temp_key;
	/*variable tracking the number of swaps*/
	int loop_size=0;
	while (flag<3){
		size=table->size;
		loop_size++;
		if (flag==1){
			/*checking table 1*/
			/*if table 1 slot empty*/
			if (!(table1->inuse[h1(key)%size])){
				table1->slots[h1(key)%size]=key;
				table1->inuse[h1(key)%size]=true;
				table->load1=table->load1+1;
				table->time += clock() - start_time; // add time elapsed
				return(true);
			}else{
				/*if table 1 slot not empty*/
				/*checking if its already present*/

				if (cuckoo_hash_table_lookup(table, key)){
					table->rep=table->rep+1;
					table->time += clock() - start_time; // add time elapsed
					return(false);
				}else{
					/*if another int64 present there, switch to table2*/
					temp_key=table1->slots[h1(key)%size];
					table1->slots[h1(key)%size]=key;
					key=temp_key;
				}
			}
			flag=2;
		}else{
			/*checking table 2*/
			/*if table 2 slot empty*/
			if (!(table2->inuse[h2(key)%size])){
				table2->slots[h2(key)%size]=key;
				table2->inuse[h2(key)%size]=true;
				table->load2=table->load2+1;
				table->time += clock() - start_time; // add time elapsed
				return(true);
			}else{
				/*if table 2 slot not empty*/
				/*checking if its already present*/
				if (cuckoo_hash_table_lookup(table, key)){
					table->rep=table->rep+1;
					table->time += clock() - start_time; // add time elapsed
					return(false);
				}else{
					/*if another int64 present there, switch to table2*/
					temp_key=table2->slots[h2(key)%size];
					table2->slots[h2(key)%size]=key;
					key=temp_key;
				}
			}
			flag=1;
		}
		/*Checking if its the time to double the tables*/
		if (loop_size>((table->load1)+(table->load2)+10)){
			flag=1;
			loop_size=0;
			double_table(table);

		}
	}
	table->time += clock() - start_time; // add time elapsed
	return false;
}




/******************************************************************************/
// lookup whether 'key' is inside 'table'
// returns true if found, false if not
bool cuckoo_hash_table_lookup(CuckooHashTable *table, int64 key) {
	assert(table);
	int start_time = clock(); // start timing
	/*checking if the key is any of the tables*/
	if (((table->table1->slots[h1(key)%(table->size)])==key) || ((table->table2->slots[h2(key)%(table->size)])==key)){
		table->time += clock() - start_time;
		return(true);
	}
	table->time += clock() - start_time;
	return(false);
}

/******************************************************************************/
// print the contents of 'table' to stdout
void cuckoo_hash_table_print(CuckooHashTable *table) {
	assert(table);
	printf("--- table size: %d\n", table->size);

	// print header
	printf("                    table one         table two\n");
	printf("                  key | address     address | key\n");

	// print rows of each table
	int i;
	for (i = 0; i < table->size; i++) {

		// table 1 key
		if (table->table1->inuse[i]) {
			printf(" %20llu ", table->table1->slots[i]);
		} else {
			printf(" %20s ", "-");
		}

		// addresses
		printf("| %-9d %9d |", i, i);

		// table 2 key
		if (table->table2->inuse[i]) {
			printf(" %llu\n", table->table2->slots[i]);
		} else {
			printf(" %s\n",  "-");
		}
	}

	// done!
	printf("--- end table ---\n");
}

/******************************************************************************/

// print some statistics about 'table' to stdout
void cuckoo_hash_table_stats(CuckooHashTable *table) {
	assert(table);
	/*printing the number of keys in each table and the repititions*/
	printf("Table 1 load factor is %f\n", 100.0*(table->load1)/ (table->size)*1.0);
	printf("Table 2 load factor is %f\n", 100.0*(table->load2)/ (table->size)*1.0);
	printf("rep is %d\n", table->rep);
	float seconds = table->time * 1.0 /CLOCKS_PER_SEC;
	printf("    CPU time spent: %.6f sec\n", seconds);
}
