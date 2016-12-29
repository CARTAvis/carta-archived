# Allow user to select web browser/timeout
# Create global variables
global browser
global sleep

# Allow user selection of global variable
print("Please select a browser 1. Firefox  2. Google Chrome")
browser = int(input("Browser selection: "))

print("Timeouts? 1. Yes  2. No")
sleep = int(input("Timeouts: "))

def _getBrowser():
	return 2

def _getSleep():
	# if sleep == 1:
	# 	timeout = 4
	# else:
	# 	timeout = 0
	return 0
