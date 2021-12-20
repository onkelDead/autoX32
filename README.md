# autoX32
Automation Recorder for audio production with Ardour and Behringer X32.

Features:
- Record any Behringer X32 fader or knob changes in relation to Ardours MTC time.
- Automation resolution depends on Ardours Timecode Settings (e.g. 30fps -> 60 changes per second).
- Playback automation time synchronously.
- Capable to communication with Ardour based on Jack on ALSA Seq.
- Detect current Ardour session to store automation beside Audio data.
   

![screenshot.png](/screenshot.png?raw=true)

## Required libraries:
    
The following libraries are added to build using pkg-config (versions I'd use):

    gtk+-3.0 (3.24.20)
    gtkmm-3.0 (3.24.2)
    libxml++-2.6 (2.40.1)
    liblo (0.30)
    jack (1.9.12)    
    check (0.10.0)
    libconfig (1.5)

## Build instruction

    aclocal
    autoconf
    automake --add-missing
    ./configure
    make
    sudo make install

    