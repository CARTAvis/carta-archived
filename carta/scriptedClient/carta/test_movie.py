import movie
import time

if __name__ == '__main__':
    print "Start to test play movie."

    cartaPath = "/Users/grimmer/cartabuild/CARTAvis-1411/build/cpp/desktop/desktop.app/Contents/MacOS/desktop"
    htmlPath =  "/Users/grimmer/cartabuild/CARTAvis-1411/carta/VFS/DesktopDevel/desktop/desktopIndex.html"
    # port is easily already occupied by other apps. be careful
    port = 32999
    # (optional) configPath = "/Users/grimmer/.cartavis/config.json"

    moviePlayer = movie.MoviePlayer(cartaPath, htmlPath, port)

    filePath = '/Users/grimmer/CARTA/Images/cube_x220_z100_17MB.fits'

    moviePlayer.startPlay(filePath)
    time.sleep(25)
    moviePlayer.stop()

    print "end to test play movie."
