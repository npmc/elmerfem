AC_DEFUN([ACX_FTGL], [
AC_REQUIRE([AC_PATH_X])
AC_PREREQ(2.50)
#
# For MinGW, test for -liconv:
#
acx_liconv_ok=yes
if test "$acx_platform_def" = "WIN32"; then
   acx_liconv_ok=no
   #
   # look for "libiconv_open" instead of "iconv_open":
   #
   AC_CHECK_LIB( [iconv], [libiconv_open], 
                 [acx_liconv_ok=yes; ICONV_LIBS="-liconv"],
                 [ICONV_LIBS=""] )

   if test "$acx_liconv_ok" = no; then
     AC_MSG_RESULT([
   ***************************************
   ***   WARNING: -liconv not found    ***
   ***   FreeType2 rendering disabled  ***
   ***           Try adding:           ***
   ***  -L/path/to -liconv in LDFLAGS  ***
   ***         and reconfigure         ***
   ***************************************
     ])
   fi
fi
#
# Look for iconv.h:
#
acx_iconv_h=no
AC_CHECK_HEADER( [iconv.h], [acx_iconv_h_ok=yes] )

if test "$acx_iconv_h_ok" = no; then
   AC_MSG_RESULT([
   ***************************************
   ***   WARNING: iconv.h not found    ***
   ***  FreeType2 rendering disabled   ***
   ***          Try adding:            ***
   ***  -I/path/to iconv.h in CFLAGS   ***
   ***         and reconfigure         ***
   ***************************************
  ])
fi
#
# Look for freetype-config:
#
acx_ft_ok=no

acx_ft_locs="$FTCONFIG /usr/local/bin /usr/bin"

if test "$acx_platform_def" = "WIN32"; then
   acx_ft_locs="$acx_ft_locs /mingw/bin"
fi

for v in $acx_ft_locs; do
   acx_ft_config="$v/freetype-config"
   AC_MSG_CHECKING([for $acx_ft_config])
   acx_ft_ok=no;
   if test -e $acx_ft_config; then
      acx_ft_ok=yes;
      AC_MSG_RESULT($acx_ft_ok)
      break;
   fi
   AC_MSG_RESULT($acx_ft_ok)
done
#
# Set freetype2 libs and cflags:
#
if test "$acx_ft_ok" = yes; then
   FT_LIBS=`$acx_ft_config --libs`
   FT_CFLAGS=`$acx_ft_config --cflags`
   AC_MSG_RESULT([
   FT_LIBS:            $FT_LIBS
   FT_CFLAGS:          $FT_CFLAGS
   ])
else
   AC_MSG_RESULT([
   *********************************************
   ***  WARNING: freetype-config not found   ***
   ***     FreeType2 rendering disabled      ***
   ***        Try setting/exporting:         ***
   ***  FTCONFIG=/path/to/ freetype-config   ***
   ***          and reconfigure              ***
   *********************************************
   ])
fi
#
# Look for FTGL (FTGL does not provide any C-functions.
# Hence, we make a brutal hack and test with printf):
#
acx_lftgl_ok=no
AC_CHECK_LIB( [ftgl], [printf], 
              [acx_lftgl_ok=yes; FTGL_LIBS="-lftgl"], [FTGL_LIBS=""] )

if test "$acx_lftgl_ok" = no; then
  AC_MSG_RESULT([
   ***************************************
   ***    WARNING: -lftgl not found    ***
   ***   FreeType2 rendering disabled  ***
   ***           Try adding:           ***
   ***   -L/path/to -lftgl in LDFLAGS  ***
   ***         and reconfigure         ***
   ***************************************
  ])
fi

AC_LANG_PUSH(C++)

acx_ftgl_h_ok=no
AC_CHECK_HEADER( [FTGL.h], [acx_ftgl_h_ok=yes] )

if test "$acx_ftgl_h_ok" = no; then
   AC_MSG_RESULT([
   ***************************************
   ***    WARNING: FTGL.h not found    ***
   ***  FreeType2 rendering disabled   ***
   ***          Try adding:            ***
   ***  -I/path/to FTGL.h in CXXFLAGS  ***
   ***         and reconfigure         ***
   ***************************************
  ])
fi
#
# Final verdict:
#
acx_ftgl_ok=yes

if test "$acx_ft_ok" = no; then
   acx_ftgl_ok=no;
fi

if test "$acx_lftgl_ok" = no; then
   acx_ftgl_ok=no;
fi

if test "$acx_ftgl_h_ok" = no; then
   acx_ftgl_ok=no;
fi

if test "$acx_liconv_ok" = no; then
   acx_ftgl_ok=no;
fi

if test "$acx_iconv_h_ok" = no; then
   acx_ftgl_ok=no;
fi
#
# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
#
if test x"$acx_ftgl_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_FTGL,1,[Define if you have a FTGL library.]),[$1])
        :
else
        acx_ftgl_ok=no
	$1 # do not choke
        # $2
fi

AC_LANG_POP(C++)

])
