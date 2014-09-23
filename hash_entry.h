template <class T, class Q>
class hash_entry
{
public:
	hash_entry<T, Q> *next;
	T *data;
	Q id;
	unsigned long hashed_location; //NOTE: this is NOT the hash, but where it was put
	unsigned long hash; //this is where it should go
	hash_entry<T, Q> *parent; //NULL if root of bucket
	bool nodelete;
public:
	hash_entry();
	hash_entry(T ent, Q idin, unsigned long hash, unsigned int rhash, hash_entry<T, Q> *p);
	hash_entry(hash_entry<T, Q> &old);
	void set_base(T ent, Q idin);
	virtual ~hash_entry();
};
