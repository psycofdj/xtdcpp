#!/bin/bash

LC_ALL=C
l_progName=$0
l_arguments=""
l_redColor="\e[1;31m"
l_greenColor="\e[1;32m"
l_resetColor="\e[0m"

function usage
{
    echo "usage : ${l_progName}"
    echo ""
		echo "This script initializes the autotools source tree environment"
		echo "  * creates Makefile.in from Makefile.am"
		echo "  * creates configure script from configure.ac"
		echo ""
    echo "[optional parameters]"
    echo "  --no-color          : Disable colored output"
		echo "  --help              : Display this message"
    exit 1;
}

function check_error
{
    status=$1
    if [ $status -ne 0 ]; then
        echo -e "${l_redColor}Ko${l_resetColor}"
				echo -e "${l_redColor}$(basename ${l_progName}) error log${l_resetColor} :" 1>&2
        cat .build/bootstrap.log 1>&2
        exit 1
    else
        echo -e "${l_greenColor}oK${l_resetColor}"
    fi
}

function error
{
    echo -e "${l_redColor}Ko${l_resetColor}"
		echo -e "${l_redColor}$(basename ${l_progName}) error${l_resetColor} : $1" 1>&2
		usage
}

function readOptions
{
    while true; do
        case "$1" in
            --no-color)
                l_redColor="";
                l_greenColor="";
                l_resetColor="";
                shift 1;;
            -h|--help)
                shift
								usage;;
            --)
                shift;
                break;;
            *)
                echo "error : internal problem"
                usage;;
        esac
    done
    l_arguments="$@"
}


function runAutotools
{
    rm -rf   .build
    mkdir -p .build

    echo -n "running aclocal ... "
    touch .build/aclocal.m4
    cp xtdmake/m4/acinclude.m4 .build
    aclocal --output .build/aclocal.m4 > .build/bootstrap.log 2>&1
    check_error $?

    echo -n "running autoconf ... "
    autoconf  >> .build/bootstrap.log  2>&1
    check_error $?

    echo -n "running autoheader ... "
    autoheader  >> .build/bootstrap.log  2>&1
    check_error $?

    echo -n "running automake ... "
    automake --copy --add-missing --foreign -Wgnu -Woverride -Wsyntax -Werror -Wunsupported  >> .build/bootstrap.log  2>&1
    check_error $?

    echo ""
    cat .build/bootstrap.log
    echo ""

    echo -e "${l_greenColor}ready for ./configure [options]${l_resetColor}"
}

l_parseResult=`/usr/bin/getopt -o h\
		--long help,no-color \
		-n "${l_progName}" -- "$@"`

if [ $? != 0 ]; then
    usage
fi

eval set -- "${l_parseResult}"
readOptions "$@"
runAutotools "${l_arguments}"
