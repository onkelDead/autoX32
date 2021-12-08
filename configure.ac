AC_INIT([autoX32], [0.4], [onkel@paraair.de])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])
AC_PREREQ([2.66])
${CXXFLAGS=""}
${CFLAGS=""}
AC_PROG_CXX

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])

AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

PKG_CHECK_MODULES([GTKMM3], [gtkmm-3.0])
PKG_CHECK_MODULES([XML2], [libxml++-2.6])
PKG_CHECK_MODULES([LO], [liblo])
PKG_CHECK_MODULES([JACK], [jack])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.10])

GLIB_GSETTINGS

AC_CHECK_HEADER_STDBOOL

AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([strdup])
AC_CHECK_FUNCS([strstr])
AC_CHECK_FUNCS([lo_message_serialise])
AC_TYPE_INT32_T
AC_TYPE_SIZE_T

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile])

AC_OUTPUT

