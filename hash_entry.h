/* FILE: 	hash_entry.h
 * Author(s):	Ethan Laur
 * Email(s):	phyrrus9@gmail.com
 * Updated:	09/24/2014
 */
#ifndef HASHMAP_ENTRY_H
#define HASHMAP_ENTRY_H
template <class T, class Q>
class hash_entry
{
public:
	hash_entry<T,Q>	*next;
	T 		*data;
	Q 		id;
	unsigned long	hashed_location, //NOTE: this is NOT the hash, but where it was put
			hash; //this is where it should go
	hash_entry<T,Q> *parent; //NULL if root of bucket
	bool		nodelete;
public:
			hash_entry	(									);
			hash_entry	(T		ent,	Q		idin,
					 unsigned long	hash,	unsigned int	rhash,
					 hash_entry<T,Q>	*p						);
			hash_entry	(hash_entry<T,Q>&old							);
	void 		set_base	(T		ent,	Q		idin				);
	virtual 	~hash_entry	(									);
};
#endif
