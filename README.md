# autoX32
Automation Recorder for audio production with Ardour and Behringer X32.

![screenshot.png](/screenshot.png?raw=true)

## Required libraries:
    
The following libraries are added to build using pkg-config (versions I'd use):

    gtk+-3.0 (3.24.20)
    gtkmm-3.0 (3.24.2)
    libxml++-2.6 (2.40.1)
    liblo (0.30)
    jack (1.9.12)    
    check (0.10.0)

## Build instruction

    aclocal
    autoconf
    automake --add-missing
    ./configure
    make
    sudo make install

    