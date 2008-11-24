/* Copyright 1998 by the Massachusetts Institute of Technology.
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 * M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 */

#ifndef RARES__H
#define RARES__H

#ifdef WIN32
	#include <winsock2.h>
	#include <stdlib.h>
	#include <errno.h>
	#include <io.h>
	#include <string.h>
	#include <stdio.h>
	#include <WS2tcpip.h>
#endif


#if defined(__cplusplus)
extern "C" {
#endif

#if defined(__APPLE__)
#include <arpa/nameser.h>
#if !defined(MAC_OS_X_VERSION_MIN_REQUIRED) || (MAC_OS_X_VERSION_MIN_REQUIRED > MAC_OS_X_VERSION_10_2)
#include <arpa/nameser_compat.h>
#endif
#endif

#include <sys/types.h>
#ifndef WIN32
/* why was this commented out?! ah, it was a 'fix for windows' */
#include <netinet/in.h>
#endif

#include "ares_socketfunc.h"

#define RARES_SUCCESS		0

/* Server error codes (RARES_ENODATA indicates no relevant answer) */
#define RARES_ENODATA		1
#define RARES_EFORMERR		2
#define RARES_ESERVFAIL		3
#define RARES_ENOTFOUND		4
#define RARES_ENOTIMP		5
#define RARES_EREFUSED		6

/* Locally generated error codes */
#define RARES_EBADQUERY		7
#define RARES_EBADNAME		8
#define RARES_EBADFAMILY		9
#define RARES_EBADRESP		10
#define RARES_ECONNREFUSED	11
#define RARES_ETIMEOUT		12
#define RARES_EOF		13
#define RARES_EFILE		14
#define RARES_ENOMEM		15
#define RARES_EDESTRUCTION	16

/* Flag values */
#define RARES_FLAG_USEVC		(1 << 0)
#define RARES_FLAG_PRIMARY	(1 << 1)
#define RARES_FLAG_IGNTC		(1 << 2)
#define RARES_FLAG_NORECURSE	(1 << 3)
#define RARES_FLAG_STAYOPEN	(1 << 4)
#define RARES_FLAG_NOSEARCH	(1 << 5)
#define RARES_FLAG_NOALIASES	(1 << 6)
#define RARES_FLAG_NOCHECKRESP	(1 << 7)
#define RARES_FLAG_TRY_NEXT_SERVER_ON_RCODE3 (1 << 8) /* when rcode of 3 ('No such name') is recvd */

/* Option mask values */
#define RARES_OPT_FLAGS		(1 << 0)
#define RARES_OPT_TIMEOUT	(1 << 1)
#define RARES_OPT_TRIES		(1 << 2)
#define RARES_OPT_NDOTS		(1 << 3)
#define RARES_OPT_UDP_PORT	(1 << 4)
#define RARES_OPT_TCP_PORT	(1 << 5)
#define RARES_OPT_SERVERS	(1 << 6)
#define RARES_OPT_DOMAINS	(1 << 7)
#define RARES_OPT_LOOKUPS	(1 << 8)

/* Capability mask values */

#define RARES_CAP_IPV6 		(1 << 0)

#if defined(WIN32) || defined(sun)
typedef unsigned char u_int8_t;
#endif

#ifdef USE_IPV6
struct multiFamilyAddr {
  u_int8_t family;
  struct in6_addr addr6;
  struct in_addr addr;
};
#endif

struct ares_options {
  int flags;
  int timeout;
  int tries;
  int ndots;
  unsigned short udp_port;
  unsigned short tcp_port;
#ifdef USE_IPV6
  struct multiFamilyAddr *servers;
#else
  struct in_addr *servers;
#endif
  int nservers;
  char **domains;
  int ndomains;
  char *lookups;
};

struct hostent;
struct timeval;
struct ares_channeldata;
typedef struct ares_channeldata *ares_channel;
typedef void (*ares_callback)(void *arg, int status, unsigned char *abuf,
			      int alen);
typedef void (*ares_host_callback)(void *arg, int status,
				   struct hostent *hostent);

extern int rares_init(ares_channel *channelptr);
extern int rares_init_with_socket_function(ares_channel *channelptr, socket_function_ptr);

extern 	int rares_capabilities(int capmask);

extern 	int rares_init_options(ares_channel *channelptr, struct ares_options *options,
                              int optmask);

extern 	int rares_init_options_with_socket_function(ares_channel *channelptr, struct ares_options *options,
                                                   int optmask, socket_function_ptr);

extern 	void rares_destroy(ares_channel channel);
extern 	void rares_destroy_suppress_callbacks(ares_channel channel);

extern 	void rares_destroy_internal(ares_channel channel, int suppressCallbacks);

extern 	void rares_send(ares_channel channel, const unsigned char *qbuf, int qlen,
			ares_callback callback, void *arg);
extern 	void rares_query(ares_channel channel, const char *name, int dnsclass,
			int type, ares_callback callback, void *arg);
extern 	void rares_search(ares_channel channel, const char *name, int dnsclass,
			int type, ares_callback callback, void *arg);
extern 	void rares_gethostbyname(ares_channel channel, const char *name, int family,
				ares_host_callback callback, void *arg);
extern 	void rares_gethostbyaddr(ares_channel channel, const void *addr, int addrlen,
				int family, ares_host_callback callback, void *arg);
extern   int rares_hostfile_lookup(const char *name, struct hostent **host);

extern 	int rares_fds(ares_channel channel, fd_set *read_fds, fd_set *write_fds);
extern 	struct timeval *rares_timeout(ares_channel channel, struct timeval *maxtv,
					struct timeval *tv);
extern 	void rares_process(ares_channel channel, fd_set *read_fds, fd_set *write_fds);

extern 	int rares_mkquery(const char *name, int dnsclass, int type, unsigned short id,
			int rd, unsigned char **buf, int *buflen);
extern 	int rares_expand_name(const unsigned char *encoded, const unsigned char *abuf,
				int alen, char **s, int *enclen);
extern 	int rares_parse_a_reply(const unsigned char *abuf, int alen,
				struct hostent **host);
extern 	int rares_parse_ptr_reply(const unsigned char *abuf, int alen, const void *addr,
				int addrlen, int family, struct hostent **host);
extern 	void rares_free_string(char *str);
extern 	void rares_free_hostent(struct hostent *host);
extern 	const char *rares_strerror(int code);
extern 	void rares_free_errmem(char *mem);


#if defined(WIN32) || defined (__CYGWIN__)

#define T_A             1               /* host address */
#define T_NS            2               /* authoritative server */
#define T_MD            3               /* mail destination */
#define T_MF            4               /* mail forwarder */
#define T_CNAME         5               /* canonical name */
#define T_SOA           6               /* start of authority zone */
#define T_MB            7               /* mailbox domain name */
#define T_MG            8               /* mail group member */
#define T_MR            9               /* mail rename name */
#define T_NULL          10              /* null resource record */
#define T_WKS           11              /* well known service */
#define T_PTR           12              /* domain name pointer */
#define T_HINFO         13              /* host information */
#define T_MINFO         14              /* mailbox information */
#define T_MX            15              /* mail routing information */
#define T_TXT           16              /* text strings */
#define T_RP            17              /* responsible person */
#define T_AFSDB         18              /* AFS cell database */
#define T_X25           19              /* X_25 calling address */
#define T_ISDN          20              /* ISDN calling address */
#define T_RT            21              /* router */
#define T_NSAP          22              /* NSAP address */
#define T_NSAP_PTR      23              /* reverse NSAP lookup (deprecated) */
#define T_SIG           24              /* security signature */
#define T_KEY           25              /* security key */
#define T_PX            26              /* X.400 mail mapping */
#define T_GPOS          27              /* geographical position (withdrawn) */
#define T_AAAA          28              /* IP6 Address */
#define T_LOC           29              /* Location Information */
        /* non standard */
#define T_UINFO         100             /* user (finger) information */
#define T_UID           101             /* user ID */
#define T_GID           102             /* group ID */
#define T_UNSPEC        103             /* Unspecified format (binary data) */
        /* Query type values which do not appear in resource records */
#define T_AXFR          252             /* transfer zone of authority */
#define T_MAILB         253             /* transfer mailbox records */
#define T_MAILA         254             /* transfer mail agent records */
#define T_ANY           255             /* wildcard match */


#define C_IN 1
#define C_CHAOS 3
#define C_HS 4
#define C_ANY 255

#define INDIR_MASK 0xc0
#define HFIXEDSZ 12
#define QFIXEDSZ 4
#define RRFIXEDSZ 10

#define NOERROR 0
#define FORMERR 1
#define SERVFAIL 2
#define NXDOMAIN 3
#define NOTIMP 4
#define REFUSED 5

#define PACKETSZ 512 

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#define NS_DEFAULTPORT 53 
#define NAMESERVER_PORT NS_DEFAULTPORT 

#define QUERY 0
#define MAXLABEL 63

#endif

#if defined(__cplusplus)
}
#endif


#endif /* RARES__H */