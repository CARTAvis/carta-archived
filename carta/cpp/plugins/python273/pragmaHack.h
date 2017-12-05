// a hack to suppress some warnings

#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wunused-function"

// and more hacks to supress numpy deprecated API warnings :)

// #define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
// it does not work (yet), because it looks like cython is actually using deprecated
// numpy APIs
