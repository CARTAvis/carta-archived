#include <sstream>
#include "ConsoleColors.h"

/*
The Color Code:     <ESC>[{attr};{fg};{bg}m

I'll explain the escape sequence to produce colors. The sequence to be printed or echoed to the terminal is

        <ESC>[{attr};{fg};{bg}m
The first character is ESC which has to be printed by pressing CTRL+V and then ESC on the Linux console or in xterm, konsole, kvt, etc. ("CTRL+V ESC" is also the way to embed an escape character in a document in vim.) Then {attr}, {fg}, {bg} have to be replaced with the correct value to get the corresponding effect. attr is the attribute like blinking or underlined etc.. fg and bg are foreground and background colors respectively. You don't have to put braces around the number. Just writing the number will suffice.

{attr} is one of following

        0	Reset All Attributes (return to normal mode)
        1	Bright (Usually turns on BOLD)
        2 	Dim
        3	Underline
        5	Blink
        7 	Reverse
        8	Hidden
{fg} is one of the following
        30	Black
        31	Red
        32	Green
        33	Yellow
        34	Blue
        35	Magenta
        36	Cyan
        37	White
{bg} is one of the following
        40	Black
        41	Red
        42	Green
        43	Yellow
        44	Blue
        45	Magenta
        46	Cyan
        47	White
*/
std::string ConsoleColors::attr(int c) {
    std::stringstream o;
    o << char(27) << "[" << c << "m";
    return o.str();
}

std::string ConsoleColors::black()   { return attr(30); }
std::string ConsoleColors::red()     { return attr(31); }
std::string ConsoleColors::green()   { return attr(32); }
std::string ConsoleColors::yellow()  { return attr(33); }
std::string ConsoleColors::blue()    { return attr(34); }
std::string ConsoleColors::magenta() { return attr(35); }
std::string ConsoleColors::cyan()    { return attr(36); }
std::string ConsoleColors::white()   { return attr(37); }

std::string ConsoleColors::blackBg()   { return attr(40); }
std::string ConsoleColors::redBg()     { return attr(41); }
std::string ConsoleColors::greenBg()   { return attr(42); }
std::string ConsoleColors::yellowBg()  { return attr(43); }
std::string ConsoleColors::blueBg()    { return attr(44); }
std::string ConsoleColors::magentaBg() { return attr(45); }
std::string ConsoleColors::cyanBg()    { return attr(46); }
std::string ConsoleColors::whiteBg()   { return attr(47); }


std::string ConsoleColors::bright()  { return attr(1); }
std::string ConsoleColors::dim()     { return attr(2); }


std::string ConsoleColors::reset() { return escape() + "[0m"; }
std::string ConsoleColors::resetln() { return reset() + "\n"; }
std::string ConsoleColors::escape() { char buff[2]; buff[0] = 27; buff[1] = 0; return buff; }

std::string ConsoleColors::error() { return redBg() + white() + bright(); }
std::string ConsoleColors::warning() { return magentaBg() + white() + bright(); }
std::string ConsoleColors::info() { return blueBg() + yellow() + bright(); }

