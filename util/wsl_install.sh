#!/bin/bash

download_dir=wsl_downloaded

function install_utils {
    rm -f -r $download_dir
    mkdir $download_dir

    pushd $download_dir

    echo "Installing dfu-programmer"
    wget 'http://downloads.sourceforge.net/project/dfu-programmer/dfu-programmer/0.7.2/dfu-programmer-win-0.7.2.zip'
    7z x -odfu-programmer dfu-programmer-win-0.7.2.zip

    echo "Installing dfu-util"
    wget 'http://dfu-util.sourceforge.net/releases/dfu-util-0.9-win64.zip'
    7z x dfu-util-0.9-win64.zip

    echo "Installing teensy_loader_cli"
    wget 'https://www.pjrc.com/teensy/teensy_loader_cli_windows.zip'
    7z x teensy_loader_cli_windows.zip

    echo "Installing Atmel Flip"
    wget 'http://www.atmel.com/images/Flip%20Installer%20-%203.4.7.112.exe'
    7z x -oFlip Flip\ Installer\ -\ 3.4.7.112.exe

    echo "Downloading the QMK driver installer"
    wget -qO- https://api.github.com/repos/qmk/qmk_driver_installer/releases | grep browser_download_url | head -n 1 | cut -d '"' -f 4 | wget -i -

    rm -f *.zip
    rm Flip\ Installer\ -\ 3.4.7.112.exe

    popd > /dev/null
}

function install_drivers {
    pushd $download_dir
    cmd.exe /C qmk_driver_installer.exe $1 $2 ../drivers.txt
    popd > /dev/null
}

dir=$(cd -P -- "$(dirname -- "$0")" && pwd -P)

if [[ $dir != /mnt/* ]];
then
    echo
    echo "You need to clone the qmk_firmware repository outside the linux filesystem."
    echo "Otherwise the windows executables can't be run."
    exit 1
fi

pushd "$dir"

while true; do
    echo
    echo "Do you want to install all toolchain dependencies needed for compiling QMK?"
    echo "This will run install_dependencies.sh, which calls apt-get upgrade."
    echo "If you don't want that, you can install the dependencies manually."
    read -p "(Y/N) " res
    case $res in
        [Yy]* ) sudo ./install_dependencies.sh; break;;
        [Nn]* ) break;;
        * ) echo "Invalid answer";;
    esac
done

echo "Installing dependencies needed for the installation (p7zip-full, wget)"
echo "This will ask for the sudo password"
sudo apt-get install p7zip-full wget


if [ ! -d "$download_dir" ]; then
    install_utils
else
    while true; do
        echo
        read -p "The utils seem to already be downloaded, do you want to re-download them and update to the newest version (Y/N) " res
        case $res in
            [Yy]* ) install_utils; break;;
            [Nn]* ) break;;
            * ) echo "Invalid answer";;
        esac
    done
fi

while true; do
    echo
    echo "Which USB drivers do you want to install?"
    echo "(A)all - All supported drivers will be installed"
    echo "(C)onnected - Only drivers for connected keyboards (in bootloader/flashing mode) will be installed"
    echo "(F)force - Like all, but will also override existing drivers for connected keyboards"
    echo "(N)one - No drivers will be installed, flashing your keyboard will most likely not work"
    read -p "(A/C/F/N)? " res
    case $res in
        [Aa]* ) install_drivers --all; break;;
        [Cc]* ) install_drivers; break;;
        [Ff]* ) install_drivers --all --force; break;;
        [Nn]* ) break;;
        * ) echo "Invalid answer";;
    esac
done

echo 
echo "Creating a softlink to the utils directory as ~/qmk_utils."
echo "This is needed so that the the make system can find all utils it need."
read -p "Press any key to continue (ctrl-c to abort)"
ln -sf "$dir" ~/qmk_utils

popd > /dev/null

