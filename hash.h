/* Auto-dynamic hashmap class
 * Authors: Ethan Laur (@phyrrus9) <phyrrus9@gmail.com>
 */
#include <string.h>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#ifndef HASHMAP_H
#define HASHMAP_H
unsigned int rotate_left(unsigned int in)
{
	unsigned int ret = in << 1;
	if (in >> 31)
		ret |= 1;
	return 0;
}

template <class T>
class hash_entry
{
#define HASHENT_DEBUG_OUT   0
#define HASHENT_DEBUG_FILE  stderr
public:
	hash_entry<T> *next;
	T *data;
	std::string id;
	unsigned long hashed_location; //NOTE: this is NOT the hash, but where it was put
	hash_entry<T> *parent; //NULL if root of bucket
public:
	hash_entry()
	{
		next = NULL;
		data = NULL;
		id = "";
		parent = (hash_entry<T> *)NULL;
	}
	hash_entry(T ent, std::string idin, unsigned long hash, hash_entry<T> *p)
	{
		data = new T;
		memcpy(data, &ent, sizeof(T));
		id = idin;
		hashed_location = hash;
		parent = p;
	}
	void set_base(T ent, std::string idin)
	{
		data = new T;
		memcpy(data, &ent, sizeof(T));
		id = idin;
		hashed_location = 0;
		parent = (hash_entry<T> *)NULL;
	}
	virtual ~hash_entry()
	{
		if (parent != NULL)
		{
			parent->next = this->next;
			this->next->parent = parent;
		}
		id = "";
		delete data;
	}
#undef HASHENT_DEBUG_OUT
#undef HASHENT_DEBUG_FILE
};

#define REORDER_SET    (0x01 << 0x00)
#define REORDER_GO     (0x01 << 0x01)
#define REORDER_ASYNC  (0x01 << 0x02) /* leaving this one out for now */
#define REORDER_NOTIME (0x01 << 0x03)
template <class T>
class hashmap
{
#define HASHMAP_DEBUG_OUT   0
#define HASHMAP_DEBUG_FILE  stderr
protected:
	unsigned int max_entry, curr_entry, bucket_max;
	hash_entry<T> * *table;
	int timer_max;
	unsigned char timer_good;
	unsigned int genhash(std::string in)
	{
		char *str = (char *)in.c_str();
		unsigned int hash = str[0], len = strlen(str);
		int i;
		for (i = 1; i < len; i++)
		{
			hash ^= str[i];
			hash = rotate_left(hash);
		}
		return hash % max_entry;
	}
	bool buckets_left(unsigned int hash) //determines if position full or not
	{
		hash_entry<T> *head = table[hash];
		int i;
		for (i = 1; i < bucket_max; i++, head = head->next)
			if (head->next == NULL)
				return true;
		return false;
	}
/*	void reorder_timeup(int s)
	{
		signal(SIGALRM, SIG_IGN);
		this->timer_good = 0;	//when the signal is done, reorder() will
					//finish its last process, and stop.
	} */ //commented for now...
	void insert_qprobe(T ent, std::string id, unsigned int hash, int exponent = 1)
	{
		unsigned int loc = hash;
		int modifier = exponent;
		loc = (hash + modifier) % max_entry;
	qloop_top:
		if (table[loc] == NULL)
		{
			table[loc] = new hash_entry<T>(ent, id, loc, (hash_entry<T> *)NULL);
			return;
		}
		else if (buckets_left(loc))
		{
			hash_entry<T> *head;
			for (head = table[loc]; head->next != NULL; head = head->next); //find last bucket
			head->next = new hash_entry<T>(ent, id, loc, head);
			return;			
		}
		if (loc > hash)
		{
			if (modifier > hash)
				loc = (((int)hash - modifier) * -1) % max_entry;
			else
				loc = hash - modifier;
			goto qloop_top;
		}
		insert_qprobe(ent, id, hash, exponent * 2);
	}
	hash_entry<T> *get_bucket(std::string id, unsigned long hash) //retrieves a key from a bucket
	{
		hash_entry<T> *head = table[hash]->next;
		int i;
		if (head == NULL) return (hash_entry<T> *)NULL;
		for (i = 1; i < bucket_max; i++, head = head->next) //i is 1 here because head is bucket[1]
		{
			if (head == NULL) return (hash_entry<T> *) NULL;
			else if (head->id == id)
				return head;
		}
		return (hash_entry<T> *)NULL;
	}
	hash_entry<T> *get_qprobe(std::string id, unsigned long hash, unsigned int exponent = 2)
	{
		unsigned int loc;
		unsigned char sign = 0; //0 = +, 1 = -
		loc = (hash + exponent) % max_entry;
		hash_entry<T> *ret = (hash_entry<T> *)NULL;
	gqloop_top:
		if ((loc > hash && sign == 1) || (loc < hash && sign == 0)) //-/+ and full rollover
			return (hash_entry<T> *)NULL;
		if (table[loc] != NULL)
		{
			if (table[loc]->id == id)
				return table[loc];
			else if ((ret = get_bucket(id, loc)) != NULL)
				return ret;
		}
		if (sign == 0)
		{
			if (exponent > hash)
				loc = ((hash - exponent) * -1) % max_entry;
			else
				loc = (hash - exponent);
			sign = 1;
			goto gqloop_top;
		}
		return get_qprobe(id, hash, exponent == 0 ? 1 : exponent * 2);
	}
	hash_entry<T> *get_object(std::string id)
	{
		unsigned int loc = genhash(id); //this is where it should be
		hash_entry<T> *ret = (hash_entry<T> *)NULL;
		assert(table[loc] != NULL);
		if (table[loc]->id == id)
			return table[loc];
		else
		{
			if ((ret = get_bucket(id, loc)) != NULL)
				return ret;
			else
				return get_qprobe(id, loc);
		}
	}
	T *get_reference(std::string id)
	{
		hash_entry<T> *ret = (hash_entry<T> *)NULL;
		ret = get_object(id); 	//NULL if not found, this will throw
					//the assert in T get(std::string id)
					//if used in a hashmap::get call..
					//useful if checking for exists
		return ret->data;
	}
public:
	hashmap(unsigned int max, unsigned int bmax = 1)
	{
		unsigned int i;
		table = new hash_entry<T>*[max];
		max_entry        = max;
		bucket_max       = bmax;
		for (i = 0; i < max_entry; i++)
			table[i] = NULL;
		timer_max = 0;
		curr_entry = 0;
	}
	void insert(T ent, std::string id)
	{
		unsigned int loc;
		assert(curr_entry < (max_entry * bucket_max));
		loc = genhash(id);
		if (table[loc] == NULL)
			table[loc] = new hash_entry<T>(ent, id, loc, (hash_entry<T> *)NULL);
		else if (buckets_left(loc))
		{
			hash_entry<T> *head;
			for (head = table[loc]; head->next != NULL; head = head->next); //find last bucket
			head->next = new hash_entry<T>(ent, id, loc, head);
		}
		else //we repeat with qprobe
		{
			insert_qprobe(ent, id, loc);
		}
		curr_entry++;
	}
	T get(std::string id)
	{
		T *ret = (T *)NULL;
		assert((ret = get_reference(id)) != NULL);
		return *ret;
	}
	/* exists, checks to see if an object exists. can be used to get
	   the object itself if irv is specified. Intended to be used in
	   non-internal code by hashmap::exists(id); And internally by
	   this->exists(id, &ptr); */
	bool exists(std::string id, hash_entry<T> **irv = NULL) //double pointer used to assign location
	{
		hash_entry<T> *ret = get_object(id);
		*irv = irv == NULL ? NULL : ret;
		return ret != NULL;
	}
	void remove(std::string id)
	{
		hash_entry<T> *ptr = (hash_entry<T> *)NULL;
		if (exists(id, &ptr)) //we know a hash with id is there, and where it is
		{
			if (bucket_max == 1) //no buckets
			{
				table[ptr->hashed_location] = NULL; //mark it as unused
				if (HASHMAP_DEBUG_OUT) fprintf(	HASHMAP_DEBUG_FILE,
								"set %p->hashed_location(%lu) = NULL\n",
								ptr, ptr->hashed_location);
				delete ptr;				
			}
			else
			{
				if (ptr->parent == NULL) //wow...wasnt working, because it `was` [if (ptr->parent = NULL)]
				{
					table[ptr->hashed_location] = ptr->next;
					if (ptr->next != NULL) ptr->next->parent = NULL;
					if (HASHMAP_DEBUG_OUT) fprintf(	HASHMAP_DEBUG_FILE,
									"set %p(%s)->hashed_location(%lu) = %p\n",
									ptr, ptr->id.c_str(), ptr->hashed_location, ptr->next);
				}
				else if (HASHMAP_DEBUG_OUT) fprintf(	HASHMAP_DEBUG_FILE,
								"set %p(%s)->hashed_location(%lu)[BUCKET] = %p\n",
								ptr, ptr->id.c_str(), ptr->hashed_location, ptr->next);
				delete ptr;
			}
			curr_entry--;
		}
		this->reorder(-1, REORDER_GO); //automatically reorder, if set bit is on
	}
	void reorder(int timer = -1, unsigned char action = REORDER_NOTIME) //0 = set, 2 = go, 3 = set & go, 4 = go without time
	{
		int mtime = 0;
		unsigned int i, j;
		hash_entry<T> *arr = (hash_entry<T> *)NULL, *tmp;
		if (timer <= 0)
		{
			if (action & REORDER_SET)
				this->timer_max = mtime = timer;
			if (action & REORDER_GO)
				mtime = timer;
		}
		else if (timer >= 0 || action & REORDER_NOTIME)
			mtime = this->timer_max;
		if (mtime <= 0) return; //save some processor...why not
		//signal(SIGALRM, reorder_timeup); //do the init, but dont set the clock yet
		arr = new hash_entry<T>[curr_entry];
		for (i = j = 0; i < max_entry && j < curr_entry; i++)
		{
			while (buckets_left(i))
			{
				tmp = (hash_entry<T> *)NULL;
				if (table[i] == NULL) continue;
				arr[j++].set_base(*table[i]->data, table[i]->id);
				tmp = table[i];
				table[i] = table[i]->next;
				delete tmp;
			}
		}
		for (j = 0; j < curr_entry; j++)
			this->insert(*tmp[j].data, tmp[j].id); //re-insert quickly
		delete[] arr; //clean up
		this->timer_good = 1;
		/* NOTE
		   later on, I will swap this method out for a bubble-sort-timed
		   method, allowing it to be more flexible, but this is quick for
		   now while I figure out how to stop the bubble sort without
		   using significant time
		   END NOTE */
	}
	void reorder(unsigned action = REORDER_GO | REORDER_NOTIME) { this->reorder(-1, action & REORDER_SET ? action ^ REORDER_SET : action); }
#undef HASHMAP_DEBUG_OUT
#undef HASHMAP_DEBUG_FILE
};
#endif
