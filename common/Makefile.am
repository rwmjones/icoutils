noinst_LTLIBRARIES = libcommon.la

libcommon_la_SOURCES = \
	alist.c \
	alist.h \
	byteswap.h \
	collections.h \
	common.h \
	error.c \
	error.h \
	hmap.c \
	hmap.h \
	io-utils.c \
	io-utils.h \
	intparse.c \
	intparse.h \
	iterator.c \
	iterator.h \
	llist.c \
	llist.h \
	memory.h \
	regex-utils.c \
	regex-utils.h \
	strbuf.c \
	strbuf.h \
	string-utils.c \
	string-utils.h \
	tab-utils.c \
	tab-utils.h \
	time64.c \
	time64.h \
	tmap.c \
	tmap.h

libcommon_la_LIBADD = \
	../lib/libgnu.la

AM_CPPFLAGS = \
	-I$(top_builddir)/lib \
	-I$(top_srcdir)/lib

AM_CFLAGS = -Wall
