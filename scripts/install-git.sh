#!/bin/bash
DIRPATH="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
. "$DIRPATH/pff-install-helper.sh"

check_install ssh
check_install vim
check_install git

ssh-keygen -t rsa -b 4096 -C "carlos.asmat+robot@piaggiofastforward.com" -f $HOME/.ssh/id_rsa_github -N ''
eval "$(ssh-agent -s)"
ssh-add $HOME/.ssh/id_rsa_github
touch $HOME/.ssh/config
add_string_to_file $HOME/.ssh/config "IdentityFile ~/.ssh/id_rsa_github"
git config --global user.name "PFF Robot"
git config --global user.email carlos.asmat+robot@piaggiofastforward.com
printf "Add this key to PFF Robot's github acount: \n\n"
cat $HOME/.ssh/id_rsa_github.pub
