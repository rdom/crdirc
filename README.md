DIRC simulations with Cosmic Ray generator (CRY 1.7) 

## Requirments
```
root 6
geant4
QT4
```

## Installation
```
git clone https://github.com/rdom/crdirc

cd crdirc
mkdir build
cd build
cmake ..
make -j4
```

example:

```
./crdirc -study 500 -r 0 -i hits.root -e 10000000 -b 1
```
with event display:
```
./crdirc -study 500 -r 0 -i hits.root -e 200
```
![alt text](https://github.com/rdom/crdirc/blob/master/pics/event_dispaly_cry.png)

example with muons:
```
./crdirc -study 500 -x mu- -p 10 -i hits.root -e 20 -w 1
```
![alt text](https://github.com/rdom/crdirc/blob/master/pics/event_dispaly_mu.png)

## Example of script usage from macro folder

use "-b 1" for a lage number of events
```
./crdirc -study 500 -x mu- -p 10 -i hits.root -e 1000 -w 1 -b 1

root draw_hp.C'("../build/hits.root")'
root draw_tracker.C'("../build/hits.root")'
```


## Synopsis
```
crdirc [OPTION] [ARGUMENT] ... [OPTION] [ARGUMENT]
```

## Options
```

-o    output file name
-i    input file name
-u    look-up file name
-pdf  pdf file name

-r    run type
                0    cosmic simulation
                1    look-up table generation
                2    geometrical reconstruction
                3    likelihood calculation
		4    create pdf
		5    simulate pdf

-study  study id

-fid  fild id

-g    geometry configuration
                1       in vacuum
		2018    1 bar (default)
                2       2 bars

-gsx    radiator-prism x step in mm

-gsy    radiator-prism y step in mm

-gx  x position of beam-radiator hit point in mm
                0    is a middle of radiator 

-gz  z position of beam-radiator hit point in mm
                0    is at edge of radiator (prism side)

-h    radiator type
                1    bar (default)
                2    plate

-c   MCP layout
                0    MCP covers all FD plain
		4    2x2 old MCP-PMT (8x8 pixels, 0.65 CE) 
		12   3x4 new MCP-PMT (8x8 pixels, 0.95 CE) 
		2015
		2016
		20171 2x4 layout
                2021 3x3+2 layout - Barrel DIRC baseline design
		2030 3x4 EIC layout with 3mm x 3mm pixels (default)

-l    focusing system
                0    no lens
                1    2-layer spherical lens
                2    2-layer cylindrical lens
                3    3-layer spherical lens
                4    1-layer spherical lens (with air-gap)
                5    test lens
                6    3-layer cylindrical lens		
                10   ideal lens (thickness = 0, ideal focusing)

-theta    polar angle between particle beam and bar radiator

-phi  azimuth angle between particle beam and bar radiator

-e    number of simulated events
-f     number of the first event for the reconstruction

-x    particle type
              "cry" (cosmic ray library, default)
              "pi+"
              "mu-"
              "proton"
              "kaon+"
                 ...
              "opticalphoton"
	      "mix_pip" - (mix pions and protons)
	      "mix_pik" - (mix pions and kaons ) 

-p    particle momentum [GeV/c]

-w    physical list
                0    standard  (default)
                1    without multiple scattering and bremsstrahlung
                10   monochromatic Cherenkov light
                11   10 + 1

-tr    time resolution [ns]
               0.2  (default)  

-seed  seed number for the random generator 

-b     batch mode
               1    run silent (without GUI)

-t1    position of the tracker #1 relative to the center of radiator bar (@ 90 deg) [mm]
       1300 (default)      

-t2    position of the tracker #2 relative to the center of radiator bar (@ 90 deg) [mm]
       300 (default)   

-t3    position of the tracker #3 relative to the center of radiator bar (@ 90 deg) [mm]
       -400 (default)
       
```

