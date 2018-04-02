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
check_install wmctrl
check_install xdotool
check_install python3-pip
sudo snap install slack --classic

sudo apt upgrade -y
sudo apt upgrade-dist

sudo -H pip3 install --upgrade pip
pip3 install pyserial

cd ~/Documents
git clone https://github.com/ben-jammin-beckvold/gitprojects.git

cd ~
git clone https://github.com/piaggiofastforward/pff-ros-ws.git

# add user to dialout
sudo usermod -a -G dialout "$USER"

# git branch
echo "parse_git_branch() { git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/ (\1)/'; }" >> ~/.bashrc
echo 'export PS1="\[\e[0;32m\]\u@\h:\[\033[32m\]\w\[\033[33m\]\$(parse_git_branch)\[\033[00m\]$ \[\e[m\]"' >> ~/.bashrc

# bashrc
echo "alias gst='git status'" >> ~/.bashrc

# emergia
cd ~/Downloads
wget energia.nu/downloads/downloadv4.php?file=energia-1.6.10E18-linux64.tar.xz
wget http://energia.nu/files/71-ti-permissions.rules

sudo mv 71-ti-permissions.rules /etc/udev/rules.d/
cd ~ && tar -xf ~/Downloads/downloadv4.php?file=energia-1.6.10E18-linux64.tar.xz

# Saleae software
cd ~/Downloads
wget downloads.saleae.com/logic/1.2.18/Logic%201.2.18%20(64-bit).zip
unzip downloads.saleae.com/logic/1.2.18/Logic%201.2.18%20(64-bit).zip
mv Logic\ 1.2.18\ \(64-bit\)/ ~/
cd ~/Logic\ 1.2.18\ \(64-bit\)/Drivers && ./install_driver.sh


# reload udev rules
sudo udevadm control --reload
sudo udevadm trigger --action=add
