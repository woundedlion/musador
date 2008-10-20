#ifndef WINMENU_H_986987979879879
#define WINMENU_H_986987979879879

#include <windows.h>
#include <string>

namespace Musador
{

    class MenuItem
    {
    public:

        DWORD id;
        std::wstring text;
        bool sep;
        bool checked;
        bool enabled;
    };

    class WinMenu
    {
    public:

        WinMenu();

        ~WinMenu();

        void insertItem(UINT pos, WORD id, const std::wstring& text, bool enabled = true, bool checked = false);

        void insertSep(UINT pos, WORD id);

        void updateItem(WORD oldID, WORD newID, const std::wstring& text, bool enabled = true, bool checked = false);

        //		void removeItem(WORD id);

        operator HMENU();		

        //		void insertSubMenu(DWORD id, const std::wstring& text, WinMenu& subMenu, int pos);

        void popupAtCursor(HWND owner);

    private:

        HMENU hMenu;

    };

}

#endif