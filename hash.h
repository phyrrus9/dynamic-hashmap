/* Auto-dynamic hashmap class
 * Authors: Ethan Laur (@phyrrus9) <phyrrus9@gmail.com>
 */
#ifndef HASHMAP_H
#define HASHMAP_H
#include <string.h>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <typeinfo>
template <class T, class Q>
class hashmap
{
private:
	unsigned int rotate_left(unsigned int in, int bits);
	unsigned int genhash(std::string in);
	unsigned int genhash(unsigned int in);
	bool valid_type(const char * str) const;
	unsigned int max_time;
#define check_types(); assert(valid_type(typeid(Q).name()) && "Invalid key type");
protected:
	unsigned int max_entry, curr_entry, bucket_max;
	hash_entry<T, Q> * *table;
	bool buckets_left(unsigned int hash);
	void insert_qprobe(T ent, Q id, unsigned int hash, int exponent = 1);
	hash_entry<T, Q> *get_bucket(Q id, unsigned long hash); //retrieves a key from a bucket
	hash_entry<T, Q> *get_qprobe(Q id, unsigned long hash, unsigned int exponent = 2);
	hash_entry<T, Q> *get_object(Q id);
	hash_entry<T, Q> *get_object(unsigned long hash, unsigned int *curr, hash_entry<T, Q> *prev);
	T *get_reference(Q id);
public:
	hashmap(unsigned int max, unsigned int bmax = 1);
	void insert(T ent, Q id);
	T get(Q id);
	bool exists(Q id, hash_entry<T, Q> **irv = NULL); //double pointer used to assign location
	void remove(Q id);
	void reorder();
	void reorder(unsigned int tmax, bool go = false);
};
#endif
