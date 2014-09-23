#include <unistd.h>
#define REMOVE_AND_REORDER (this->max_entry / 4) /* if you remove 1/4 of the elements, it should reorder */
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
		if (removal_count > REMOVE_AND_REORDER)
			this->reorder();
	}
}
template <class T, class Q> void hashmap<T, Q>::reorder(unsigned int tmax, bool go)
{
	this->max_time = tmax;
	if (go) this->reorder();
}
template <class T, class Q> void hashmap<T, Q>::reorder()
{
	unsigned int i, j, lp, ln, qi, get_current;
	hash_entry<T, Q> hash_tmp, *hash_obj;
	time_t end_time = time(NULL) + this->max_time;
	for (i = 0; i < max_entry && time(NULL) <= end_time; i++)
	{
		if (this->table[i] == NULL) continue;
		if (!buckets_left(i)) continue;
		for (i = 0; i < max_entry && time(NULL) <= end_time; i++)
		{
			get_current = 0;
			while ((hash_obj = get_object(i, &get_current, &hash_tmp)) != NULL) //see note in get_object
			{
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
