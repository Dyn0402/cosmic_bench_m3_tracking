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

### Track reconstruction improvements (updated 2026-07-06)

Since the ZS fix, the track-reconstruction chain has been reworked ("Tracking
v2"). The changes below are all in `src/event.cpp`, `src/ray.cpp`,
`src/Tray.cpp` and `src/detector.cpp`; the ray output tree gained two branches.

**More events reconstructed — layer-drop rescue.**
`CosmicBenchEvent::get_absorption_rays` has a fallback pass that drops a
detector layer when a full-layer fit finds no ray. That fallback was dead code:
the inner loop flag was never reset between passes, so the `drop=1` rescue never
ran. Resetting it per pass recovers ~25% more events with rays (det3_test:
8407 → 10503 of 12976); rays that were already found are unchanged. The pass
also now guards against dereferencing `end()` when a layer has no
positive-amplitude cluster.

**Better quality flags — NClusX / NClusY branches.**
The rays tree (`Tray`) now writes `NClusX` and `NClusY`: the number of clusters
that entered each coordinate fit. ~35% of rays have a 2-point coordinate; those
are only ~38% within 5 mm of the DUT hit (vs ~85% for full fits) and most carry
a denormal-tiny — not exactly zero — chi2, so they were effectively invisible to
downstream cuts. **Recommended consumer selection: `NClusX>=3 && NClusY>=3 &&
chi2 < 5`.** On the 6-27 Saturday det3 run against the DUT this selection moved
reco_near 65.2% → 73.5%, reco_far 11.0% → 5.0%, within-5mm 85.6% → 93.6%
(at the cost of ~10% fewer rays).

**Better spatial resolution — charge-weighted centroid (MGv2).**
`MGv2_Event::MultiCluster` now weights the cluster centroid by the integrated
charge over the sample window (positive samples only) instead of the single peak
sample. `ClusAmpl` and `ClusMaxStripAmpl` keep the original peak-sample
definition; only the position weighting changed. Result: ~1–2% narrower
exclusive track residuals on det3_test.

**Latent bugfixes in ray fitting / QA.** Several long-standing bugs that
affected reconstruction and alignment were fixed (verified to leave the
det3_test 6-22 ray output bit-for-bit unchanged, i.e. they only corrected paths
that were silently wrong):

* `Ray::Ray()` never initialized the Y-coordinate members
  (`chiSquare_Y`/`slope_Y`/`Z_intercept_Y`) — the X members were set twice.
* `RayPair` copy ctor/assignment copied `delta_theta_x` from
  `other.delta_theta_y`.
* `Ray_2D::get_residu(Detector*)` had its TF1 parameter limits swapped
  (`[0]` is the slope, not the intercept, for `"[0]*x+[1]"`), which clamped the
  refit intercept to ±0.4 mm.
* `Detector::operator==` compared a detector's `get_n_in_tree()` with itself, so
  any two same-type detectors compared equal.
* `Makefile` now generates and includes header dependencies (`-MMD`); without
  them a header edit left stale objects with the old class layout (this once
  showed up as heap corruption via a `sizeof(Tray)` mismatch). After pulling
  these changes, do a clean rebuild once (`make clean && make -j4 …`).

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