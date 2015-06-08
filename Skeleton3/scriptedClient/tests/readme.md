# Running the tests

To run the tests, change to the `Skeleton3/scriptedClient/tests` directory and
run `py.test` with the following options:

* --directory
  * The directory where the Carta executable file is located.
  * e.g. `/home/username/scratch/build/cpp/desktop/`
* --executable
  * The name of the Carta executable file.
  * e.g. `desktop`
* --configFile
  * The full path of the cartavis config.json file.
  * e.g. /home/username/.cartavis/config.json
* --port
  * The port which will be used to send commands to C++ and receive results.
  * e.g. 9999
* --htmlFile
  * The full path of the desktopIndex.html file.
  * e.g. `/home/username/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html`
* --imageFile
  * The full path of a compatible image file to load.
  * e.g. `$HOME/CARTA/Images/imagetestimage.fits`

The `-v` option can also be included for more verbose output. So a sample run
might look like this:

`py.test -v --directory='/home/jeff/scratch/build/cpp/desktop/' --executable='desktop' --configFile='/home/jeff/.cartavis/config.json' --port=9999 --htmlFile='/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html' --imageFile='/scratch/Images/mexinputtest.fits'`

# Sample pytest.ini file

A `pytest.ini` file in the `tests` directory can store default values for all
of the above options. I did not include this file in the repository because its
contents would be different for everyone who runs the tests, but I have
included a sample `pytest.ini` file below:

```py
[pytest]
addopts = --directory='/home/jeff/scratch/build/cpp/desktop/'
    --executable='desktop'
    --configFile='/home/jeff/.cartavis/config.json'
    --port=9999
    --htmlFile='/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html'
    --imageFile='/scratch/Images/mexinputtest.fits'
```

Once the `pytest.ini` file has been set up, then the tests can be executed by
simply running `py.test` or `py.test -v`.
