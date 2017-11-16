#!/bin/bash

if [ ! -d "DIRECTORY" ]; then
    git clone https://github.com/markccchiang/CARTA-Image-Samples.git
fi

./testPercentile CARTA-Image-Samples/aH.fits


