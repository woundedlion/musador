#ifndef CONSOLE_H_A5EBFB7D_37CE_43ef_BCDC_8F9058B6FA65
#define CONSOLE_H_A5EBFB7D_37CE_43ef_BCDC_8F9058B6FA65

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "windows.h"
#include "boost/function.hpp"

namespace Musador
{
    namespace UI
    {
        class Console
        {
        public:

            enum TextColor
            {
                COLOR_RED_LO = FOREGROUND_RED,
                COLOR_RED_HI = FOREGROUND_RED| FOREGROUND_INTENSITY,
                COLOR_GREEN_LO = FOREGROUND_GREEN,
                COLOR_GREEN_HI = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                COLOR_BLUE_LO = FOREGROUND_BLUE,
                COLOR_BLUE_HI = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                COLOR_YELLOW_LO = FOREGROUND_RED | FOREGROUND_GREEN,
                COLOR_YELLOW_HI = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
                COLOR_PURPLE_LO = FOREGROUND_RED | FOREGROUND_BLUE,
                COLOR_PURPLE_HI = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                COLOR_CYAN_LO = FOREGROUND_GREEN | FOREGROUND_BLUE,
                COLOR_CYAN_HI = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
                COLOR_WHITE_LO = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
                COLOR_WHITE_HI = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
            };

            typedef PHANDLER_ROUTINE SignalHandler;

            Console();

            ~Console();

            void revert();

            void setTextColor(TextColor color);

            void setSignalHandler(SignalHandler handler);

            BOOL ctrlHandler(DWORD ctrlType);

        private:

            HANDLE hCon;
            WORD oldColor;
            SignalHandler signalHandler;		
        };
    }
}

#endif
