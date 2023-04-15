#!/bin/sh

# Install cpp develop tools on centos 7.
# cn: 安装c++的开发环境 可以指定gcc版本，默认情况下使用系统自带
#     默认指定版本的gcc只对当前用户生效，如果想要所有用户有效请修改for_alluser=1

gccversion="default"
curdir=`pwd`
username=`whoami`
for_alluser=0

# Install base tools.
sudo yum -y groupinstall "Development Tools"
sudo yum -y install git
sudo yum -y wget

#print error message, red words
#@param string message
#@return void
function error_message() {
  local message=${@}
  echo -e "\e[0;31;1merror: ${message}\e[0m"
  exit 1
}

#print warning message, yellow words
#@param message
#@return string void
function warning_message() {
  local message=${@}
  echo -e "\e[0;33;1mwarning: ${message}\e[0m"
}

# Install gcc by version
# Also can clone with git like: git clone git://gcc.gnu.org/git/gcc.git
# @param version The gcc version, not exists will download from network.
# @return void
function install_gcc() {
  local version=${1}
  local packagename=gcc-${version}.tar.gz
  local baseurl=https://mirrors.nju.edu.cn/gnu/gcc
  local installpath=/usr/local/opt/gcc-${gccversion}
  [ ! -f $packagename ] && wget -c ${baseurl}/gcc-${gccversion}/${packagename}
  [ ! -f $packagename ] && error_message "Can't found package: ${packagename}"
  tar -xzvf $packagename
  cd gcc-${gccversion} && mkdir build && cd build
  [ $? != 0 ] && error_message "Have error"

  ../configure --prefix=$installpath --enable-bootstrap \
    --enable-checking=release --enable-languages=c,c++ --disable-multilib
  make && sudo make install
  [ $? != 0 ] && error_message "Have error"
  sudo cp ${curdir}/enable_gcc $installpath
  local enablefile=${installpath}/enable_gcc
  sudo sed -i "s;\${installpath};${installpath};g" $enablefile
  if [[ $for_alluser -eq 1 ]] ; then
    sudo echo "source ${enablefile}" >> /etc/profile
    sudo source /etc/profile
  else
    echo "source ${enablefile}" >> ~/.bashrc
    source ~/.bashrc
  fi
  [ $? != 0 ] && error_message "Have error"
  local cur_gccversion=`gcc --version | head -1 | awk '{print $3}'`
  [ $cur_gccversion != $gccversion ] && "Install gcc failed"
}

# Install other tools.
# @return void
function install_other() {
  # Debug
  sudo yum -y install strace
  # Net
  sudo yum -y install netstat
  sudo yum -y install telnet
  sudo yum -y install tcpdump
}

# Check need install by gcc version.
[ ${gccversion} != "default" ] && install_gcc ${gccversion}

install_other

[ 0 -eq $? ] && echo "Install develop tools success"
