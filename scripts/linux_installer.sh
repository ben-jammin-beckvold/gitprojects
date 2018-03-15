#!/bin/bash

check_install() {
    if [ $(dpkg-query -W -f='${Status}' ${1} 2>/dev/null | grep -c "ok installed") -eq 0 ];
    then
        printf "\tInstalling package [${1}].\n"
        sudo apt install ${1} -y
    else
        printf "\tPackage [${1}] already installed.\n"
    fi
}

# sublime-text
wget -qO - https://download.sublimetext.com/sublimehq-pub.gpg | sudo apt-key add -
sudo apt-get install apt-transport-https
echo "deb https://download.sublimetext.com/ apt/stable/" | sudo tee /etc/apt/sources.list.d/sublime-text.list

sudo apt update

check_install vim
check_install git
check_install terminator
check_install htop
check_install tree
check_install sublime-text
sudo snap install slack --classic

sudo apt upgrade -y
sudo apt upgrade-dist

cd ~/Documents
git clone https://github.com/ben-jammin-beckvold/gitprojects.git

cd ~
git clone https://github.com/piaggiofastforward/pff-ros-ws.git

# add user to dialout
sudo usermod -a -G dialout "$USER"

# git branch
echo "parse_git_branch() { git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'; }" >> ~/.bashrc
echo 'export PS1="\[\e[0;32m\]\u@\h:\[\033[32m\]\w\[\033[33m\]\$(parse_git_branch)\[\033[00m\]$ \[\e[m\]"' >> ~/.bashrc



