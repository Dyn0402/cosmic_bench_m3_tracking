### About this repository

This is the M3 cosmic bench tracking software: it reads raw DAQ data from the
cosmic ray test bench (`DataReader`), builds pedestals/signal trees from it,
and reconstructs and analyses muon tracks (`tracking`) to characterize the
detectors.

The codebase was inherited from a previous student/developer (originally
hosted at `kallithea.quarkstrange.me/thesis/tracking`, imported here with its
full history). Since inheriting it, the main fix has been to the handling of
zero-suppressed (ZS) data, which was causing the data reading/tracking chain
to fail on ZS-compressed runs — this has been fixed.

The script actually used day-to-day to run the full chain (pedestal
computation, cosmic data analysis, and tracking) on a data run is
`run_tracking_single.sh`.

### Build / current state (updated 2026-06-18)

The binaries are built and run against **ROOT 6** (`~/root_6_30_02`) — the same
ROOT version used by the `Cosmic_Bench_DAQ_Control` decode/analyze pipeline.
The whole chain is now ROOT 6; the old ROOT 5 split is gone. The `cpp11.patch`
(ROOT 6 / C++11 source fixes) is **already applied** in this checkout — do not
re-apply it.

To (re)build the binaries used day-to-day:

```bash
source ~/root_6_30_02/root-build/bin/thisroot.sh   # ROOT 6
source scl_source enable devtoolset-9               # gcc 9
make -j4 DataReader tracking
mkdir -p root_files                                 # DataReader writes pedestals/signals here
```

The compiled binaries (`DataReader`, `tracking`, …) and the `root_files/`
output directory are git-ignored, so a fresh clone of this repo has neither —
they must be rebuilt/created with the commands above before tracking will run.
(This is exactly what bit us once already: a fresh checkout had no binaries.)

`run_tracking_single.sh` calls `./DataReader` and `./tracking` with relative
paths, so it must be run from this directory — the DAQ processor handles this
via its `tracking_run_dir` setting. Importantly, `processor_watcher.py` now
launches the tracking subprocess with the ROOT 6 environment sourced (the same
`cpp_env` it uses for decode/analyze). Running the binaries in the machine's
default shell instead picks up the stale ROOT 5 at `/workspace/root` and fails
with a `libRIO.so` undefined-symbol error.

### Use this soft

* Datareader :

`Datareader <config_file> {ped,data,analyse,live,read}`

config_file : path to config file which contain the cosmic bench caracteristics and different file path  
ped : use this option to build signal file ped and corr branches, calculate the Ped.dat and RMS.dat using an existing signal file with filled raw branches  
data : use this option to build signal file ped and corr branches using existing Ped.dat and signal file with filled raw branches
analyse : use this option to build analyse file using existing Ped.dat and RMS.dat  
live : use this option to read online data from FeuUdpControl using option -q to build the raw branches of the signal file  
read : use this option to read electronic binary files and build the raw branches of the signal file

* Multicluster

`Multicluster <config_file> {multicluster,SoB,dispersion,display,hough} [additional options]`

config_file : path to config file which contain the cosmic bench caracteristics and different file path  
multicluster : use this option to build analyse file using existing Ped.dat and RMS.dat and signal file  
rawcluster : use this option to build analyse file using existing Ped.dat and RMS.dat and signal file with only raw branches filled  
SoB : use this option to display signal and noise amplitude  
dispersion : use this option to display some correlation between the detectors signal  
display : use this option to display the signal shape for each detector for a signal type (raw,ped or corr) from event 0 to n (n being the additional option)  
hough : use this option to study hough tracking style for event n (n being the additional option)
conv : use this option to study convolution clustering

* tracking

`tracking <config_file> {efficacity,eff2D,residus,fluxmap,tomoAbs,raypairs,srf,correlation,eventdisplay,eventdisplaymult,SoN,acceptance,watto} [additional options]`

config_file : path to config file which contain the cosmic bench caracteristics and different file path  
eff2D : compute 2D efficiency of 2D detectors (you need to switch is_ref to
false for both coordinates  
residus : compute efficiency, residuals and various alignement distribution for
the is_ref=false detectors  
fluxmap : compute muon flux for a given height in additional option  
raypairs : export deviation tomography rays in a file which name is given in
additional option  
correlation : show correlation between detectors/coordinates for time and
amplitude  
eventdisplay : show the detectors, cluster and reconstructed tracks for the
event number given in additional option  
eventdisplaymult : do the same as eventdisplay but scan from event 0 to the
given event

* wrapper

`wrapper <config_wrapper> [config_file]`

config_wrapper : path to config file which contain path to the pedestal run and the data run in order to output the cosmic rays root file  
config_file : use a custom config_file for the cosmic bench caracteristics (use config_default.json by default)  

### Extend this soft :

Add a new type of detector :

* Add your type to Tomogrphy::det_type enum
* update Tomography::Static_Detector map
* Implement your `<type>_Detector` class (must inherit from `Detector` class)
* Implement your `<type>_Cluster` (must inherit from `Cluster` class)
* Implement your `<type>_Event` (must inherit from `Event` class)
* Update the config file corresponding to your bench caracteristcs