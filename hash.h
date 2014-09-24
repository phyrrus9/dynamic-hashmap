/* FILE: 	hash.h
 * Author(s):	Ethan Laur
 * Email(s):	phyrrus9@gmail.com
 * Updated:	09/24/2014
 */
#ifndef HASHMAP_H
#define HASHMAP_H
#include <string.h>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <typeinfo>
#define REORDER_SET	(0x01 << 0x00)
#define REORDER_GO	(0x01 << 0x01)
#define REORDER_NOW	(0x01 << 0x02)
#define REORDER_INF	(0x01 << 0x03)
typedef unsigned char reorder_args;
template <class T, class Q>
class hashmap
{
#define check_types(); assert(valid_type(typeid(Q).name()) && "Invalid key type");
protected:
	unsigned int	max_time;
	bool 		max_time_infinite;
	unsigned int	max_entry,
			curr_entry,
			bucket_max;
	hash_entry<T,Q>	**table;
	bool		buckets_left	(unsigned int	hash						);
	void 		insert_qprobe	(T 		ent,	Q		id,
					 unsigned int 	hash,	int		exponent = 1		);
	hash_entry<T,Q>	*get_bucket	(Q		id,	unsigned long	hash			); //retrieves a key from a bucket
	hash_entry<T,Q>	*get_qprobe	(Q		id,	unsigned long	hash,
					 unsigned int	exponent = 2					);
	hash_entry<T,Q>	*get_object	(Q		id						);
	hash_entry<T,Q>	*get_object	(unsigned long 	hash,	unsigned int	*curr,
					 hash_entry<T,Q>*prev						);
	T 		*get_reference	(Q		id						);
private:
	unsigned int	rotate_left	(unsigned int	in,	int		bits			);
	unsigned int	genhash		(std::string 	in						);
	unsigned int	genhash		(unsigned int	in						);
	bool 		valid_type	(const char 	*str						) const;
public:
			hashmap		(unsigned int	max,	unsigned int	bmax = 1		);
	void		insert		(T		ent,	Q		id			);
	T		get		(Q		id						);
	bool		exists		(Q		id,	hash_entry<T,Q> **irv = NULL		); //double pointer used to assign location
	void		remove		(Q		id						);
	void		reorder		(								);
	void		reorder		(unsigned int	tmax,	reorder_args options = REORDER_SET	);
};
#endif
