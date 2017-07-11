#!/bin/bash

dir=$(cd -P -- "$(dirname -- "$0")" && pwd -P)
download_dir=~/qmk_utils
avrtools=avr8-gnu-toolchain
armtools=gcc-arm-none-eabi

echo "Installing dependencies needed for the installation (quazip)"
pacman --needed -S msys/unzip msys/p7zip

source "$dir/win_shared_install.sh"

function install_avr {
    rm -f -r "$avrtools"
    wget "http://www.atmel.com/images/avr8-gnu-toolchain-installer-3.5.4.91-win32.any.x86.exe"
    7z x avr8-gnu-toolchain-installer-3.5.4.91-win32.any.x86.exe
    rm avr8-gnu-toolchain-installer-3.5.4.91-win32.any.x86.exe
}

function install_arm {
    wget -O gcc-arm-none-eabi.zip "https://developer.arm.com/-/media/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-win32.zip?product=GNU%20ARM%20Embedded%20Toolchain,ZIP,,Windows,6-2017-q2-update"
    unzip -d gcc-arm-none-eabi gcc-arm-none-eabi.zip
    rm gcc-arm-none-eabi.zip
}

pushd "$download_dir"

if [ ! -d "$avrtools" ]; then
    while true; do
        echo
        echo "The AVR toolchain is not installed."
        echo "This is needed for building AVR based keboards."
        read -p "Do you want to install it? (Y/N) " res
        case $res in
            [Yy]* ) install_avr; break;;
            [Nn]* ) break;;
            * ) echo "Invalid answer";;
        esac
    done
else
    while true; do
        echo
        echo "The AVR toolchain is already installed"
        read -p "Do you want to reinstall? (Y/N) " res
        case $res in
            [Yy]* ) install_avr; break;;
            [Nn]* ) break;;
            * ) echo "Invalid answer";;
        esac
    done
fi

if [ ! -d "$armtools" ]; then
    while true; do
        echo
        echo "The ARM toolchain is not installed."
        echo "This is needed for building ARM based keboards."
        read -p "Do you want to install it? (Y/N) " res
        case $res in
            [Yy]* ) install_arm; break;;
            [Nn]* ) break;;
            * ) echo "Invalid answer";;
        esac
    done
else
    while true; do
        echo
        echo "The ARM toolchain is already installed"
        read -p "Do you want to reinstall? (Y/N) " res
        case $res in
            [Yy]* ) install_arm; break;;
            [Nn]* ) break;;
            * ) echo "Invalid answer";;
        esac
    done
fi
popd

echo
echo "******************************************************************************"
echo "Installation completed!"
echo "You need to open a new batch command prompt for all the utils to work properly"
echo "******************************************************************************"