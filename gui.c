/*
 *  OpenVPN-GUI -- A Windows GUI for OpenVPN.
 *
 *  Copyright (C) 2011 Michael Berger <michael.berger@gmx.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program (see the file COPYING included with this
 *  distribution); if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#define _WIN32_IE 0x0500

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <prsht.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>

#include "gui.h"

//
// WARNING:
// The following structures must NOT be DWORD padded because they are
// followed by strings, etc that do not have to be DWORD aligned.
//
#include <pshpack2.h>

typedef struct {
    WORD dlgVer;
    WORD signature;
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    WORD cDlgItems;
    short x;
    short y;
    short cx;
    short cy;

    // Everything else in this structure is variable length,
    // and therefore must be determined dynamically

    // sz_Or_Ord menu;               // name or ordinal of a menu resource
    // sz_Or_Ord windowClass;        // name or ordinal of a window class
    // WCHAR title[titleLen];        // title string of the dialog box
    // WORD pointsize;               // only if DS_SETFONT is set
    // WORD weight;                  // only if DS_SETFONT is set
    // BYTE italic;                  // only if DS_SETFONT is set
    // BYTE charset;                 // only if DS_SETFONT is set
    // WCHAR typeface[stringLen];    // only if DS_SETFONT is set
} DLGTEMPLATEEX;
typedef       DLGTEMPLATEEX *LPDLGTEMPLATEEX;
typedef CONST DLGTEMPLATEEX *LPCDLGTEMPLATEEX;

typedef struct {
    DWORD helpID;
    DWORD exStyle;
    DWORD style;
    short x;
    short y;
    short cx;
    short cy;
    DWORD id;

    // Everything else in this structure is variable length,
    // and therefore must be determined dynamically

    // sz_Or_Ord windowClass;    // name or ordinal of a window class
    // sz_Or_Ord title;          // initial text or resource identifier of the control
    // WORD extraCount;          // bytes of creation data that follow this member
} DLGITEMTEMPLATEEX;
typedef       DLGITEMTEMPLATEEX *LPDLGITEMTEMPLATEEX;
typedef CONST DLGITEMTEMPLATEEX *LPCDLGITEMTEMPLATEEX;

#include <poppack.h> // Resume normal packing


BOOL IsWindowsVistaOrLater()
{
    OSVERSIONINFO osvi;

    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

    GetVersionEx(&osvi);

    if (osvi.dwMajorVersion >= 6)
        return TRUE;
    else
        return FALSE;
}

BOOL IsDialogEx(LPCDLGTEMPLATE pdt)
{
    return ((LPCDLGTEMPLATEEX) pdt)->signature == 0xFFFF;
}

INT_PTR ExtendedDialogBoxIndirectParam(const HINSTANCE hInstance, const LPCDLGTEMPLATE hDialogTemplate, const HWND hWndParent, const DLGPROC lpDialogFunc, const LPARAM dwInitParam)
{
    INT_PTR nResult;
    LPCDLGTEMPLATE hNewDialogTemplate;

    // Windows Vista and above use Segoe UI 9 pt as default font while all the static dialog box templates
    // are using MS Shell Dlg 8 pt font which is fine for Windows 2000 and Windows XP. Therefore dynamically
    // build a new dialog box template for Windows Vista and later OS versions with Segoe UI 9 pt font.
    if (IsWindowsVistaOrLater())
    {
        if ((hNewDialogTemplate = CreateModifiedDialogTemplate(hDialogTemplate)))
        {
            nResult = DialogBoxIndirectParam(hInstance, hNewDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
            DeleteModifiedDialogTemplate(hNewDialogTemplate);
        }
        else
        {
            // Something went wrong so use the unmodified template as fallback
            nResult = DialogBoxIndirectParam(hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
        }
    }
    else
    {
        // No need to modify the dialog box template as we are running on Windows 2000 or Windows XP
        nResult = DialogBoxIndirectParam(hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
    }

    return nResult;
}

HWND ExtendedCreateDialogIndirectParam(const HINSTANCE hInstance, const LPCDLGTEMPLATE hDialogTemplate, const HWND hWndParent, const DLGPROC lpDialogFunc, const LPARAM dwInitParam)
{
    HWND hwndResult;
    LPCDLGTEMPLATE hNewDialogTemplate;

    // Windows Vista and above use Segoe UI 9 pt as default font while all the static dialog box templates
    // are using MS Shell Dlg 8 pt font which is fine for Windows 2000 and Windows XP. Therefore dynamically
    // build a new dialog box template for Windows Vista and later OS versions with Segoe UI 9 pt font.
    if (IsWindowsVistaOrLater())
    {
        if ((hNewDialogTemplate = CreateModifiedDialogTemplate(hDialogTemplate)))
        {
            hwndResult = CreateDialogIndirectParam(hInstance, hNewDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
            DeleteModifiedDialogTemplate(hNewDialogTemplate);
        }
        else
        {
            // Something went wrong so use the unmodified template as fallback
            hwndResult = CreateDialogIndirectParam(hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
        }
    }
    else
    {
        // No need to modify the dialog box template as we are running on Windows 2000 or Windows XP
        hwndResult = CreateDialogIndirectParam(hInstance, hDialogTemplate, hWndParent, lpDialogFunc, dwInitParam);
    }

    return hwndResult;
}

void DeleteModifiedDialogTemplate(LPCDLGTEMPLATE hDialogTemplate)
{
    HeapFree(GetProcessHeap(), 0, (LPVOID) hDialogTemplate);

    return;
}

LPCDLGTEMPLATE CreateModifiedDialogTemplate(LPCDLGTEMPLATE pdtSrc)
{
    DWORD dwSize;
    LPDLGTEMPLATE pdtDest = NULL;

    // Get the size of the supplied source dialog box template and allocate twice as much
    // space for the new dialog box template. This should be enough space to perform all
    // required modifications.
    dwSize = GetSizeOfDialogTemplate(pdtSrc);

    if ((pdtDest = (LPDLGTEMPLATE) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwSize * 2)))
    {
        LPBYTE pbSrc = (LPBYTE) pdtSrc, pbDest = (LPBYTE) pdtDest;
        WORD cItems;
        DWORD dwStyle;
        BOOL bDialogEx = IsDialogEx(pdtSrc);

        if (bDialogEx)
        {
            cItems = ((LPCDLGTEMPLATEEX) pdtSrc)->cDlgItems;
            dwStyle = ((LPCDLGTEMPLATEEX) pdtSrc)->style;

            // Copy the static part of the DLGTEMPLATEEX structure
            memcpy(pdtDest, pdtSrc, sizeof(DLGTEMPLATEEX));
            pbSrc = (LPBYTE) (((LPCDLGTEMPLATEEX) pdtSrc) + 1);
            pbDest = (LPBYTE) (((LPCDLGTEMPLATEEX) pdtDest) + 1);
        }
        else
        {
            cItems = pdtSrc->cdit;
            dwStyle = pdtSrc->style;

            // Copy the static part of the DLGTEMPLATE structure
            memcpy(pdtDest, pdtSrc, sizeof(DLGTEMPLATE));
            pbSrc = (LPBYTE) (pdtSrc + 1);
            pbDest = (LPBYTE) (pdtDest + 1);
        }

        CopyElement(&pbDest, &pbSrc);    // menu resource
        CopyElement(&pbDest, &pbSrc);    // window class
        CopyElement(&pbDest, &pbSrc);    // title

        // If the style specifies DS_SETFONT, there is a font information in the dialog template
        if (dwStyle & DS_SETFONT)
        {
            // Replace the font point size by our own value
            ReplaceFontPointSize(&pbDest, &pbSrc);

            // If this is a DLGTEMPLATEEX then there is also the font weight,
            // italic flag and charset which must be copied
            if (bDialogEx)
            {
                // WORD weight;
                // BYTE italic;
                // BYTE charset;
                memcpy(pbDest, pbSrc, sizeof(WORD) + (2 * sizeof(BYTE)));
                pbSrc += sizeof(WORD) + (2 * sizeof(BYTE));
                pbDest += sizeof(WORD) + (2 * sizeof(BYTE));
            }

            // Replace the font typeface name by our own value
            ReplaceFontTypeface(&pbDest, &pbSrc);
        }

        pbSrc = (LPBYTE) (((DWORD_PTR) pbSrc + 3) & ~3);      // DWORD align
        pbDest = (LPBYTE) (((DWORD_PTR) pbDest + 3) & ~3);    // DWORD align

        // Loop through the control of the dialog box template
        while (cItems--)
        {
            WORD cbCreationDataSize;

            if (bDialogEx)
            {
                // Copy the static part of the DLGITEMTEMPLATEEX structure
                memcpy(pbDest, pbSrc, sizeof(DLGITEMTEMPLATEEX));
                pbSrc += sizeof(DLGITEMTEMPLATEEX);
                pbDest += sizeof(DLGITEMTEMPLATEEX);
            }
            else
            {
                // Copy the static part of the DLGITEMTEMPLATE structure
                memcpy(pbDest, pbSrc, sizeof(DLGITEMTEMPLATE));
                pbSrc += sizeof(DLGITEMTEMPLATE);
                pbDest += sizeof(DLGITEMTEMPLATE);
            }

            CopyElement(&pbDest, &pbSrc);    // window class
            CopyElement(&pbDest, &pbSrc);    // title

            cbCreationDataSize = *((LPWORD) pbSrc);

            // Copy the WORD containing the size of the creation data
            memcpy(pbDest, pbSrc, sizeof(WORD));
            pbSrc += sizeof(WORD);
            pbDest += sizeof(WORD);

            if (cbCreationDataSize)
            {
                if (!bDialogEx)
                {
                    // In the case of a DLGTEMPLATE structure the creation data size includes
                    // the WORD containing the size so the actual data size is 2 bytes less
                    cbCreationDataSize -= 2;
                }

                // Copy the creation data
                memcpy(pbDest, pbSrc, cbCreationDataSize);
                pbSrc += cbCreationDataSize;
                pbDest += cbCreationDataSize;
            }

            pbSrc = (LPBYTE) (((DWORD_PTR) pbSrc + 3) & ~3);      // DWORD align
            pbDest = (LPBYTE) (((DWORD_PTR) pbDest + 3) & ~3);    // DWORD align
        }
    }

    return pdtDest;
}

DWORD GetSizeOfDialogTemplate(LPCDLGTEMPLATE pdt)
{
    WORD cItems;
    DWORD dwStyle;
    LPBYTE pb = (LPBYTE) pdt;
    BOOL bDialogEx = IsDialogEx(pdt);

    if (bDialogEx)
    {
        cItems = ((LPCDLGTEMPLATEEX) pdt)->cDlgItems;
        dwStyle = ((LPCDLGTEMPLATEEX) pdt)->style;

        // Skip the static part of the DLGTEMPLATEEX structure
        pb = (LPBYTE) (((LPCDLGTEMPLATEEX) pdt) + 1);
    }
    else
    {
        cItems = pdt->cdit;
        dwStyle = pdt->style;

        // Skip the static part of the DLGTEMPLATE structure
        pb = (LPBYTE) (pdt + 1);
    }

    SkipElement(&pb);    // menu resource
    SkipElement(&pb);    // window class
    SkipElement(&pb);    // title

    // If the style specifies DS_SETFONT, there is a font information in the dialog template
    if (dwStyle & DS_SETFONT)
    {
        // WORD pointsize
        pb += sizeof(WORD);

        // If this is a DLGTEMPLATEEX then there is also the font weight,
        // italic flag and charset which must be skipped
        if (bDialogEx)
        {
            // WORD weight;
            // BYTE italic;
            // BYTE charset;
            pb += sizeof(WORD) + sizeof(BYTE) + sizeof(BYTE);
        }

        // Skip the typeface name
        SkipElement(&pb);
    }

    pb = (LPBYTE) (((DWORD_PTR) pb + 3) & ~3);      // DWORD align

    // Loop through the control of the dialog box template
    while (cItems--)
    {
        WORD cbCreationDataSize;

        if (bDialogEx)
        {
            // Skip the static part of the DLGITEMTEMPLATEEX structure
            pb += sizeof(DLGITEMTEMPLATEEX);
        }
        else
        {
            // Skip the static part of the DLGITEMTEMPLATE structure
            pb += sizeof(DLGITEMTEMPLATE);
        }

        SkipElement(&pb);    // window class
        SkipElement(&pb);    // title

        cbCreationDataSize = *((LPWORD) pb);

        // Skip the WORD containing the size of the creation data
        pb += sizeof(WORD);

        if (cbCreationDataSize)
        {
            if (!bDialogEx)
            {
                // In the case of a DLGTEMPLATE structure the creation data size includes
                // the WORD containing the size so the actual data size is 2 bytes less
                cbCreationDataSize -= 2;
            }

            // Skip the creation data
            pb += cbCreationDataSize;
        }

        pb = (LPBYTE) (((DWORD_PTR) pb + 3) & ~3);    // DWORD align
    }

    // Return the template size
    return (DWORD) (pb - (LPBYTE) pdt);
}

void SkipElement(LPBYTE *ppb)
{
    // The first WORD of the element determines the following structure:
    // 0x0000 means no other data is following
    // 0xFFFF means the following WORD contains data
    // Everything else means the element is a NULL terminated Unicode string

    if (*(LPWORD) (*ppb) == 0xFFFF)
    {
        // Skip the first WORD of the element and the WORD containing data
        *ppb += sizeof(WORD) + sizeof(WORD);
        return;
    }

    // Skip Unicode string
    while (*(LPWORD) (*ppb) != 0)
    {
        *ppb += sizeof(WORD);
    }

    // Skip the first WORD of the element or NULL terminating the Unicode string
    *ppb += sizeof(WORD);

    return;
}

void CopyElement(LPBYTE *ppbDest, LPBYTE *ppbSrc)
{
    // The first WORD of the element determines the following structure:
    // 0x0000 means no other data is following
    // 0xFFFF means the following WORD contains data
    // Everything else means the element is a NULL terminated Unicode string

    if (*(LPWORD) (*ppbSrc) == 0xFFFF)
    {
        // Copy the first WORD of the element and the WORD containing data
        memcpy(*ppbDest, *ppbSrc, sizeof(WORD) + sizeof(WORD));
        *ppbSrc += sizeof(WORD) + sizeof(WORD);
        *ppbDest += sizeof(WORD) + sizeof(WORD);
        return;
    }

    // Copy Unicode string
    while (*(LPWORD) (*ppbSrc) != 0)
    {
        memcpy(*ppbDest, *ppbSrc, sizeof(WORD));
        *ppbSrc += sizeof(WORD);
        *ppbDest += sizeof(WORD);
    }

    // Copy the first WORD of the element or NULL terminating the Unicode string
    memcpy(*ppbDest, *ppbSrc, sizeof(WORD));
    *ppbSrc += sizeof(WORD);
    *ppbDest += sizeof(WORD);

    return;
}

void ReplaceFontPointSize(LPBYTE *ppbDest, LPBYTE *ppbSrc)
{
    // Use 9 point font size
    *(LPWORD) (*ppbDest) = 9;

    *ppbSrc += sizeof(WORD);
    *ppbDest += sizeof(WORD);

    return;
}

void ReplaceFontTypeface(LPBYTE *ppbDest, LPBYTE *ppbSrc)
{
    WCHAR typeface[] = L"Segoe UI";

    // Use Segoe UI font
    wcscpy((LPWSTR) *ppbDest, typeface);

    *ppbSrc += (lstrlenW((LPWSTR) *ppbSrc) + 1) * sizeof(WCHAR);
    *ppbDest += (lstrlenW(typeface) + 1) * sizeof(WCHAR);

    return;
}

void RemoveSysMenu(HWND hwndDlg)
{
    SendMessage(hwndDlg, WM_SETICON, (WPARAM) (ICON_BIG), NULL);
    SendMessage(hwndDlg, WM_SETICON, (WPARAM) (ICON_SMALL), NULL);

    return;
}

BOOL GetControlRect(const HWND hWnd, LPRECT lpRect)
{
    HWND hWndParent = GetParent(hWnd);
    POINT p = {0};

    // Get upper left position of the control within the window
    MapWindowPoints(hWnd, hWndParent, &p, 1);

    // Get size of the control
    if (GetClientRect(hWnd, lpRect))
    {
        // Add upper left offset to the control size for absolute
        // position within the window
        (*lpRect).left += p.x;
        (*lpRect).top += p.y;
        (*lpRect).right += p.x;
        (*lpRect).bottom += p.y;

        return TRUE;
    }

    return FALSE;
}
