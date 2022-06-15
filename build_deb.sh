#!/bin/bash

# info
author="jiejie"
email="jiejietop@gmail.com"
github="https://github.com/jiejieTop"
description="is a communication library"

# variable
package_dir=""
package_name=""
info_name=""
arch="x86"
system="all"
major_version="1"
minor_version="2.0"
version="$major_version.$minor_version"

current_pwd=$(
    cd "$(dirname "$0")"
    pwd
)

fun_do_generate_depend() {
    date=$(date)
    changelog_file="author : ${author}
VERSION : ${version}
DATE: ${date}
"

    copyright_file="
******************************************************************
  * @attention
  *
  * github  : ${github}
  * author  : ${author}  
  *
* * ******************************************************************
"

    postinst_file="#!/bin/sh
sudo ldconfig
echo '******************************************************************'
echo 'welcome to use ${info_name} v${version}!'
echo '******************************************************************'
"

    control_file="Source: ${author}
Package: ${info_name}
Version: ${version}
Section: develop / communication
Priority: optional
Architecture: all
Depends : 
Maintainer: ${author}[${email}]
Description: ${description}, git hash : ${git_hash}
"

    echo "$control_file" >$package_dir/DEBIAN/control
    echo "$changelog_file" >$package_dir/DEBIAN/changelog
    echo "$copyright_file" >$package_dir/DEBIAN/copyright
    echo "$postinst_file" >$package_dir/DEBIAN/postinst
}

fun_do_get_system_id()
{
    source /etc/os-release
    system="$ID-$VERSION_ID"
}

fun_do_generate_arch() {
    hostnamectl | grep -i "Architecture: x86"
    if [ $? -ne 0 ]; then
        hostnamectl | grep -i "Architecture: arm"
        if [ $? -ne 0 ]; then
            exit 1
        fi
        arch="arm"
    else
        arch="x86"
    fi
}

fun_do_config() {
    if [ " $1" == " " ]; then
        echo "the construction path of the deb package must be specified"
        echo "$0 [path]"
        exit -1
    fi

    package_dir=$1

    if [ " $2" == " " ]; then
        # take the name of the deb package according to the path
        package_name=${package_dir##*/}.deb
    else
        package_name=$2
    fi

    info_name=${package_name%.*}

    if [ ! -d "${package_dir}/DEBIAN" ]; then
        mkdir -p ${package_dir}/DEBIAN
        fun_do_generate_depend
        echo "please create a simulated root directory in ${package_dir}, and put the content in this directory"
        exit -1
    fi

    echo "build $package_name in $package_dir"

}

fun_do_make_deb() {
    package_name="${package_name%.*}-$system-$arch-$version.${package_name##*.}"
    sudo chmod 775 $package_dir/DEBIAN/postinst
    dpkg -b $package_dir $package_name
}

main() {
    cd $current_pwd
    fun_do_config $1 $2
    fun_do_generate_arch
    fun_do_get_system_id
    fun_do_generate_depend
    fun_do_make_deb
    cd -
}

main "$@"
