# linux-keyboard-clean
Temporarilty lock all HID inputs so that the keyboard can be cleaned

Dependencies can be installed by running
	sudo apt install libevdev2 libevdev-dev pkg-config

Clone the project and cd into the repo. To compile, run
	
	cmake -S . -B /your/installation/directory -DCMAKE_BUILD_TYPE=Release
	cmake --build /your/installation/directory

or with Ninja
	
	cmake -S . -B /your/installation/directory -G Ninja
	cmake --build /your/installation/directory

The executable binary has to be run using ROOT privileges since it has to access the information in /dev/input. This is circumvented by changing the executable group type after compilation so that ROOT access is not necessary. To turn this off, toggle the setting in the CMakeLists.txt file.
	
	option(ADD_EVENT_PRIVIELGES "Be able to run without sudo" OFF)

Currently, there is no guarantee that this will compile.

The binary defaults to a 30 second pause but takes inputs to change that up to maximum of 120 seconds and can be run by

    ./keyboard=lock 5

in the chosen installation directory. VS code configuration files are also included to run with the CodeLLDB extension.
