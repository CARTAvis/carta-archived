This quick feasability test essentially shows a possible way for plugins to provide some mathy functions that can be called on casa::ImageInterface images.

It also shows how it would be possible to define new image formats inside plugins, while keeping the ability to call casa algorithms...

Possible implementation of the plugin system.

- main program has built-in support for casaFits
- main program has a function that relies on casaFits to do some math, using casacore functions
- plugins can provide a math function to execute on a simplified image representation
- converter provided to convert casa::ImageInterface<float> to this simplified image interface
- plugins can provide an image reading routine that returns a simplified image representation
- main program has the ability to conver simplified image representation to casa::imageInterface

Todo:

clean up the code a lot
let's see if we can get python working

