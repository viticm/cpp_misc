#!/bin/bash
#the git tool script
#@version 1.0
#@author viticm<viticm.ti@gmail.com>
#@date 2023-06-13 15:08

currentdir=`pwd`
#help text, the script desc
#@param void
#@return void
function help_text() {
  cat <<EOF
${0} ver 1.0

options:
--commit   diff the file or directory and comfirm commit.
EOF
}

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

#get format path
#param string path
#return string
function getpath() {
  local path=${1}
  local newpath=${path}
  if [[ "~" == ${path:0:1} ]] ; then
    newpath=`echo ${path} | sed "s;~;${HOME};"`
  fi
  echo ${newpath}
}

#show diff of file
#param string path
#return void
function showdiff() {
  local path=${1}
  status=`git status -s $path | awk '{print $1}'`
  [[ $status == '??' ]] && echo 'new file: '$path && cat $path
  [[ $status != '' ]] && git diff HEAD^ $path
}

#commit git files or directorys.
#param string log
#return string
function commit() {
  local argc=${#}
  if (($argc < 3)); then
    error_message 'the parameters must more than 3'
  fi
  local log=${2}
  local i=0
  local files=''

  # git diff(uncontroll will use git add to version).
  for var in "${@}"
  do
    ((++i))
    if (($i >= 3 )); then
      # echo $var
      local list=`git status -s $var | grep -v cscope |
        grep -v tags | awk '{print $2}'`
      for path in $list
      do
        local status=`git status -s $path | awk '{print $1}'`
        [[ $status == '??' ]] && echo $path' not in version controll'
        [[ $status == '??' ]] && echo 'new file: '$path && cat $path
        if [[ $status != '' ]] ; then
          [[ $status != 'D' && ! -d $var ]] && git diff HEAD^ $path
          files=$files' '$path
        fi
      done
      [ -d $var ] && git diff HEAD $var
    fi
  done

  # git commit.
  echo 'Log: '$log
  read -p "Are you sure to commit this changes(yes/no):" answer
  if [[ $answer == 'yes' ]] ; then
    echo 'files: '$files
    if [[ $files != '' ]] ; then
      git add $files
      git commit -m '$log'
      #echo "git commit -m '$log'"
    fi
  fi
}

#the script main function, like c/c++
function main() {
  cmd=${1}
  case "${cmd}" in
    --help ) help_text;;
    -h) help_text;;
    --commit) commit "${@}";;
  esac
}
if [[ "" == ${@} ]] ; then
  error_message "${0}: no commond specified.You can use <${0} --help>
                get parameters for this script."
else
  main "${@}"
  exit 0
fi
