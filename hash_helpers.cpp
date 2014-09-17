#define HASHMAP_EXPOSE_TYPE(k); { if (strcmp(str, typeid(k).name()) == 0) return true; }
template <class T, class Q>
bool hashmap<T, Q>::valid_type(const char * str) const
{
	HASHMAP_EXPOSE_TYPE(std::string);
	HASHMAP_EXPOSE_TYPE(unsigned int);
	return false;
}
#undef HASHMAP_EXPOSE_TYPE
template <class T, class Q>
bool hashmap<T, Q>::buckets_left(unsigned int hash) //determines if position full or not
{
	hash_entry<T, Q> *head = table[hash];
	for (int i = 1; i < bucket_max && head != NULL; i++, head = head->next)
		if (head->next == NULL)
			return true;
	return false;
}
template <class T, class Q>
void hashmap<T, Q>::insert_qprobe(T ent, Q id, unsigned int hash, int exponent)
{
	unsigned int loc = hash;
	loc = (hash + exponent) % max_entry;
qloop_top:
	if (table[loc] == NULL)
		table[loc] = new hash_entry<T, Q>(ent, id, loc, (hash_entry<T, Q> *)NULL);
	else if (buckets_left(loc))
	{
		hash_entry<T, Q> *head;
		for (head = table[loc]; head->next != NULL; head = head->next); //find last bucket
		head->next = new hash_entry<T, Q>(ent, id, loc, head);			
	}
	else
	{
		if (loc > hash)
		{
			if (exponent > hash)
				loc = (((int)hash - exponent) * -1) % max_entry;
			else
				loc = hash - exponent;
			goto qloop_top;
		}
		insert_qprobe(ent, id, hash, exponent * 2); //possible tricks coming into recursion to eliminate all this
							    //code dealing with the - half
	}
}
template <class T, class Q>
hash_entry<T, Q> *hashmap<T, Q>::get_bucket(Q id, unsigned long hash) //retrieves a key from a bucket
{
	hash_entry<T, Q> *head = table[hash]->next;
	int i;
	if (head == NULL) return (hash_entry<T, Q> *)NULL;
	for (i = 1; i < bucket_max; i++, head = head->next) //i is 1 here because head is bucket[1]
	{
		if (head == NULL) return (hash_entry<T, Q> *) NULL;
		else if (head->id == id) return head;
	}
	return (hash_entry<T, Q> *)NULL;
}
template <class T, class Q>
hash_entry<T, Q> *hashmap<T, Q>::get_qprobe(Q id, unsigned long hash, unsigned int exponent)
{
	unsigned int loc;
	unsigned char sign = 0; //0 = +, 1 = -
	loc = (hash + exponent) % max_entry;
	hash_entry<T, Q> *ret = (hash_entry<T, Q> *)NULL;
gqloop_top:
	if ((loc > hash && sign == 1) || (loc < hash && sign == 0)) //-/+ and full rollover
		return (hash_entry<T, Q> *)NULL;
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
template <class T, class Q>
hash_entry<T, Q> *hashmap<T, Q>::get_object(Q id)
{
	unsigned int loc = genhash(id); //this is where it should be
	hash_entry<T, Q> *ret = (hash_entry<T, Q> *)NULL;
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
template <class T, class Q>
T *hashmap<T, Q>::get_reference(Q id)
{
	hash_entry<T, Q> *ret = (hash_entry<T, Q> *)NULL;
	ret = get_object(id); 	//NULL if not found, this will throw
				//the assert in T get(std::string id)
				//if used in a hashmap::get call..
				//useful if checking for exists
	return ret->data;
}
