# Image related functions are detailed here. 
import os
import time
import cv2
from cv2 import cv
import numpy as np
import pyautogui
import Image
from matplotlib import pyplot as plt

# Returns a cropped image of a specific location from the desktop
# screenshot. Mainly for usage to get textboxes for later image 
# analysis, namely getting textbox values.
def get_croppedImage(cropImage, matchCoords, saveImageName):
    # Crop the image according to the matching coordinates and save image
    img = cv2.imread( cropImage )
    MPx, MPx2, MPy, MPy2 = matchCoords
    # Crop and save the image
    cropped = img[ MPy:MPy2, MPx:MPx2 ]
    cv2.imwrite( saveImageName, cropped )
    # Allow image to be saved before continuing
    time.sleep(5)
    return cropped

# Locate the needle image from the current desktop screen. 
# First take a screenshot of the desktop, then perform image 
# matching and return the coordinates of the image (left corner).
def locateOnScreen(needleImg):
    # Take a screenshot image of the desktop and save it
    img = pyautogui.screenshot("desktop.png")
    # Allow image to be saved prior to continuing
    time.sleep(5)
    # Get the matching coordinates
    matchCoords = get_match_coordinates( needleImg, "desktop.png")

    return matchCoords

# Locates a needle image on the screen. First take a screenshot of the 
# desktop, then perform image matching and returns the (x, y) 
# coordinates of the center position where the image has been found
def locateCenterOnScreen(needleImg):
    # Take a screenshot image of the desktop and save it
        img = pyautogui.screenshot( "desktop.png")
        # Allow image to be saved prior to continuing
        time.sleep(5)
        # Get the matching coordinates
        matchCoords = get_match_coordinates( needleImg, "desktop.png")
        # If there are no matching coordinates, return nothing
        if matchCoords == None:
            return None
        else:
            coords = locateCenter( matchCoords )
        
        return coords

# Perform image matching to determine the location of the needle image
# and return the coordinates of the matching location
def get_match_coordinates(needleImg, haystackImg):
    # Specify the method for image matching
    method = cv.CV_TM_SQDIFF_NORMED
    imageName = needleImg
    
    # Allow open cv to read the needle and haystack images
    # We want to find the needle image in the haystack image
    needleImg = cv2.imread( needleImg )
    haystackImg = cv2.imread( haystackImg )
    
    # Perform image matching and save result
    result = cv2.matchTemplate( needleImg, haystackImg, method )
    minVal, maxVal, minLoc, maxLoc = cv2.minMaxLoc(result)
    #print imageName, minVal

    # Set a threshold for image match accuracy
    # Image match will otherwise always return the "best match"
    if minVal > 0.03: #0.05
        matchCoords = None
    else:
        # minLoc variable stores the upper left corner coordinates
        # where the needle image was found in the haystack image
        MPx, MPy = minLoc
        # Find the rectangular region where the needle image matches the 
        # haystack image and return coordinates of the match
        trows, tcols = needleImg.shape[:2]
        matchCoords = (MPx, MPx+tcols, MPy, MPy+trows)
        # Uncomment if you want to see rectangular region
        # Draw a rectangle around match region to check that the match region is correct 
        #cv2.rectangle(haystackImg, (MPx, MPy),(MPx+tcols, MPy+trows), (0,0,225),1)
        # Save the resulting image as a png image
        #cv2.imwrite( 'result.png', haystackImg )
        
    # print matchCoords
    return matchCoords

# Find the center of the coordinates 
def locateCenter(coords):
    MPx, MPx2, MPy, MPy2 = coords
    # Get the center location of the image match 
    xCoord = (MPx + MPx2)/2
    yCoord = (MPy + MPy2)/2

    return ( xCoord, yCoord )

# Find all instances of an image on the desktop and return the coordinates of all instances
def locateAllOnScreen(needleImg):
    # Take a screenshot image of the desktop and save it
    img = pyautogui.screenshot( "desktop.png")

    # Allow image to be saved prior to continuing
    time.sleep(5)
    
    # Allow opencv2 to read both images
    haystackImg = cv2.imread( "desktop.png")
    grayImg = cv2.cvtColor( haystackImg, cv2.COLOR_BGR2GRAY)
    needleImg = cv2.imread( needleImg ,0)
    width , height = needleImg.shape[::-1]
    
    # Use thresholding to find multiple matches
    res = cv2.matchTemplate( grayImg, needleImg, cv2.TM_CCOEFF_NORMED)
    threshold = 0.8
    matchCoords = np.where( res >= threshold)
    
    #Uncomment to view the rectangular match regions
    #for pt in zip(*matchCoords[::-1]):
    #    cv2.rectangle( haystackImg, pt, (pt[0] + width, pt[1] + height), (0,0,255), 2)
    
    # Save the resulting image as a png image
    #cv2.imwrite('result.png', haystackImg )

    return matchCoords

# Perform image preprocessing prior to image recognition. The image will
# be geometrically corrected, denoised, light-contrasted, and resolution corrected.
# This is to prevent the python OCR wrapper from incorrectly reading the image.
def image_preprocessing(imageName):
    # Open the screenshot png image as a grayscale image
    img = cv2.imread( imageName, 0 )

    # Record the dimensions of the image
    height, width = img.shape
    
    # Denoise the image
    img = cv2.fastNlMeansDenoising( img, 10, 10, 7, 21 )
    
    # We enlarge the image to the following dimensions: 200 pixels in height
    # Note that we resize images while maintaining the aspect ratio.
    if float( height ) != 100:
        baseheight = 100
        # Determine the percentage of the new basewidth from the original width 
        hpercent = ( baseheight / float( height ) )
        # Multiply the original height by the width percentage 
        wsize = int( (float(width * float(hpercent))) )
        # Resize the image based on the basewidth and new height
        img = cv2.resize(img, (int(wsize), int(baseheight)))
    
    # Perform binarization of the image (covert to black and white)
    ret,img = cv2.threshold(img,127,255,cv2.THRESH_BINARY)
    
    # Save and replace the old image with the new image
    cv2.imwrite( imageName, img)
    
    # Now sharpen the image and improve resolution with PIL 
    im = Image.open( imageName )
    
    # Save the image with higher resolution
    im.save(imageName, dpi=(600,600))
    
    # Wait for image to save
    time.sleep(10)

# Enlarge the image to a specific height and width
# Does not maintain the aspect ratio of the image
def image_enlarge(imageName, newHeight, newWidth, newImage):
    # Open the screenshot png image 
    img = cv2.imread( imageName )

    # Record the dimensions of the image
    height, width, depth = img.shape
    baseheight = newHeight
    width = newWidth
    
    # Resize the image based on the basewidth and new height
    img = cv2.resize(img, (int(width), int(baseheight)))
    
    # Save the image
    cv2.imwrite( newImage, img)