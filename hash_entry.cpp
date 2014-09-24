/* FILE: 	hash_entry.cpp
 * Author(s):	Ethan Laur
 * Email(s):	phyrrus9@gmail.com
 * Updated:	09/24/2014
 */
template <class T, class Q>
hash_entry<T, Q>::hash_entry()
{
	next = NULL;
	data = NULL;
	parent = (hash_entry<T, Q> *)NULL;
	nodelete = false;
}
template <class T, class Q>
hash_entry<T, Q>::hash_entry(T ent, Q idin, unsigned long hash, unsigned int rhash, hash_entry<T, Q> *p)
{
	data = new T;
	memcpy(data, &ent, sizeof(T));
	id = idin;
	hashed_location = hash;
	parent = p;
	next = NULL;
	nodelete = false;
}
template <class T, class Q>
hash_entry<T, Q>::hash_entry(hash_entry<T, Q> &old)
{
	data = new T;
	memcpy(data, &old.data, sizeof(T));
	id = old.id;
	hashed_location = old.hashed_location;
	hash = old.hash;
	parent = old.parent;
	next = old.next;
	nodelete = false;
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
	if (nodelete) return;
	if (parent != NULL)
	{
		parent->next = this->next;
		if (this->next != NULL) this->next->parent = parent;
	}
	delete data;
}
