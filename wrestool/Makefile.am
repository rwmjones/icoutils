bin_PROGRAMS = wrestool

wrestool_SOURCES = \
  extract.c \
  main.c \
  restable.c \
  wrestool.h \
  fileread.c \
  fileread.h

wrestool_LDADD = \
  ../common/libcommon.la \
  ../lib/libgnu.la

man_MANS = \
  wrestool.1

EXTRA_DIST = \
  $(man_MANS)

AM_CPPFLAGS = \
  -I$(top_builddir)/lib \
  -I$(top_srcdir)/lib \
  -I$(top_srcdir) \
  -I$(srcdir)/../icotool

AM_CFLAGS = -Wall
