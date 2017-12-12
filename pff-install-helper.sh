#!/bin/bash

# setup common directories if non-root user
if [[ $EUID -ne 0 ]]; then
  # config files
  BASHRC="${HOME}/.bashrc"
  PFF_CONFIG="${HOME}/.pff-config"
  BAGS_DIR="$HOME/bags"
  MAPS_DIR="$HOME/maps"
  LOG_DIR="$HOME/log"
fi

check_install() {
  if [ $(dpkg-query -W -f='${Status}' ${1} 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
    printf "\tInstalling package [${1}].\n"
    sudo apt install ${1} -y
  else
    printf "\tPackage [${1}] already installed.\n"
  fi
}

has_nvidia() {
  echo $(lspci  -v -s  $(lspci | grep VGA | cut -d" " -f 1) | grep NVIDIA)
}

check_gpu() {
  if [ "$(has_nvidia)" ]; then
    printf "\n\tHas nvidia GPU.\n\n"
  else
    printf "\n\tDoes not have GPU.\n\n"
  fi
}

check_uninstall() {
  if [ $(dpkg-query -W -f='${Status}' ${1} 2>/dev/null | grep -c "ok installed") -eq 0 ];
  then
    printf "\tPackage [${1}] already uninstalled.\n"
  else
    printf "\tPackage [${1}] installed.  Uninstalling ...\n"
    sudo apt install --purge ${1} -y
  fi
}

add_string_to_file() {
  FILE="$1"
  VAR="$2"
  local NEW_VAR
  [[ $# -gt 2 ]] && NEW_VAR="$3"
  if grep -q "$VAR" "$FILE"
  then
    :
    # printf "\n\t[%s]: String [%s] already exists\n" "$FILE" "$VAR"
  else
    [[ -z ${NEW_VAR} ]] && printf "$VAR\n" >> "$FILE"
    [[ -n ${NEW_VAR} ]] && printf "$NEW_VAR\n" >> "$FILE"
    printf "\n\t[%s]: Add string [%s] to flie\n" "$FILE" "$VAR"
  fi
}

remove_string_in_file() {
  FILE="$1"
  VAR="$2"
  if [[ ! -e $FILE ]]; then
    echo "File [$FILE] does not exist."
    return 1;
  fi
  if grep -q "$VAR" "$FILE"
  then
    sed -i "/$(echo $VAR | sed -e 's/\([[\/.*]\|\]\)/\\&/g')/d" $FILE
    printf "\n\t[%s]: Removed string [%s].\n" "$FILE" "$VAR"
  else
    :
    # printf "\n\t[%s]: String [%s] not found.\n" "$FILE" "$VAR" "$NEW_VALUE"
  fi
}

add_string_to_bashrc() {
  if [ $# -gt 0 ]; then
    add_string_to_file "$1" "${HOME}/.bashrc"
  fi
}

remove_directory_if_empty() {
  DIR="$1"
  # return if not a directory
  [[ ! -d $DIR ]] && return 0
  # remove if empty
  [[ ! "$(ls -A $DIR)" ]] && rm -rv "$DIR"
}
