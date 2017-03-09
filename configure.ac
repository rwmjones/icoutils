#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
AC_PREREQ(2.59)
AC_INIT(icoutils, 0.31.2, frank.richter@gmail.com)
AC_CONFIG_MACRO_DIR([m4]) # doesn't seem to have any effect at the moment
AC_CONFIG_SRCDIR([icotool/icotool.h])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE

# Stuff for the config.h.in file
AC_GNU_SOURCE
#AH_TEMPLATE([PKGUSERDIR], [User directory for program files])

# Checks for programs.
AC_PROG_CC
gl_EARLY
AC_PROG_LN_S
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
# AC_PROG_LIBTOOL

# Gnulib
gl_INIT

# --enable-werror option for developers.
AC_ARG_ENABLE([werror],
    [AS_HELP_STRING([--enable-werror],
                    [turn GCC warnings into errors (for developers)])],
    [case $enableval in
     yes|no) ;;
     *)      AC_MSG_ERROR([bad value $enableval for werror option]) ;;
     esac
     gcc_werror=$enableval],
    [gcc_werror=no]
)
if test "$gcc_werror" = "yes"; then
    gl_WARN_ADD([-Werror], [WERROR_CFLAGS])
    AC_SUBST([WERROR_CFLAGS])
fi

# $nw is the list of warnings we want to *disable*.  The nw warnings
# are complemented below.
nw=
nw="$nw -Wvla"                    # Allow variable length arrays.
nw="$nw -Wsuggest-attribute=pure" # Don't suggest function attributes.
nw="$nw -Wsuggest-attribute=const"
nw="$nw -Wsuggest-attribute=format"
nw="$nw -Wstrict-prototypes"      # Don't require prototypes for every function.
nw="$nw -Wmissing-prototypes"
nw="$nw -Wmissing-declarations"
nw="$nw -Wpacked"                 # Don't warn about packed structs.
nw="$nw -Wswitch-default"         # This warning is actively dangerous.
nw="$nw -Wunused-macros"          # Don't warn about defined but unused macros.

gl_MANYWARN_ALL_GCC([ws])
gl_MANYWARN_COMPLEMENT([ws], [$ws], $[nw])
for w in $ws; do
    gl_WARN_ADD([$w])
done

# Normally we disable warnings in $nw above.  However $nw only
# filters out exact matching warning strings from a list inside
# gnulib (see m4/manywarnings.m4).  So we need to explicitly list a
# few disabled warnings below.

# Allow unused function parameters.
gl_WARN_ADD([-Wno-unused-parameter])

AC_SUBST([WARN_CFLAGS])

# Gettext
AM_GNU_GETTEXT([external])
# AM_GNU_GETTEXT_VERSION(0.14.1)

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
#AC_CHECK_HEADERS([byteswap.h errno.h fcntl.h getopt.h limits.h sys/stat.h sys/time.h sys/types.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_C_CONST
AC_C_BIGENDIAN
#AC_TYPE_OFF_T
#AC_TYPE_SIZE_T
#AC_TYPE_MODE_T
AC_CHECK_TYPES([comparison_fn_t])

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([pow])

# Check for libpng
AC_CHECK_LIB(png, png_create_read_struct, [
AC_SUBST(PNG_LIBS, "-lpng -lz -lm")
], [
  AC_CHECK_LIB(png10, png_create_read_struct, [
  AC_SUBST(PNG_LIBS, "-lpng10 -lz -lm")
  ], [
    AC_CHECK_LIB(png12, png_create_read_struct, [
    AC_SUBST(PNG_LIBS, "-lpng12 -lz -lm")
    ], [
      AC_MSG_ERROR([cannot find required library png])
    ], [-lz -lm])
  ], [-lz -lm])
], [-lz -lm])
AC_CHECK_HEADERS([png.h libpng/png.h libpng10/png.h libpng12/png.h locale.h])

AC_CONFIG_FILES([Makefile
		 icoutils.spec
		 po/Makefile.in
		 lib/Makefile
		 common/Makefile
		 icotool/Makefile
		 wrestool/Makefile
		 extresso/Makefile])
AC_CONFIG_FILES([extresso/extresso], [chmod +x extresso/extresso])
AC_CONFIG_FILES([extresso/genresscript], [chmod +x extresso/genresscript])
AC_OUTPUT
