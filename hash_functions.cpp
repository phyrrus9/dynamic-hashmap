/* FILE: 	hash_functions.cpp
 * Author(s):	Ethan Laur
 * Email(s):	phyrrus9@gmail.com
 * Updated:	09/24/2014
 */
template <class T, class Q>
unsigned int hashmap<T, Q>::rotate_left(unsigned int in, int bits)
{
	unsigned int ret = in << 1;
	if (in >> bits)
		ret |= 1;
	return ret; //used to be return 0...this might be a bug in orig release
}
template <class T, class Q>
unsigned int hashmap<T, Q>::genhash(std::string in)
{
	char *str = (char *)in.c_str();
	unsigned int hash = str[0], len = strlen(str);
	int i;
	for (i = 1; i < len; i++)
		hash = rotate_left(hash ^ str[i], 8 * sizeof(unsigned int));
	return hash % max_entry;
}
template <class T, class Q>
unsigned int hashmap<T, Q>::genhash(unsigned int in)
{
	unsigned int tmp = /*htonl(in)*/in, *tmpr, ret, i; //<---------------fix htonl issue
	unsigned char *bytes = (unsigned char *)&tmp;
	for (i = 0; i < sizeof(unsigned int); i++)
		bytes[i] = rotate_left(bytes[i], 8) ^ bytes[i + 1 % sizeof(unsigned int)] & in;
	tmpr = (unsigned int *)bytes;
	return ret % max_entry;
}

