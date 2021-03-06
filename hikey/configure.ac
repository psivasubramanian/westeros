#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

dnl AC_PREREQ([2.65])
AC_INIT(westeros-soc, version-1.0, sivasubramanian.patchaiperumal@linaro.com,westeros-soc)
AC_CONFIG_SRCDIR([])
AC_CONFIG_HEADERS([cfg/config.h])
AC_CONFIG_MACRO_DIR([cfg])
AC_CONFIG_AUX_DIR([cfg])
AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-bzip2 1.9])
LT_INIT

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_CXX

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

AC_ARG_ENABLE(egl, [  --disable-egl],,
              enable_egl=yes)
AM_CONDITIONAL(ENABLE_EGL, test x$enable_egl = xyes)
if test x$enable_egl = xyes; then
        AC_DEFINE([ENABLE_EGL], [1], [Build Westeros with EGL support])
        PKG_CHECK_MODULES(EGL, [egl >= 7.10 glesv2])
        PKG_CHECK_MODULES([EGL_TESTS], [egl >= 7.10 glesv2 wayland-client wayland-egl])
        PKG_CHECK_MODULES([DRM], [libdrm])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

IARM_CFLAGS=" "

# Checks for library functions.
#Add the subdirectories to be considered for building.
SUBDIRS="westeros-gl"
#to setup m4 macros
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
[AM_DEFAULT_VERBOSITY=1
AC_SUBST(AM_DEFAULT_VERBOSITY)])

PKG_CHECK_MODULES([GLIB],[glib-2.0 >= 2.22.5])
PKG_CHECK_MODULES([GTHREAD],[gthread-2.0 >= 2.38.2])
PKG_CHECK_MODULES([WAYLAND_CLIENT],[wayland-client >= 1.6.0])
PKG_CHECK_MODULES([WAYLAND_SERVER],[wayland-server >= 1.6.0])

WAYLANDLIB="-lwayland-client"
AC_SUBST(WAYLANDLIB)

AC_CONFIG_FILES([Makefile])
AC_SUBST(WAYLAND_CFLAGS)
AC_SUBST(GUPNP_VERSION)
AC_SUBST(plugindir)
AC_OUTPUT
