#include "Menu.h"

#include "Logger/Logger.h"
#define LOG_SENDER L"GUI"
using namespace Musador;

using namespace Musador::UI;

Menu::Menu() :
hMenu(::CreatePopupMenu())
{
}

Menu::~Menu()
{
    ::DestroyMenu(this->hMenu);
}

Menu::operator HMENU()
{
    return this->hMenu;
}

void
Menu::insertItem(UINT pos, WORD id, const std::wstring& text, bool enabled /* = true */, bool checked /* = false */)
{
    MENUITEMINFO item;
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE;
    item.wID = id;
    item.dwTypeData = const_cast<wchar_t *>(text.c_str());
    item.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);

    if (!InsertMenuItem(this->hMenu, pos, TRUE, &item))
    {
        LOG(Error) << "InsertMenuItem failed: " << ::GetLastError();
    }
}

void 
Menu::updateItem(WORD oldID, WORD newID, const std::wstring& text, bool enabled /* = true */, bool checked /* = false */)
{
    MENUITEMINFO item;
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_ID | MIIM_STRING | MIIM_STATE;
    item.wID = newID;
    item.dwTypeData = const_cast<wchar_t *>(text.c_str());
    item.fState = (enabled ? MFS_ENABLED : MFS_DISABLED) | (checked ? MFS_CHECKED : MFS_UNCHECKED);

    if (!::SetMenuItemInfo(this->hMenu, oldID, false, &item))
    {
        LOG(Error) << "SetMenuItem failed: " << ::GetLastError();
    }
}

void
Menu::insertSep(UINT pos, WORD id)
{
    MENUITEMINFO item;
    item.cbSize = sizeof(MENUITEMINFO);
    item.fMask = MIIM_ID | MIIM_FTYPE;
    item.wID = id;
    item.fType = MFT_SEPARATOR;

    if (!InsertMenuItem(this->hMenu, pos, TRUE, &item))
    {
        LOG(Error) << "InsertMenuItem failed: " << ::GetLastError();
    }
}

void
Menu::popupAtCursor(HWND owner)
{
    POINT p;
    ::GetCursorPos(&p);
    ::SetForegroundWindow(owner);
    ::TrackPopupMenu(this->hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN, p.x, p.y, 0, owner, NULL);
    ::PostMessage(owner,  WM_NULL, 0, 0);
}