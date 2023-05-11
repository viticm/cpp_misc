#!/bin/sh

# Install cpp develop tools on centos 7.
# cn: 安装c++的开发环境 可以指定gcc版本，默认情况下使用系统自带
#     默认指定版本的gcc只对当前用户生效，如果想要所有用户有效请修改for_alluser=1

gccversion="default"
luaversion="default"
luarocks_version="3.9.2"
clangversion="default"
curdir=`pwd`
username=`whoami`
for_alluser=0

alias sudo="sudo env PATH=$PATH"

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

# Check the shell if success.
# @return number
function successed() {
  local code=$?
  if [[ 1 == $code ]] ; then
    code=0
  fi
  echo $code
}

yumversion=`yum --version | head -1`
[ $? -ne 0 ] && error_message "Install script need yum"

# Install base tools.
sudo yum -y groupinstall "Development Tools"
sudo yum -y install git
sudo yum -y install wget

# Install gcc by version(This can use install_withsource function future)
# Also can clone with git like: git clone git://gcc.gnu.org/git/gcc.git
# @param version The gcc version, not exists will download from network.
# @return void
function install_gcc() {
  cd $curdir
  local version=${1}
  local cur_version=`gcc --version | head -1 | awk '{print $3}'`
  if [[ $cur_version == $version ]] ; then
    warning_message "gcc already installed"
    return
  fi
  local packagename=gcc-${version}.tar.gz
  local baseurl=https://mirrors.nju.edu.cn/gnu/gcc
  local installpath=/usr/local/opt/gcc-${version}
  [ ! -f $packagename ] && wget -c ${baseurl}/gcc-${version}/${packagename}
  [ ! -f $packagename ] && error_message "Can't found package: ${packagename}"
  tar -xzvf $packagename
  cd gcc-${version} && mkdir -p dependent
  ./contrib/download_prerequisites --directory=dependent
  local findcmd="find ./dependent/ -maxdepth 1 -type d -name"
  [ ! -d gmp ] && ln -s `$findcmd gmp-*` gmp
  [ ! -d mpfr ] && ln -s `$findcmd mpfr-*` mpfr
  [ ! -d mpc ] && ln -s `$findcmd mpc-*` mpc
  [ ! -d isl ] && ln -s `$findcmd isl-*` isl
  [[ $(successed) != 0 ]] && error_message "have error install gcc"
  mkdir -p build && cd build

  ../configure --prefix=$installpath --enable-bootstrap \
    --enable-checking=release --enable-languages=c,c++ --disable-multilib
  make && sudo make install
  [ $(successed) != 0 ] && error_message "have error install gcc"
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
  [ $(successed) != 0 ] && error_message "have error install gcc"
  cur_version=`gcc --version | head -1 | awk '{print $3}'`
  [ $cur_version != $version ] && error_message "Install gcc failed"
}

# Install clang by version(This can use install_withsource function future)
# @param version The clang version, not exists will download from network.
# @return void
function install_clang() {
  cd $curdir
  local version=${1}
  local cur_version=`clang --version | head -1 | awk '{print $3}'`
  if [[ $cur_version == $version ]] ; then
    warning_message "clang already installed"
    return
  fi
  sudo yum -y install llvm llvm-devel
  [ $(successed) != 0 ] && error_message "have error install clang with llvm"
  local baseurl=https://github.com/llvm/llvm-project/releases/download
  local llvmpackage=llvm-${version}.src.tar.gz
  [ ! -f $llvmpackage ] && wget -c ${baseurl}/llvmorg--${version}/${llvmpackage}
  tar -xvf $llvmpackage
  cd llvm-${version}.src
  mkdir build && cd build
  cmake -DLLVM_INCLUDE_TESTS=0 ../ && make && make install
  [ $(successed) != 0 ] && error_message "have error install clang with llvm"

  cd $curdir
  local packagename=clang-${version}.src.tar.xz
  local installpath=/usr/local/opt/clang-${version}
  [ ! -f $packagename ] && wget -c ${baseurl}/llvmorg--${version}/${packagename}
  [ ! -f $packagename ] && error_message "Can't found package: ${packagename}"
  tar -xvf $packagename
  cd clang-${gccversion}.src
  local cmakepackage=cmake-${version}.src.tar.xz
  [ ! -f $cmakepackage ] && wget -c ${baseurl}/llvmorg--${version}/${cmakepackage}
  [ ! -f $cmakepackage ] && error_message "Can't found package: ${cmakepackage}"
  tar -xvf $cmakepackage
  mkdir -p ../cmake
  cp cmake-${version}.src/* ../cmake/ -r

  mkdir -p build && cd build

  cmake -DCMAKE_INSTALL_PREFIX=$installpath ../
  make && sudo make install
  [ $(successed) != 0 ] && error_message "have error install clang"
  cur_version=`clang --version | head -1 | awk '{print $3}'`
  [ $cur_version != $version ] && error_message "Install clang failed"
}

# Install by source code.
# @param string name The soft name.
# @param string version The package version.
# @param string baseurl The package download base url.
# @param string versioncmd The get version cmd string.
# @param string makecmd If empty then defalt use "make".
# @param string configurecmd If empty then not excute before make.
# @param string installedcmd If empty then not excute after make install.
function install_withsource() {
  cd $curdir
  local name=${1}
  local version=${2}
  local baseurl=${3}
  local versioncmd=${4}
  local curversion=`echo ${versioncmd} | sh`
  if [[ $curversion == $version ]] ; then
    warning_message "${name} already installed"
    return
  fi
  local configurecmd=${5}
  local makecmd=${6}
  [ "" == $makecmd ] && makecmd="make"
  local installedcmd=${7}
  if [[ "" == $name || "" == $version ||
        "" == $baseurl || "" == $versioncmd ]] ; then
    error_message "install ${name} check param failed"
  fi
  local packagename=${name}-${version}.tar.gz
  [ ! -f $packagename ] && wget -c ${baseurl}/${packagename}
  [ ! -f $packagename ] && error_message "Can't found package: ${packagename}"

  # Build and make install.
  tar -xzvf $packagename
  cd ${name}-${version}
  [[ $configurecmd != "" ]] && $configurecmd
  $makecmd && sudo make install
  [ $(successed) != 0 ] && error_message "have error install ${name}"

  # After installed.
  [[ $installedcmd != "" ]] && $installedcmd
  [[ $installedcmd != "" ]] && [ $(successed) != 0 ] && \
    error_message "have error install ${name}"

  # Check version.
  curversion=`echo ${versioncmd} | sh`
  [[ $curversion != $version ]] && error_message "Install ${name} failed"
}

# Install lua.
# @return void
function install_lua() {
  local version=${1}
  local baseurl=http://www.lua.org/ftp
  local versioncmd="lua -v | awk '{print \$2}'"
  local makecmd="make linux"
  install_withsource lua ${version} "$baseurl" "$versioncmd" "" "$makecmd"
}

# Install luarocks.
# @return void
function install_luarocks() {
  local version=${1}
  local baseurl=https://luarocks.github.io/luarocks/releases
  local versioncmd="luarocks --version | head -1 | awk '{print \$2}'"
  local configurecmd="./configure"
  install_withsource luarocks ${version} "$baseurl" "$versioncmd" \
    "$configurecmd"
}

# Install lua check.
# @return void
function install_luacheck() {
  sudo luarocks install luacheck
  [ $(successed) != 0 ] && error_message "install luacheck error"
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
  # Other
  sudo yum -y install lrzsz
  sudo yum -y install cmake
}

# Check need install by gcc version.
[ ${gccversion} != "default" ] && install_gcc ${gccversion}

# Check need install by clang version.
# [ ${clangversion} != "default" ] && install_clang ${clangversion}

sudo yum -y install lua-devel
# Check need install by lua version.
[ ${luaversion} != "default" ] && install_lua ${luaversion}

install_luarocks ${luarocks_version}
install_luacheck
install_other

[ 0 == $(successed) ] && echo "install develop tools success"
