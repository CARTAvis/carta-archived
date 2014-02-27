#ifndef CONSOLECOLORS_H
#define CONSOLECOLORS_H

#include <string>

class ConsoleColors
{
public:
    static std::string black();
    static std::string red();
    static std::string green();
    static std::string yellow();
    static std::string blue();
    static std::string magenta();
    static std::string cyan();
    static std::string white();

    static std::string blackBg();
    static std::string redBg();
    static std::string greenBg();
    static std::string yellowBg();
    static std::string blueBg();
    static std::string magentaBg();
    static std::string cyanBg();
    static std::string whiteBg();


    static std::string bright();
    static std::string dim();

    static std::string reset();
    static std::string resetln();

    static std::string error();
    static std::string warning();
    static std::string info();
private:
    static std::string escape();
    static std::string attr( int);
};

#endif // CONSOLECOLORS_H
