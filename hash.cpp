/* FILE: 	hash.cpp
 * Author(s):	Ethan Laur
 * Email(s):	phyrrus9@gmail.com
 * Updated:	09/24/2014
 */
#include <unistd.h>
#define REMOVE_AND_REORDER ((this->max_entry / 4) - (this->bucket_max / 8)) /* you get the point.. */
unsigned int removal_count; /* file global, incremented in remove(), set to zero in constructor and reorder() */
template <class T, class Q> hashmap<T, Q>::hashmap(unsigned int max, unsigned int bmax)
{
	check_types();
	table = new hash_entry<T, Q>*[max];
	max_entry        = max;
	bucket_max       = bmax;
	memset(table, 0, sizeof(hash_entry<T, Q> *) * max);
	curr_entry = 0;
	max_time = 0;
	max_time_infinite = false;
	removal_count = 0; //<---file GLOBAL!
}

template <class T, class Q> void hashmap<T, Q>::insert(T ent, Q id)
{
	unsigned int loc;
	assert(curr_entry < (max_entry * bucket_max));
	loc = genhash(id);
	if (table[loc] == NULL)
		table[loc] = new hash_entry<T, Q>(ent, id, loc, loc, (hash_entry<T, Q> *)NULL);
	else if (buckets_left(loc))
	{
		hash_entry<T, Q> *head;
		for (head = table[loc]; head->next != NULL; head = head->next); //find last bucket
		head->next = new hash_entry<T, Q>(ent, id, loc, loc, head);
	}
	else insert_qprobe(ent, id, loc);
	curr_entry++;
}
template <class T, class Q> T hashmap<T, Q>::get(Q id)
{
	T *ret = (T *)NULL;
	assert((ret = get_reference(id)) != NULL);
	return *ret;
}
template <class T, class Q> bool hashmap<T, Q>::exists(Q id, hash_entry<T, Q> **irv) //double pointer used to assign location
{ //normal use (as a user): exists(id); use internally: exists(id, &ptr);
	hash_entry<T, Q> *ret = get_object(id);
	*irv = irv == NULL ? NULL : ret;
	return ret != NULL;
}
template <class T, class Q> void hashmap<T, Q>::remove(Q id)
{
	hash_entry<T, Q> *ptr = (hash_entry<T, Q> *)NULL;
	if (exists(id, &ptr)) //we know a hash with id is there, and where it is
	{
		if (bucket_max == 1) //no buckets
		{
			table[ptr->hashed_location] = NULL; //mark it as unused
			delete ptr;				
		}
		else
		{
			if (ptr->parent == NULL) //wow...wasnt working, because it `was` [if (ptr->parent = NULL)]
			{
				table[ptr->hashed_location] = ptr->next;
				if (ptr->next != NULL) ptr->next->parent = NULL;
			}
			delete ptr;
		}
		curr_entry--;
		++removal_count;
		if (removal_count > REMOVE_AND_REORDER) this->reorder();
	}
}
template <class T, class Q> void hashmap<T, Q>::reorder(unsigned int tmax, reorder_args options)
{
	unsigned int old_tmax = this->max_time; //save it, so if its a reorder_now, we can restore
	if (options & REORDER_SET) this->max_time = tmax;
	if (options & REORDER_INF) this->max_time_infinite = true;
	else                       this->max_time_infinite = false;
	if (options & REORDER_GO)  this->reorder();
	if (options & REORDER_NOW) this->max_time = tmax;
}
template <class T, class Q> void hashmap<T, Q>::reorder()
{
	unsigned int i, j, lp, ln, qi, get_current;
	hash_entry<T, Q> hash_tmp, *hash_obj;
	time_t end_time = time(NULL) + this->max_time;
	for (i = 0; i < max_entry && (time(NULL) <= end_time || this->max_time_infinite); i++)
	{
		if (this->table[i] == NULL) continue;
		if (!buckets_left(i)) continue;
		for (i = 0; i < max_entry && (time(NULL) <= end_time || this->max_time_infinite); i++)
		{
			get_current = 0;
			while ((hash_obj = get_object(i, &get_current, &hash_tmp)) != NULL) //see note in get_object
			{
				if (!(time(NULL) <= end_time || this->max_time_infinite)) break; //get_object takes time too!
				if (hash_obj->hashed_location == i) continue;
				hash_entry<T, Q> hash_buf(*hash_obj);
				this->remove(hash_obj->id);
				this->insert(*hash_buf.data, hash_buf.id);
				delete hash_buf.data; //since we can't call delete directly here
				hash_buf.nodelete = true; //so it won't relink for us
				removal_count = removal_count <= 0 ? 0 : removal_count - 1;
			}
		}
	}
}
