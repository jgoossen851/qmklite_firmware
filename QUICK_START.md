# Quick Start Directions

This project includes a Vagrantfile that will allow you to build a new firmware for your keyboard very easily without major changes to your primary operating system. This also ensures that when you clone the project and perform a build, you have the exact same environment as anyone else using the Vagrantfile to build. This makes it much easier for people to help you troubleshoot any issues you encounter.

## Requirements

Using the Vagrantfile in this repository requires you have [Vagrant](http://www.vagrantup.com/) as well as [VirtualBox](https://www.virtualbox.org/) (or [VMware Workstation](https://www.vmware.com/products/workstation) and [Vagrant VMware plugin](http://www.vagrantup.com/vmware) but the (paid) VMware plugin requires a licensed copy of VMware Workstation/Fusion).

Other than having Vagrant and Virtualbox installed and possibly a restart of your computer afterwards, you can simple run a 'vagrant up' anywhere inside the folder where you checked out this project and it will start a Linux virtual machine that contains all the tools required to build this project. There is a post Vagrant startup hint that will get you off on the right foot, otherwise you can also reference the build documentation below.

Build Firmware and Program Controller
-------------------------------------
See [doc/build.md](tmk_core/doc/build.md), or the README in the particular keyboard/* folder.

Change your keymap
------------------
See [doc/keymap.md](tmk_core/doc/keymap.md).

## Flashing the firmware

The "easy" way to flash the firmware is using a tool from your host OS like the Teensy programming app. [ErgoDox EZ](keyboard/ergodox_ez/README.md) gives a great example.
