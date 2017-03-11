useful scripts

fabfile.py
==========

is a fabric fab file for deploying to almatest.cyberska.org

To use it you will have to install fabric, i.e. without root, you can:

pip install --user fabric

then you can run it from the root of your build directory like so:

cd <wherever you built carta>
python -m fabric -f ~/Work/ALMAvis/Skeleton3/scripts/fabfile.py deploy

