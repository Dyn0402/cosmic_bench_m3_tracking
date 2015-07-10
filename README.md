### Use this soft

* Datareader :

`Datareader <config_file> {ped,data,analyse}`

config_file : path to config file which contain the cosmic bench caracteristics and different file path

ped : use this option to build signal file with raw, ped and corr branches, and calculate the Ped.dat and RMS.dat

data : use this option to build signal file with raw, ped and corr branches using existing Ped.dat

analyse : use this option to build analyse file using existing Ped.dat and RMS.dat

* Multicluster

`Multicluster <config_file> {multicluster,SoB,dispersion,display,hough}`

config_file : path to config file which contain the cosmic bench caracteristics and different file path

multicluster : use this option to build analyse file using existing Ped.dat and RMS.dat and signal file

SoB : use this option to display signal and noise amplitude

dispersion : use this option to display some correlation between the detectors signal

display : use this option to display the signal shpape for each detector

hough : use this option to study hough tracking style


### Extend this soft :

Add a new type of detector :

* Add your type to Tomogrphy::det_type enum
* update Tomography::operator<<(ostream,det_type) function
* update Tomography::Static_Detector map
* Implement your <type>_Detector class (must inherit from Detector class)
* Implement your <type>_Cluster (must inherit from Cluster class)
* Implement your <type>_Event (must inherit from Event class)
* Update T{signal,analyse}_{R,W} storage classes to use ROOT disk I/O (in particular constructor, destructor, init, fill and get methods)
* Update the config file corresponding to your bench caracteristcs