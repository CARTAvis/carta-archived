#!/bin/bash

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

try echo -e "1\n1\n" | py.test -v --capture=no   tLoadImage.py::tLoadImage
try echo -e "1\n1\n" | py.test -v --capture=no   tWindow.py::tWindow
try echo -e "1\n1\n" | py.test -v --capture=no   tLayout.py::tLayout
#try echo -e "1\n1\n" | py.test -v --capture=no   tView.py::tView
try echo -e "1\n1\n" | py.test -v --capture=no   tMenuToolVisibility.py::tMenuToolVisibility
#try echo -e "1\n1\n" | py.test -v --capture=no   tAnimatorTapeDeck.py::tAnimatorTapeDeck
#try echo -e "1\n1\n" | py.test -v --capture=no   tAnimatorSettings.py::tAnimatorSettings
try echo -e "1\n1\n" | py.test -v --capture=no   tAnimatorLinks.py::tAnimatorLinks
try echo -e "1\n1\n" | py.test -v --capture=no   tAxis.py::tAxis
try echo -e "1\n1\n" | py.test -v --capture=no   tHistogram.py::tHistogram
#try echo -e "1\n1\n" | py.test -v --capture=no   tSnapshotData.py::tSnapshotData
try echo -e "1\n1\n" | py.test -v --capture=no   tSnapshotLayout.py::tSnapshotLayout
try echo -e "1\n1\n" | py.test -v --capture=no   tSnapshotPreferences.py::tSnapshotPreferences
try echo -e "1\n1\n" | py.test -v --capture=no   tContour.py::tContour
