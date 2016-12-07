
This is the simple cmake but it can work.

I will rewrite it later to make it more elegant

You can continue to ues qmake.

1. install all third party libraries in the 'ThirdParty'

2. mkdir build && cd build

3. cmake -DPREFIX=${INSTALL_PATH} ${CARTA_SOURCE_ROOT}/carta

4. make -j 8

5. make install

6. cd ${INSTALL_PATH}/bin

7. source carta-vars/sh

8. ./desktop --html ${HTML_PATH}

