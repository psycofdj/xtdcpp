dnl ---------------------------------------------------------------------------
dnl TDB
dnl ---------------------------------------------------------------------------
AC_DEFUN([SET_VAR_DEFAULT],
[
    if test -z "$]$1[" -o "$]$1[" = "$]$2["; then
       ]$1[="$]$2[ ]$3["
    else
       ]$1[="$]$1[ ]$3["
    fi
])

AC_DEFUN([XTD_INIT],
[
	dnl gestion du xtd dir

  SRC_DIR="$(dirname $(readlink -m ${0}))"

  if test ! -z "$1"; then
    XTD_REL_DIR="$1"
  else
    XTD_REL_DIR="."
  fi

  XTD_DIR="$(readlink -m ${SRC_DIR}/${XTD_REL_DIR})"
  XTDMAKE_DIR="${XTD_DIR}/xtdmake"
  XTD_INCLUDE="${XTD_DIR}"
  XTD_LIB="\$(top_builddir)/${XTD_REL_DIR}"
  AC_SUBST(XTD_DIR)
  AC_SUBST(XTDMAKE_DIR)
  AC_SUBST(XTD_INCLUDE)
  AC_SUBST(XTD_LIB)

	dnl gestion du svc dir
  DEFAULT_XTD_SVCROOT_DIR="$(dirname $(readlink -m ${0}))"
  if test ! -z "$2"; then
    DEFAULT_XTD_SVCROOT_DIR="$(readlink -m ${DEFAULT_XTD_SVCROOT_DIR}/$2)"
  fi
  AC_ARG_WITH(
		[svc-dir],
    [AS_HELP_STRING([--with-svc-dir=DIR], [source control directory]) ],
    [XTD_SVCROOT_DIR="$withval"],
    [XTD_SVCROOT_DIR="${DEFAULT_XTD_SVCROOT_DIR}"]
  )
  XTD_SVCROOT_DIR=$(readlink -m ${XTD_SVCROOT_DIR})
  ac_configure_args="${ac_configure_args} --with-svc-dir=${XTD_SVCROOT_DIR}"
  AC_SUBST(XTD_SVCROOT_DIR)
])

dnl ---------------------------------------------------------------------------
dnl Cette macro ajoute les parametres :
dnl   --enable-debug (default no)
dnl   --enable-coverage (default no)
dnl   --enable-etch (default no)
dnl
dnl On definit les conditionnelles automake :
dnl - IS_BUILD_DEBUG
dnl - IS_BUILD_COVERAGE
dnl - IS_BUILD_ETCH
dnl ---------------------------------------------------------------------------
AC_DEFUN([ASK_BUILD_TYPE],
[
	dnl Declare le parametre --enable-debug
  AC_ARG_ENABLE(debug,
    [AS_HELP_STRING([--enable-debug ], [enable debug build, default no])],
    [AS_IF([test -z "$enableval"], [DEBUG_BUILD="yes"], [DEBUG_BUILD=$enableval])],
    [DEBUG_BUILD="no"]
  )
	dnl Verifie que la valeur passee en parametre est valide
  AC_MSG_CHECKING(debug option sanity)
  AS_IF([ test "$DEBUG_BUILD" != "yes" && test "$DEBUG_BUILD" != "no" ],
    AC_MSG_FAILURE([ --enable-debug=$DEBUG_BUILD is not valid. Use "yes" or "no" ]),
    AC_MSG_RESULT([ ok ])
  )

	dnl Declare le parametre --enable-coverage
  AC_ARG_ENABLE(coverage,
    [AS_HELP_STRING([--enable-coverage ], [enable code coverage tracking, default no])],
    [AS_IF([test -z "$enableval"], [COVERAGE_BUILD="yes"], [COVERAGE_BUILD=$enableval])],
    [COVERAGE_BUILD="no"]
  )
	dnl Verifie que la valeur passee en parametre est "release" ou "debug"
  AC_MSG_CHECKING(coverage options sanity)
  AS_IF([ test "$COVERAGE_BUILD" != "yes" && test "$COVERAGE_BUILD" != "no" ],
    AC_MSG_FAILURE([ --enable-coverage=$COVERAGE_BUILD is not valid. Use "yes" or "no" ]),
    AC_MSG_RESULT([ ok ])
  )

	dnl Declare le parametre --enable-etch
  AC_ARG_ENABLE(efence,
    [AS_HELP_STRING([--enable-efence ], [link with Electric Fence, default no])],
    [AS_IF([test -z "$enableval"], [EFENCE_BUILD="yes"], [EFENCE_BUILD=$enableval])],
    [EFENCE_BUILD="no"]
  )
	dnl Verifie que la valeur passee en parametre est "release" ou "debug"
  AC_MSG_CHECKING(efence options sanity)
  AS_IF([ test "$EFENCE_BUILD" != "yes" && test "$EFENCE_BUILD" != "no" ],
    AC_MSG_FAILURE([ --enable-efence=$EFENCE_BUILD is not valid. Use "yes" or "no" ]),
    AC_MSG_RESULT([ ok ])
  )

  dnl declaration des compilateurs
  CC="gcc"
  CXX="g++"


  BOOST_INCLUDE="/usr/include/"
  BOOST_LIB="/usr/lib/x86_64-linux-gnu/"

  AC_SUBST(BOOST_INCLUDE, ${BOOST_INCLUDE})
  AC_SUBST(BOOST_CHRONO, boost_chrono)
  AC_SUBST(BOOST_CONTEXT, boost_context)
  AC_SUBST(BOOST_DATE_TIME, boost_date_time)
  AC_SUBST(BOOST_EXCEPTION, boost_exception)
  AC_SUBST(BOOST_FILESYSTEM, boost_filesystem)
  AC_SUBST(BOOST_GRAPH, boost_graph)
  AC_SUBST(BOOST_IOSTREAMS, boost_iostreams)
  AC_SUBST(BOOST_LOCALE, boost_locale)
  AC_SUBST(BOOST_MATH_C99, boost_math_c99)
  AC_SUBST(BOOST_MATH_C99F, boost_math_c99f)
  AC_SUBST(BOOST_MATH_C99L, boost_math_c99l)
  AC_SUBST(BOOST_MATH_TR1, boost_math_tr1)
  AC_SUBST(BOOST_MATH_TR1F, boost_math_tr1f)
  AC_SUBST(BOOST_MATH_TR1L, boost_math_tr1l)
  AC_SUBST(BOOST_PRG_EXEC_MONITOR, boost_prg_exec_monitor)
  AC_SUBST(BOOST_PROGRAM_OPTIONS, boost_program_options)
  AC_SUBST(BOOST_RANDOM, boost_random)
  AC_SUBST(BOOST_REGEX, boost_regex)
  AC_SUBST(BOOST_SERIALIZATION, boost_serialization)
  AC_SUBST(BOOST_SIGNALS, boost_signals)
  AC_SUBST(BOOST_SYSTEM, boost_system)
  AC_SUBST(BOOST_TEST_EXEC_MONITOR, boost_test_exec_monitor)
  AC_SUBST(BOOST_THREAD, boost_thread)
  AC_SUBST(BOOST_TIMER, boost_timer)
  AC_SUBST(BOOST_UNIT_TEST_FRAMEWORK, boost_unit_test_framework)
  AC_SUBST(BOOST_WAVE, boost_wave)
  AC_SUBST(BOOST_WSERIALIZATION, boost_wserialization)

  dnl On declare les constantes pour les flags par defaut
  COMMON_CFLAGS="${CFLAGS} -W -Wall -march=k8 -g -pipe"
  COMMON_CXXFLAGS="${CXXFLAGS} -std=c++11 -W -Wall -march=k8 -g -pipe -DBOOST_ASIO_DISABLE_EPOLL"
  COMMON_LDFLAGS="${LDFLAGS}"
  COMMON_LDPATH="-L/usr/lib"
  DEBUG_CFLAGS="-O0"
  DEBUG_CXXFLAGS="-O0"
  RELEASE_CFLAGS="-O2"
  RELEASE_CXXFLAGS="-O2"
  COVERAGE_CFLAGS="-fprofile-arcs -ftest-coverage"
  COVERAGE_CXXFLAGS="-fprofile-arcs -ftest-coverage"
  COVERAGE_LIBS="-lgcov"

  dnl nettoyage de l'environement
  CFLAGS=""
  CXXFLAGS=""
  LDFLAGS="${COMMON_LDPATH}"

  dnl Adaptation des flags de compilation en cas de "--enable-efence"
  AS_IF([test "${EFENCE_BUILD}" = "yes"],
  [
    AC_CHECK_LIB([pthread],
                 [pthread_mutex_trylock],
             		 [LIBS="-lpthread ${LIBS}"],
                 [AC_MSG_FAILURE([PThread is not found (necessary for Electric Fence)])])

    AC_CHECK_LIB([efence],
                 [malloc],
             		 [LIBS="-lefence ${LIBS}"],
                 [AC_MSG_FAILURE([Electric Fence library not available (could not find -lefence)])])
  ])


	dnl Adaptation des flags de compilation par default en cas de "--enable-debug"
  AS_IF([test "${DEBUG_BUILD}" = "yes"],
  [
    CFLAGS="${COMMON_CFLAGS} ${DEBUG_CFLAGS}"
    CXXFLAGS="${COMMON_CXXFLAGS} ${DEBUG_CXXFLAGS}"
    LDFLAGS="${COMMON_LDFLAGS} ${COMMON_LDPATH} ${DEBUG_LDFLAGS}"
  ],
  [
    CFLAGS="${COMMON_CFLAGS} ${RELEASE_CFLAGS}"
    CXXFLAGS="${COMMON_CXXFLAGS} ${RELEASE_CXXFLAGS}"
    LDFLAGS="${COMMON_LDFLAGS} ${COMMON_LDPATH} ${RELEASE_LDFLAGS}"
  ])

  dnl Adaptation des flags de compilation en cas de "--enable-coverage"
  AS_IF([test "${COVERAGE_BUILD}" = "yes"],
  [
    CFLAGS="${CFLAGS} ${COVERAGE_CFLAGS}"
    CXXFLAGS="${CXXFLAGS} ${COVERAGE_CXXFLAGS}"
    LIBS="${COVERAGE_LIBS} ${LIBS}"
  ])

	dnl	Declare les conditionnelles automake
  AM_CONDITIONAL(IS_BUILD_DEBUG,    test $DEBUG_BUILD = "yes")
  AM_CONDITIONAL(IS_BUILD_COVERAGE, test $COVERAGE_BUILD = "yes")
])



dnl ---------------------------------------------------------------------------
dnl ---------------------------------------------------------------------------
AC_DEFUN([ASK_THREAD],
[
  AC_ARG_ENABLE(threads,
    [AS_HELP_STRING([--enabled-threads=BOOL], [enable thread yes/no])],
    [
      if test -z "$enable_threads" ; then
        enable_threads=yes
      else
        enable_threads=$enableval
      fi
    ]
  )

  AC_MSG_CHECKING(whether threads must be used)
  AS_IF([test "$enable_threads" = "no"],
          [ AC_DEFINE(DISABLE_THREADS, [], [Disable threads])
            USE_THREADS=N
            AC_MSG_RESULT(no) ],
          [ USE_THREADS=Y
            THREAD_CFLAGS="-DREENTRANT=1"
            THREAD_CXXFLAGS="-DREENTRANT=1"
            THREAD_LDFLAGS="-lpthread"
            AC_MSG_RESULT(yes) ])

  AM_CONDITIONAL(THREAD, test $USE_THREADS = Y)
  AC_SUBST(USE_THREADS)
  AC_SUBST(THREAD_CFLAGS)
  AC_SUBST(THREAD_CXXFLAGS)
  AC_SUBST(THREAD_LDFLAGS)
])


dnl ---------------------------------------------------------------------------
dnl ---------------------------------------------------------------------------
AC_DEFUN([ASK_STATIC],
[
  AC_ARG_ENABLE(static,
    [AS_HELP_STRING([--enabled-static=BOOL], [enable static build yes/no])],
    [ if test -z "$enable_static" ; then
        enable_static=yes
      else
        enable_static=$enableval
      fi
    ])
  AC_MSG_CHECKING(whether build should be static)
  AS_IF([test "$enable_static" = "yes"],
        [ USE_SHARED=N
          STATIC_LDFLAGS="-all-static"
          AC_MSG_RESULT(yes) ],
        [ USE_SHARED=Y
          AC_MSG_RESULT(no) ])
  AC_SUBST(USE_SHARED)
  AC_SUBST(STATIC_LDFLAGS)
  AM_CONDITIONAL(SHARED, test $USE_SHARED = Y)
])


dnl ---------------------------------------------------------------------------
dnl Macro utilitaire qui verifie l'existance des utilitaires standards utilises
dnl dans les composants KE. Elle definit les variables automake :
dnl - $(CC)
dnl - $(RENAME)
dnl - $(STRIPDEBUG)
dnl - $(MKDIR)
dnl - $(AR)
dnl - $(AS)
dnl - $(LD)
dnl - $(OBJDUMP)
dnl - $(RANLIB)
dnl ---------------------------------------------------------------------------
AC_DEFUN([SET_PROGS],
[
  AC_PROG_CC(gcc)
  AC_PROG_CXX(g++)
  AC_PROG_LEX(flex)
  AC_PROG_MAKE_SET
  AC_PATH_PROG(PKG_CONFIG)
  AC_CHECK_PROG(RENAME,mv,mv)

  MKDIR="mkdir -p"
  AR="/usr/bin/ar"
  AS="/usr/bin/as"
  LD="/usr/bin/ld"
  NM="/usr/bin/nm"
  OBJDUMP="/usr/bin/objdump"
  RANLIB="/usr/bin/ranlib"
  STRIPDEBUG="/usr/bin/stripdebug"
  SHELL="/bin/bash"
  enable_silent_rules="yes"

  AC_MSG_WARN([-= Programs =-])
  AC_MSG_WARN([      \$(MKDIR) : ${MKDIR}])
  AC_MSG_WARN([         \$(AR) : ${AR}])
  AC_MSG_WARN([         \$(AS) : ${AS}])
  AC_MSG_WARN([         \$(LD) : ${LD}])
  AC_MSG_WARN([         \$(NM) : ${NM}])
  AC_MSG_WARN([    \$(OBJDUMP) : ${OBJDUMP}])
  AC_MSG_WARN([     \$(RANLIB) : ${RANLIB}])
  AC_MSG_WARN([ \$(STRIPDEBUG) : ${STRIPDEBUG}])
  AC_MSG_WARN([      \$(SHELL) : ${SHELL}])
  AC_MSG_WARN()
  AC_MSG_WARN([-= LIBS =-])
  AC_MSG_WARN([\$(BOOST_INCLUDE) : ${BOOST_INCLUDE}])
  AC_MSG_WARN([    \$(BOOST_LIB) : ${BOOST_LIB}])
  AC_MSG_WARN()
  AC_MSG_WARN([-= XTD =-])
  AC_MSG_WARN([    \$(XTD_DIR)     : ${XTD_DIR}])
  AC_MSG_WARN([\$(XTDMAKE_DIR)     : ${XTDMAKE_DIR}])
  AC_MSG_WARN([\$(XTD_INCLUDE)     : ${XTD_INCLUDE}])
  AC_MSG_WARN([    \$(XTD_LIB)     : ${XTD_LIB}])
  AC_MSG_WARN([\$(XTD_SVCROOT_DIR) : ${XTD_SVCROOT_DIR}])
  AC_MSG_WARN()

  AC_SUBST(SHELL)
  AC_SUBST(MKDIR)
  AC_SUBST(AR)
  AC_SUBST(AS)
  AC_SUBST(LD)
  AC_SUBST(NM)
  AC_SUBST(OBJDUMP)
  AC_SUBST(RANLIB)
	AC_SUBST(STRIPDEBUG)


])



dnl ---------------------------------------------------------------------------
dnl Macro initialisant le framework de developpement standard de KE
dnl ---------------------------------------------------------------------------
AC_DEFUN([XTD_PACKAGE],
[
  m4_define([MY_LIBNAME], m4_toupper([$1])[_LIB])
  m4_define([MY_INCNAME], m4_toupper([$1])[_INCLUDE])
  AC_SUBST(MY_INCNAME($1), ["\$(top_srcdir)"])
  AC_SUBST(MY_LIBNAME($1), ["\$(top_builddir)"])
  AC_SUBST(PKG_NAME, [$1])

  XTD_INIT([$2], [$3])
  ASK_BUILD_TYPE()
  ASK_THREAD()
  ASK_STATIC()
  SET_PROGS()

  dnl On imprime un resume de la configuration
  AC_MSG_WARN([-= Configuration =-])
  AC_MSG_WARN([    debug mode  : $DEBUG_BUILD])
  AC_MSG_WARN([ coverage mode  : $COVERAGE_BUILD])
  AC_MSG_WARN([electric fence  : $EFENCE_BUILD])
  AC_MSG_WARN()

  AC_MSG_WARN([-= Compilers =-])
  AC_MSG_WARN([  \$(CC) : $CC])
  AC_MSG_WARN([ \$(CXX) : $CXX])
  AC_MSG_WARN()

  AC_MSG_WARN([-= Flags =-])
  AC_MSG_WARN([   \$(CFLAGS) : $CFLAGS])
  AC_MSG_WARN([ \$(CXXFLAGS) : $CXXFLAGS])
  AC_MSG_WARN([  \$(LDFLAGS) : $LDFLAGS])
  AC_MSG_WARN([     \$(LIBS) : $LIBS])
  AC_MSG_WARN()
])
