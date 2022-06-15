#!/bin/bash

work_dir=""
compiler=""
compiler_path=""
install=""
install_path="/usr/local/mqttclient"
ld_so_config_path="/etc/ld.so.conf.d"
build_type="-r"
build_arg=""
deb_install=""
deb_name="mqtt-client"

fun_do_config() {
    work_dir=$(
        cd "$(dirname "$0")"
        pwd
    )
    rm ${work_dir}/*.deb
}

fun_do_set_output() {
    if [ " $work_dir" != " " -a "${work_dir}" != "/" ]; then
        mkdir -p "${work_dir}/$deb_name/DEBIAN"
        mkdir -p "${work_dir}/${deb_name}${ld_so_config_path}"
        mkdir -p "${work_dir}/$deb_name/${install_path}"
    else
        echo "$work_dir is not a directory"
        exit -1
    fi
}

fun_do_del_output() {
    if [ " $work_dir" != " " -a "${work_dir}" != "/" ]; then
        sudo rm -rdf "${work_dir}/$deb_name/"
    fi
}

fun_do_generate_ld_conf() {
    target_arch=$(g++ -dumpmachine)

    mqttclient_ld_conf_file="${install_path}/lib"

    if [ ! -d "${work_dir}/${deb_name}${ld_so_config_path}" ]; then
        mkdir -p ${work_dir}/${deb_name}${ld_so_config_path}
    fi

    sudo echo "$mqttclient_ld_conf_file" >${work_dir}/${deb_name}${ld_so_config_path}/${deb_name}.conf
}

fun_do_help() {
    echo "usage: $0 [-i install path] [-c compiler / compiler path] [-s <on> <off>] [-n <name>]"
        echo "[-r / --release] [-d / --debug] [--toolchanin <toolchanin file>]"
    echo "  [-i] install path: install $deb_name path"
    echo "  [-c] compiler: specify the compiler you are using, default: gcc"
    echo "  [-c] compiler path: specify the compiler path you are using"
    echo "  [-r] build release type: specify the release type, default: release "
    echo "  [-d] build debug type: specify the build type, default: no "
    echo "  eg:"
    echo "      $0"
    echo "      $0 -i"
    echo "      $0 -i /usr/lib/"
    echo "      $0 -c arm-linux-gnueabihf-gcc"
    echo "      $0 -c /usr/bin/arm-linux-gnueabihf-gcc"
    echo "      $0 -r"
    echo "      $0 -d"
    echo "      $0 -n <name>"
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
        compiler_path=$(which $compiler)
        if [ " $compiler_path" == " " ]; then
            echo -e "\033[31mNo $compiler compiler found in the system\033[0m"
            exit
        fi
    fi
}

fun_do_build_debug() {
    build_type="-d"
}

fun_do_build_release() {
    build_type="-r"
}

fun_do_config_shared() {
    if [ " $1" != " " ]; then
        build_shared=$1
    fi
}

fun_do_toolchanin() {
    if [ " $1" != " " ]; then
        toolchanin=$1
    fi
}

fun_do_config_name() {
    if [ " $1" != " " ]; then
        deb_name=$1
    fi
}

fun_do_build_lib() {
    $work_dir/build.sh ${build_arg}
}

fun_do_make_deb() {
    # 去掉临时安装目录的前缀
    sudo sed -i "s#${work_dir}/$deb_name##g" $(find ${work_dir}/$deb_name -name "*.cmake")
    # 分号换行，避免太长
    sudo sed -i "s#;#;#g" $(find ${work_dir}/$deb_name -name "*.cmake")
    $work_dir/build_deb.sh "${work_dir}/$deb_name/" "$deb_name.deb"
}

fun_do_arg_init() {
    if [ " $compiler_path" != " " ]; then
        build_arg="-c ${compiler_path} ${build_arg}"
    fi

    if [ " $install_path" != " " ]; then
        build_arg="-i${work_dir}/$deb_name${install_path} ${build_arg}"
    fi

    if [ " $build_type" != " " ]; then
        build_arg="${build_type} ${build_arg}"
    fi

    if [ " $build_shared" != " " ]; then
        build_arg="-s${build_shared} ${build_arg}"
    fi

    echo "${build_arg}"
}

main() {
    ARGS=$(getopt -o hrdi::c:s::n: --long help,release,debug,install::,compiler::,shared:,name:,toolchanin: -- "$@")
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
        -n | --name)
            fun_do_config_name $2
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
        --toolchanin)
            fun_do_toolchanin $2
            shift
            exit 0
            ;;
        -h | --help)
            fun_do_help
            shift
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "unknow : {$1}"
            exit 1
            ;;
        esac
    done

    fun_do_config
    fun_do_set_output
    fun_do_generate_ld_conf
    fun_do_arg_init
    fun_do_build_lib
    fun_do_make_deb
    fun_do_del_output
}

main "$@"
