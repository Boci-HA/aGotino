# aGotino
A simple telescope Goto solution based on Arduino Nano (or Uno) that supports:

- aGotino commands - an Android phone can do the job via an [USB OTG cable](https://www.amazon.com/s?k=usb+otg+cable) and a [Serial App](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal&hl=it)
- basic Meade LX200 protocol - drive with Stellarium or any software that supports INDI

Goal is to build a simple & cheap, but good quality, solution for tracking and *augmented starhopping*: you point the scope to something you can  easily find and get help to reach a remote, low magnitude object.

No additional boards needed, just an Arduino Nano and two Stepper Drivers do the job. Photos and hardware details on [CloudyNights (English)](https://www.cloudynights.com/topic/735800-agotino-a-simple-arduino-nano-goto/) or [Astronomia.com (Italian)](https://www.astronomia.com/forum/showthread.php?34605-aGotino-un-goto-con-Arduino).

![aGotino](https://www.cloudynights.com/uploads/gallery/album_14775/sml_gallery_329462_14775_4192.jpg)

### Features

- tracking - move Right Ascension at sidereal speed, 1x
- a two button remote allows to cycle among forward and backward speeds (8x) on RA&Dec
- listen on serial port for basic LX200 commands
- listen on serial port for aGotino commands

### aGotino Command set
**x** can be **s (set)** or **g (goto)**:    
  - **`x HHMMSS±DDMMSS`** set/goto coordinates
  - **`x Mn`**            set/goto Messier object n
  - **`x Sn`**            set/goto Star number n in aGotino Star List
  - **`±RRRR±DDDD`**     slew Ra&Dec by RRRR&DDDD primes (RRRRx4 corresponds to arcsecs)
  - **`±debug`**       verbose output
  - **`±sleep`**       power saving on dec motor when unused
  - **`±speed`**       increase or decrease speed by 4x
  - **`±range`**       increase or decrease max slew range (default 30°)

blanks are ignored and can be omitted.

> WARNING: watch your scope while slewing!
> There are no controls to avoid collisions with mount,
> for this reason default maximum range for slew is 30°

#### [aGotino Star List](https://github.com/mappite/aGotino/blob/main/aGotino-StarList.pdf)

Contains all α, β, γ constellation stars up to mag 4 and other stars up to mag 3. The goal is to provide a quick lookup number reference for easy-to-point stars vs having to type coords. Credits to Nasa's [BSC5P - Bright Star Catalog](https://heasarc.gsfc.nasa.gov/W3Browse/star-catalog/bsc5p.html) and KStars project for star names.

#### Example 

Point scope to Mizar in UMa, set Star 223 in aGotino Star List, and slew to M101 Pinwheel Galaxy;  point Altair in Aql, set using coordinates, and slew to M11 Wild Duck

    19:06:34 aGotino: ready.
    > s S223
    19:06:58 Set Star 223      
    19:06:58 Current Position: 13h23'56" 54°55'31
    > g M101
    19:07:08 Goto M101
    19:07:08  *** moving...
    19:07:17  *** done
    19:07:17 Current Position: 14h03'12" 54°20'57"
    > s 195147+085206
    19:09:59 Current Position: 19h51'47" 08°52'06"
    > g M11
    19:10:19 Goto M11
    19:10:19  *** moving...
    19:10:32  *** done
    19:10:32 Current Position: 18h51'06" ‑06°16'00"

Slew +1° Dec (North) and -1° in RA.  
*Note:* 1° = 60' (arcmins) translates to 60*4 secs = 4 mins.

    19:22:28 Current Position: 02h03'54" 42°19'47
    > -0060+0060
    21:23:10  *** moving...
    21:23:12  *** done
    21:23:12 Current Position: 1h59'54" 43°19'47"

[Here a video: look aGotino in action](https://www.youtube.com/watch?v=YF_J7_7lyB4)

### Meade LX200 Protocol Support

Sync&Slew actions are supported, commands **`:GR :GD :Sr :Sd :MS :CM`**  
Tested with Stellarium (direct) and INDI LX200 Basic driver (KStars, Cartes du Ciel, Stellarium, etc)

[Here a video: Stellarium and aGotino](https://youtu.be/PdkoGX5PcDA)

### Files

    aGotino.ino           C Source
    catalogs.h            Contains catalogues (Messier and aGotino Star List in J2000.0)
    aGotino-StarList.pdf  Star list in PDF
    aGotino-wiring.png    wirings 

### AstroMath (i.e. adapt for your mount)

    How to calculate STEP_DELAY to drive motor at right sidereal speed for your mount
    
    Worm Ratio                144   // 144 eq5/exos2, 135 heq5, 130 eq3-2
    Other (Pulley/Gear) Ratio   2.5 // depends on your pulley setup e.g. 40T/16T = 2.5
    Steps per revolution      400   // or usually 200 depends on your motor
    Microstep                  32   // depends on driver
     
    MICROSTEPS_PER_DEGREE   12800   // = WormRatio*OtherRatio*StepsPerRevolution*Microsteps/360
                                    // = number of motor microsteps to rotate the scope by 1 degree
     
    STEP_DELAY              18699   // = (86164/360)/(MicroSteps per Degree)*1000000
                                    // = microseconds to advance a microstep
                                    // 86164 is the number of secs for earth 360deg rotation (23h56m04s)
                                  
The above example is for an EQ5/Exos2 with 40T-16T pulleys which results in 53 microsteps/second or .281 arcsec/microstep.

### Hardware

- 2 Stepper Motors:  Nema 17 400 step per revolution seems a great solution - for visual only you can select smaller ones
  - Supports to attach motors to the mount - depends on your mount, be creative and use a 3D printer if you have one ;)
- 2 Drivers: with at least 32 microsteps. Cheap DRV8825 works, but better TMC or LV can be used of course
- 4 Pulleys and 2 Belts (GT2). Size depends on your mount (see [Belt Calculator](https://www.bbman.com/belt-length-calculator/))
- RJ11 cable with two RJ11 sockets to connect the Dec Motor
- Dupont cables, a couple of momentary buttons and a 100µF Capacitor to protect the circuit.
- Arduino Nano

![Hardware](https://imgur.com/zhQLEPC.png)
