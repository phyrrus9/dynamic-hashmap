template <class T, class Q>
hash_entry<T, Q>::hash_entry()
{
	next = NULL;
	data = NULL;
	parent = (hash_entry<T, Q> *)NULL;
}
template <class T, class Q>
hash_entry<T, Q>::hash_entry(T ent, Q idin, unsigned long hash, hash_entry<T, Q> *p)
{
	data = new T;
	memcpy(data, &ent, sizeof(T));
	id = idin;
	hashed_location = hash;
	parent = p;
}
template <class T, class Q>
void hash_entry<T, Q>::set_base(T ent, Q idin)
{
	data = new T;
	memcpy(data, &ent, sizeof(T));
	id = idin;
	hashed_location = 0;
	parent = (hash_entry<T, Q> *)NULL;
}
template <class T, class Q>
hash_entry<T, Q>::~hash_entry()
{
	if (parent != NULL)
	{
		parent->next = this->next;
		if (this->next != NULL) this->next->parent = parent;
	}
	delete data;
}
