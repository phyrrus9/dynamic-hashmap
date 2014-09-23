/* Sample auto-dynamic hashmap unit test
 * Authors: Ethan Laur (@phyrrus9) <phyrrus9@gmail.com>
 */
#include "hashmap.h"
#include <stdio.h>

struct data
{
	int i;
	float f;
	char c;
};

template class hashmap<struct data, std::string>;

int main()
{
	struct data d[5] =
	{
		{ 1, 3.14, 'c' },
		{ 2, 5.8,  'f' },
		{ 5, 1.21, 'g' },
		{ -1, 2.0, 'a' },
		{ 0, 1.11, '0' }
	};
	hashmap<struct data, std::string> q(4, 6);
	q.insert(d[0], "a");
	q.insert(d[1], "b");
	q.insert(d[2], "c");
	q.insert(d[3], "d");
	q.insert(d[4], "e");
	q.remove("c");
	q.remove("a");
	q.remove("a");
	q.remove("a");
	q.reorder();
	printf("done!\n");
}
