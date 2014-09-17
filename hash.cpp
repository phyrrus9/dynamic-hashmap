template <class T, class Q> hashmap<T, Q>::hashmap(unsigned int max, unsigned int bmax)
{
	check_types();
	table = new hash_entry<T, Q>*[max];
	max_entry        = max;
	bucket_max       = bmax;
	memset(table, 0, sizeof(hash_entry<T, Q> *) * max);
	curr_entry = 0;
}

template <class T, class Q> void hashmap<T, Q>::insert(T ent, Q id)
{
	unsigned int loc;
	assert(curr_entry < (max_entry * bucket_max));
	loc = genhash(id);
	if (table[loc] == NULL)
		table[loc] = new hash_entry<T, Q>(ent, id, loc, (hash_entry<T, Q> *)NULL);
	else if (buckets_left(loc))
	{
		hash_entry<T, Q> *head;
		for (head = table[loc]; head->next != NULL; head = head->next); //find last bucket
		head->next = new hash_entry<T, Q>(ent, id, loc, head);
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
	}
	//this->reorder();
}
template <class T, class Q> void hashmap<T, Q>::reorder()
{
	unsigned int i, j;
	hash_entry<T, Q> *arr = (hash_entry<T, Q> *)NULL, *tmp;
	arr = new hash_entry<T, Q>[curr_entry];
	for (i = j = 0; i < max_entry && j < curr_entry; i++)
		while (buckets_left(i)) //<--currently broken, causes segmentation fault here
		{
			tmp = (hash_entry<T, Q> *)NULL;
			if (table[i] == NULL) continue;
			arr[j++].set_base(*table[i]->data, table[i]->id);
			tmp = table[i];
			table[i] = table[i]->next;
			delete tmp;
		}
	for (j = 0; j < curr_entry; j++)
		this->insert(*tmp[j].data, tmp[j].id); //re-insert quickly
	delete[] arr; //clean up
	/* NOTE
	   later on, I will swap this method out for a bubble-sort-timed
	   method, allowing it to be more flexible, but this is quick for
	   now while I figure out how to stop the bubble sort without
	   using significant time
	   END NOTE */
}
