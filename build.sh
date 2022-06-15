#!/bin/bash

compiler=""
compiler_path=""
install=""
install_path="/usr/local/mqttclient"
build_type="Release"
build_shared="OFF"
build_example="OFF"
cmake_arg=""
toolchanin=""
cpu_core="1"

fun_do_config() {
    processor=$(cat /proc/cpuinfo | grep "processor" | wc -l)
    cpu_core=$(expr ${processor} / 2)
    current_pwd=$(
        cd "$(dirname "$0")"
        pwd
    )
    mkdir -p $current_pwd/build $current_pwd/build/bin $current_pwd/build/lib
    cd $current_pwd/build
}

fun_do_clean() {
    if [ -d "$current_pwd/build" ]; then
        rm -rdf $current_pwd/build
    fi
}

fun_do_help() {
    echo "usage: $0 [-i install path] [-c <compiler> / <compiler path>] [-s <on> / <off>] "
    echo "[-r / --release] [-d / --debug] [-e] [--toolchanin <toolchanin file>] [--clean]"
    echo "  [-i] install path: install wturs path"
    echo "  [-c] compiler: specify the compiler you are using, default: g++"
    echo "  [-c] compiler path: specify the compiler path you are using"
    echo "  [-r] build release type: specify the release type, default: release "
    echo "  [-d] build debug type: specify the build type, default: off "
    echo "  [--toolchanin] specify toolchanin file"
    echo "  eg:"
    echo "      $0"
    echo "      $0 -i"
    echo "      $0 -i /usr/lib/"
    echo "      $0 -carm-linux-gnueabihf-gcc"
    echo "      $0 -c/usr/bin/arm-linux-gnueabihf-gcc"
    echo "      $0 -r"
    echo "      $0 -d"
    echo "      $0 -soff"
    echo "      $0 -son"
    echo "      $0 --toolchanin [toolchanin file]"
}

fun_do_install() {
    install="true"
    if [ " $1" != " " ]; then
        install_path=$1
    fi
}

fun_do_compiler() {
    if [ " $1" != " " ]; then
        compiler=$1
    fi
}

fun_do_toolchanin() {
    if [ " $1" != " " ]; then
        toolchanin=$1
    fi
}

fun_do_example() {
    build_example="ON"
}

fun_do_build_debug() {
    build_type="Debug"
}

fun_do_build_release() {
    build_type="Release"
}

fun_do_config_shared() {
    if [ " $1" != " " ]; then
        build_shared=$1
    fi
}

fun_do_check() {
    if [ " $compiler" != " " ]; then
        compiler_path=$(which $compiler)
        if [ " $compiler_path" == " " ]; then
            echo -e "\033[31mNo $compiler compiler found in the system\033[0m"
            exit
        fi
    fi

    echo "compiler : $compiler"
    echo "compiler path : $compiler_path"
    echo "install : $install"
    echo "install path : $install_path"
    echo "build type : $build_type"
    echo "build example : $build_example"
}

fun_cmake_arg_init() {
    if [ " $compiler_path" != " " ]; then
        cmake_arg="-DCMAKE_C_COMPILER=$compiler_path $cmake_arg"
    fi

    if [ " $install_path" != " " ]; then
        cmake_arg="-DCMAKE_INSTALL_PREFIX=$install_path $cmake_arg"
    fi

    if [ " $build_type" != " " ]; then
        cmake_arg="-DCMAKE_BUILD_TYPE=$build_type $cmake_arg"
    fi

    if [ " $build_shared" != " " ]; then
        cmake_arg="-DBUILD_SHARED_LIBS=$build_shared $cmake_arg"
    fi
    
    if [ " $build_example" != " " ]; then
        cmake_arg="-DBUILD_EXAMPLES=$build_example $cmake_arg"
    fi

    if [ " $toolchanin" != " " ]; then
        cmake_arg="-DCMAKE_TOOLCHAIN_FILE=$toolchanin $cmake_arg"
    fi

    cmake .. $cmake_arg
}

fun_do_make() {
    fun_cmake_arg_init

    make -j $cpu_core

    if [ " $install" != " " ]; then
        sudo make install
    fi
}


main() {
    fun_do_config

    # [-h] [-e] [-i install path] [-c compiler path]
    ARGS=$(getopt -o hdrei::c:s:: --long help,clean,debug,release,example,install::,compiler:,shared::,toolchanin: -- "$@")
    if [ $? != 0 ]; then
        echo "Terminating..." >&2
        exit 1
    fi
    eval set -- "$ARGS"

    while true; do
        case "$1" in
        -i | --install)
            fun_do_install $2
            shift 2
            ;;
        -c | --compiler)
            fun_do_compiler $2
            shift 2
            ;;
        -s | --shared)
            fun_do_config_shared $2
            shift 2
            ;;
        -r | --release)
            fun_do_build_release
            shift
            ;;
        -d | --debug)
            fun_do_build_debug
            shift
            ;;
        -e | --example)
            fun_do_example
            shift
            ;;
        -h | --help)
            fun_do_help
            shift
            exit 0
            ;;
        --clean)
            fun_do_clean;
            shift
            exit 0
            ;;
        --toolchanin)
            fun_do_toolchanin $2
            shift
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "unknow : {$1}"
            fun_do_help
            exit 1
            ;;
        esac
    done

    fun_do_check
    fun_do_make
}

main "$@"


# mkdir -p build build/bin build/lib
# cd build

# if [ " $1" == " " ]; then
#     compiler="gcc"
# elif [ $1 == "--help" -o $1 == "-help" -o $1 == "--h" -o $1 == "-h" ]; then
#     echo "usage: $0 [compiler] [compiler path]"
#     echo "  compiler: specify the compiler you are using, default: gcc"
#     echo "  compiler path: specify the compiler path you are using"
#     echo "  eg:"
#     echo "      ./build.sh"
#     echo "      ./build.sh arm-linux-gnueabihf-gcc"
#     echo "      ./build.sh /usr/bin/arm-linux-gnueabihf-gcc"
#     echo "      ./build.sh arm-linux-gnueabihf-gcc /usr/bin"
#     exit
# else
#     if [ " $2" == " " ]; then
#         compiler=$1
#     else
#         compiler=$2/$1
#     fi
# fi

# path=$(which $compiler)

# if [ " $path" == " " ]; then
#     echo -e "\033[31mNo $compiler compiler found in the system\033[0m"
#     exit
# fi

# cmake .. "-DCMAKE_C_COMPILER=$path"

# make
