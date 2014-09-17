template <class T, class Q>
class hash_entry
{
public:
	hash_entry<T, Q> *next;
	T *data;
	Q id;
	unsigned long hashed_location; //NOTE: this is NOT the hash, but where it was put
	hash_entry<T, Q> *parent; //NULL if root of bucket
public:
	hash_entry();
	hash_entry(T ent, Q idin, unsigned long hash, hash_entry<T, Q> *p);
	void set_base(T ent, Q idin);
	virtual ~hash_entry();
};
