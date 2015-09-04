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

```shell
py.test -v --directory='/home/jeff/scratch/build/cpp/desktop/'
--executable='desktop' --configFile='/home/jeff/.cartavis/config.json'
--port=9999
--htmlFile='/home/jeff/dev/CARTAvis/Skeleton3/VFS/DesktopDevel/desktop/desktopIndex.html'
--imageFile='/scratch/Images/mexinputtest.fits'
```

# Sample pytest.ini file

A `pytest.ini` file in the `tests` directory can store default values for all
of the above options. I did not include this file in the repository because its
contents would be different for everyone who runs the tests, but I have
included a sample `pytest.ini` file below:

```ini
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

# Using Image Recognition

For image recognition, the following pre-requisites must be installed on your local machine:
(1) pyautogui - a Python wrapper to automate mouse clicking
(2) OpenCV 2.4.11.0 - library for image-related functions, including image recognition

Note: When running the tests, ensure that the desktop is not disturbed in any way. In particular, avoid 
covering the desktop application as it may interfere with Image recognition. Image recognition
can be error prone because it relies on taking a screenshot image of the desktop prior to performing image 
matching to find the image on the desktop. Similarly, avoid using the mouse during the test running 
duration to avoid conflict between pyautogui clicks and real mouse clicks.

Currently, we mainly use image recognition to target Animator tape deck buttons - play, stop, return to
the first value, etc. We also use image recognition to click on the image window, open Animator settings, 
restore a CasaImageLoader and click on the Animator window. Image matching will return None if the image 
cannot be found on the desktop, within a given threshold (the current threshold value is 0.03).

# Troubleshooting clicks, image recognition: 

(1) Use coords.py python script to verify location on the screen that you wish to click: script will continuously return the coordinates of mouse so you can record the location of the certain element you are targeting - this should only be used for double checking. The get_match_coordinates function in the ImageUtils module has print statements that will display the coordinates of the match - by uncommenting these statements, you can cross-check these coordinates with the coords.py script location of the element

(2) Another quick method of checking whether image recognition provided correct results is by uncommenting the image saving in the get_match_coordinates function in the ImageUtils module. This way, each time image matching is performed in the python script, the resulting image (with the match region outlined using a red rectangle) will be saved as 'result.png'. Note that the 'result.png' will be overwritten each time a new image match is performed using the function, so it is best to view the result.png file over the course of the testing script with targeting issues. 

# Image recognition Limitations

(1) Image recognition is prone to error. Adjusting the threshold of the get_matching_coordinates function in ImageUtil will result in more matches (and should theoretically provide the closest match); however, this can lead to an increase in incorrect targeting of the specific element.

(2) Changes in the GUI may be difficult to account for, and may require obtaining new sets of images so that the tests run accurately. 
