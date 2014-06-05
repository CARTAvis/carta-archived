Contents:

PureWebDevel

	html5 vfs that allows running the client portion of the
	application with debugging urned on, and direct links to
	sources, so that JavaScript error messages reference the
	actual source files

PureWebRelease

	link to html5 vfs that is to be deployed (i.e. no debugging,
	optimized JavaScript, etc)

DesktopDevel

	similar to PureWebDevel directory, but intended to be run
	by the desktop executable using file:/// URIs

DesktopRelease

	this is a layout that should allow us to recursively add all
	files in it to the QRC file that will be compiled into the
	release executable
