# Gamma-ray pixel noise simulator using Geant4 
This is source code for the following publication.

>Ren Komatsu, Hanwool Woo, Yusuke Tamura, Atsushi Yamashita and Hajime Asama, "Gamma-ray Image Noise Generation Using Energy-Image Converter Based on Image Histogram", Proceedings of the 2021 IEEE/SICE International Symposium on System Integration (SII2021), 2021.


## Requirements
C++14 or newer
- [Geant4](https://geant4.web.cern.ch/)

Python3.6 or newer
- Numpy
- OpenCV
- Jupyter notebook

## How to run
This source code is written based on a simuation where 1.0 PBq of Co60 is placed as a radioactive source. You need to adjust some settings to use different radioactive sources.

Gamma-ray simulation using Geant4.
```bash
git clone https://github.com/matsuren/pixel_noise_sim_geant4.git
cd pixel_noise_sim_geant4
mkdir build && cd build && cmake ..
make -j
./Application_Main 10782150
```

Generate gamma-ray noise images.
```bash
cd notebook
jupyter notebook
```
Run `parameter_estimation` notebook to obtain optimized conversion paramters.

After that, run `gamma_noise_image_convert` notebook to generate gamma-ray noise images using optimized parameters.

