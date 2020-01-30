#!/bin/bash

display_help() {
  echo -e "Power Cycle Gita boards - turn off power to select boards for 100ms"
  echo -e "usage-"
  echo -e "  b-resetPower [command]\n"
  echo -e "  Commands\n"
  echo -e "    -m Motion"
  echo -e "    -v Vision"
  echo -e "    -a All 3 Boards"

  exit
}

if [[ $# == 1 ]];
then
  if [[ $1 == "-m" ]]; then 
    CODE=05
  elif [[ $1 == "-v" ]]; then
    CODE=06
  elif [[ $1 == "-a" ]]; then
    CODE=02
  else
    display_help
  fi
else
  display_help
fi

cansend can1 200#$CODE

