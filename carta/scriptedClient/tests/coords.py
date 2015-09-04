# A simple python script that will continuously print the location of the mouse cursor
# Mainly used for the verification of coordinates during test script writing

#! python
import pyautogui, sys
print('Press Ctrl-C to quit.')
try:
    while True:
        x, y = pyautogui.position()
        positionStr = 'X: ' + str(x).rjust(4) + ' Y: ' + str(y).rjust(4)
        print positionStr,
        print '\b' * (len(positionStr) + 2),
        sys.stdout.flush()
except KeyboardInterrupt:
    print '\n'
