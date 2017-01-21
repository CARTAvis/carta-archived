import cartavis
import os
import time
from threading import Timer
import threading

class MoviePlayer:
    # TODO: add config.json path parameter
    def __init__(self, cartaPath, htmlPath, port, configPath = None):
        self.cartavisInstance = cartavis.Cartavis(cartaPath, htmlPath, port, configPath= configPath)
        self.imageViews = self.cartavisInstance.getImageViews()
        self.animatorViews = self.cartavisInstance.getAnimatorViews()
        self.playTimer = None
        self.currentIndex = 0
        self.totalChannels = 0

    # TODO: may need to use "setImage" to change fits image
    # TODO: it's better to get initial channel index, now just assume it always is 0
    def startPlay(self, filePath, tick=5):
        print("start to play animator movie")

        self.imageViews[0].loadFile(filePath)

        # TODO: check. possible workaround way,
        # since user can define lower, upper bound of animator?
        self.totalChannels = self.imageViews[0].getChannelCount()

        self.playTimer = ContinuousTimer(tick, self.loopChannel)
        self.playTimer.start()

    def loopChannel(self):
        # currentIndex
        # totalChannels
        self.currentIndex = self.currentIndex +1
        if self.currentIndex == self.totalChannels:
            self.currentIndex = 0

        print("go to next channel:", self.currentIndex)
        self.animatorViews[0].setChannel(self.currentIndex)

    def stop(self):
        print("stop animator movie")
        if self.playTimer is not None:
            self.playTimer.stop()
            self.playTimer = None

# http://stackoverflow.com/questions/12435211/python-threading-timer-repeat-function-every-n-seconds
class ContinuousTimer(threading.Thread):
    def __init__(self, seconds, task):
        threading.Thread.__init__(self)
        self.task = task
        self.seconds = seconds
        self.stopped = threading.Event()

    def stop(self):
        self.stopped.set()

    def run(self):
        while not self.stopped.wait(self.seconds):
            # call the task function
            self.task()

def task():
    print 'hello timer!'
    # timer thread
    print threading.current_thread().__class__.__name__

if __name__ == '__main__':
    print "a example to use ContinuousTimer"

    # main thread
    # print threading.current_thread().__class__.__name__
    # or print the id, threading.get_ident() works, or threading.current_thread()

    # timer = threading.Timer(5, tick)
    # timer.start()
    # timer.cancel()

    # Example Usage
    t = ContinuousTimer(1, task)
    t.start()
    time.sleep(5)
    t.stop()

    # when the main thread/process is closed, timer's function will no be execuated. so just keep this main thread live
    # while True:
    #     time.sleep(1)
    #     print 'main running'
    # or just use "join""
    # t.join()

    print "end to test ContinuousTimer."
