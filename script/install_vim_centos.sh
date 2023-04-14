#!/bin/sh

# Put the script in vim source directory.
# cn: 将该脚本放到vim的源码目录下，该脚本会自动安装插件所需的东西
#     该脚本会自动删除所有.cache文件

# 由于要启用提示插件，所以需要安装python3（python2系统自带）

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

sudo yum -y install python3
[[ $? -ne 0 ]] && error_message "install python3 failed"

python_conf_dir=`find /usr/lib64/python3*/config*/ | head -1`
[[ $? -ne 0 ]] && error_message "find python3 config path failed"

sudo yum -y install ncurses-devel
[[ $? -ne 0 ]] && error_message "install ncurses-devel failed"

./configure --enable-gui=auto --enable-cscope --enable-multibyte \
  --enable-xim --enable-fontset --with-features=huge \
  --enable-luainterp --enable-pythoninterp --enable-sniff \
  --with-python-config-dir=/usr/lib64/python2.7/config/ \
  --enable-python3interp=yes \
  --with-python3-config-dir=${python_conf_dir} \
  –-enable-fail-if-missing \
  --with-lua-prefix=/usr/local

[[ $? -ne 0 ]] && error_message "configure vim source build failed"

make && sudo make install

[[ $? -ne 0 ]] && error_message "vim build or install failed"

# install pynvim for vim plugin vim-hug-neovim-rpc
# cn: 提示插件必须要这个，否则提示不了
sudo pip3 install pynvim
[[ $? -ne 0 ]] && error_message "pynvim install failed"