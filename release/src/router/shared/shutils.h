/*
 * Shell-like utility functions
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id: shutils.h,v 1.8 2005/03/07 08:35:32 kanki Exp $
 */

#ifndef _shutils_h_
#define _shutils_h_
#include <string.h>

#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)
#define sin6_addr(s) (((struct sockaddr_in6 *)(s))->sin6_addr)

/*
 * Reads file and returns contents
 * @param	fd	file descriptor
 * @return	contents of file or NULL if an error occurred
 */
extern char * fd2str(int fd);

/*
 * Reads file and returns contents
 * @param	path	path to file
 * @return	contents of file or NULL if an error occurred
 */
extern char * file2str(const char *path);

/* 
 * Waits for a file descriptor to become available for reading or unblocked signal
 * @param	fd	file descriptor
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @return	1 if descriptor changed status or 0 if timed out or -1 on error
 */
//	extern int waitfor(int fd, int timeout);

/* 
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @param	path	NULL, ">output", or ">>output"
 * @param	timeout	seconds to wait before timing out or 0 for no timeout
 * @param	ppid	NULL to wait for child termination or pointer to pid
 * @return	return value of executed command or errno
 */
extern int _eval(char *const argv[], const char *path, int timeout, pid_t *ppid);

/* 
 * Concatenates NULL-terminated list of arguments into a single
 * commmand and executes it
 * @param	argv	argument list
 * @return	stdout of executed command or NULL if an error occurred
 */
//	extern char * _backtick(char *const argv[]);

/* 
 * Kills process whose PID is stored in plaintext in pidfile
 * @param	pidfile	PID file
 * @return	0 on success and errno on failure
 */
extern void killall_tk_period_wait(const char *name, int wait_ds);
extern int kill_pidfile_s(char *pidfile, int sig);
extern void killall_and_waitfor(const char *name, int loop, int killtime);

/*
 * fread() with automatic retry on syscall interrupt
 * @param	ptr	location to store to
 * @param	size	size of each element of data
 * @param	nmemb	number of elements
 * @param	stream	file stream
 * @return	number of items successfully read
 */
extern int safe_fread(void *ptr, size_t size, size_t nmemb, FILE *stream);

/*
 * fwrite() with automatic retry on syscall interrupt
 * @param	ptr	location to read from
 * @param	size	size of each element of data
 * @param	nmemb	number of elements
 * @param	stream	file stream
 * @return	number of items successfully written
 */
extern int safe_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);

/*
 * Convert Ethernet address string representation to binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @param	e	binary data
 * @return	TRUE if conversion was successful and FALSE otherwise
 */
extern int ether_atoe(const char *a, unsigned char *e);

/*
 * Convert Ethernet address binary data to string representation
 * @param	e	binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @return	a
 */
extern char * ether_etoa(const unsigned char *e, char *a);

/*
 * Concatenate two strings together into a caller supplied buffer
 * @param	s1	first string
 * @param	s2	second string
 * @param	buf	buffer large enough to hold both strings
 * @return	buf
 */
static inline char *strlcat_r(const char *s1, const char *s2, char *buf, const size_t buf_len)
{
	strlcpy(buf, s1, buf_len);
	strlcat(buf, s2, buf_len);
	return buf;
}
/*
static inline char * strcat_r(const char *s1, const char *s2, char *buf)
{
	strcpy(buf, s1);
	strcat(buf, s2);
	return buf;
}
*/
/* Strip trailing CR/NL from string <s> */
#define chomp(s) ({ \
	char *c = (s) + strlen((s)) - 1; \
	while ((c > (s)) && (*c == '\n' || *c == '\r' || *c == ' ')) \
		*c-- = '\0'; \
	s; \
})


/* Simple version of _eval() (no timeout and wait for child termination) */
#if 1
#define eval(cmd, args...) ({ \
	char *argv[] = { cmd, ## args, NULL }; \
	_eval(argv, NULL, 0, NULL); \
})
#else
#define eval(cmd, args...) ({ \
	char *argv[] = { cmd, ## args, NULL }; \
	_eval(argv, ">/dev/console", 0, NULL); \
})
#endif

/* Copy each token in wordlist delimited by space into word */
#define foreach(word, wordlist, next) \
	for (next = &wordlist[strspn(wordlist, " ")], \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '); \
	     strlen(word); \
	     next = next ? &next[strspn(next, " ")] : "", \
	     strncpy(word, next, sizeof(word)), \
	     word[strcspn(word, " ")] = '\0', \
	     word[sizeof(word) - 1] = '\0', \
	     next = strchr(next, ' '))

/* Return NUL instead of NULL if undefined */
#define safe_getenv(s) (getenv(s) ? : "")

extern void dbg(const char * format, ...);
extern void cprintf(const char *format, ...);


/*
 * Parse the unit and subunit from an interface string such as wlXX or wlXX.YY
 *
 * @param	ifname	interface string to parse
 * @param	unit	pointer to return the unit number, may pass NULL
 * @param	subunit	pointer to return the subunit number, may pass NULL
 * @return	Returns 0 if the string ends with digits or digits.digits, -1 otherwise.
 *		If ifname ends in digits.digits, then unit and subuint are set
 *		to the first and second values respectively. If ifname ends
 *		in just digits, unit is set to the value, and subunit is set
 *		to -1. On error both unit and subunit are -1. NULL may be passed
 *		for unit and/or subuint to ignore the value.
 */
extern int get_ifname_unit(const char* ifname, int *unit, int *subunit);

/*
 * Set the ip configuration index given the eth name
 * Updates both wlXX_ipconfig_index and lanYY_ifname.
 *
 * @param	eth_ifname 	pointer to eth interface name
 * @return	0 if successful -1 if not.
 */
extern int set_ipconfig_index(char *eth_ifname, int index);

/*
 * Get the ip configuration index if it exists given the
 * eth name.
 *
 * @param	wl_ifname 	pointer to eth interface name
 * @return	index or -1 if not found
 */
extern int get_ipconfig_index(char *eth_ifname);

/*
 * Get interfaces belonging to a specific bridge.
 *
 * @param	bridge_name 	pointer to bridge interface name
 * @return	list on interfaces beloging to the bridge
 */
extern char *
get_bridged_interfaces(char *bridge_name);

/*
		remove_from_list
		Remove the specified word from the list.

		@param name word to be removed from the list
		@param list List to modify
		@param listsize Max size the list can occupy

		@return	error code
*/
extern int remove_from_list(const char *name, char *list, int listsize);

/*
		add_to_list
		Add the specified interface(string) to the list as long as
		it will fit in the space left in the list.

		@param name Name of interface to be added to the list
		@param list List to modify
		@param listsize Max size the list can occupy

		@return	error code
*/
extern int add_to_list(const char *name, char *list, int listsize);

extern char *find_in_list(const char *haystack, const char *needle);

extern char *remove_dups(char *inlist, int inlist_size);

extern char *find_smallest_in_list(char *haystack);

extern char *sort_list(char *inlist, int inlist_size);

extern int nvifname_to_osifname(const char *nvifname, char *osifname_buf,
                                int osifname_buf_len);
extern int osifname_to_nvifname(const char *osifname, char *nvifname_buf,
                                int nvifname_buf_len);

int ure_any_enabled(void);

extern int _vstrsep(char *buf, const char *sep, ...);
#define vstrsep(buf, sep, args...) _vstrsep(buf, sep, args, NULL)

#endif /* _shutils_h_ */
