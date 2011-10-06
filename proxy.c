/*
 *  OpenVPN-GUI -- A Windows GUI for OpenVPN.
 *
 *  Copyright (C) 2004 Mathias Sundman <mathias@nilings.se>
 *                2011 Heiko Hund <heikoh@users.sf.net>
 *                2011 Michael Berger <michael.berger@gmx.de>
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

#define WINVER 0x0500

#include <windows.h>
#include <prsht.h>
#include <tchar.h>
#include <wininet.h>

#include "config.h"
#include "main.h"
#include "options.h"
#include "registry.h"
#include "proxy.h"
#include "openvpn-gui-res.h"
#include "localization.h"
#include "manage.h"
#include "openvpn.h"

extern options_t o;

INT_PTR CALLBACK
ProxySettingsDialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, UNUSED LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        /* Limit Port editboxes to 5 chars. */
        SendMessage(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), EM_SETLIMITTEXT, 5, 0);
        SendMessage(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), EM_SETLIMITTEXT, 5, 0);

        LoadProxySettings(hwndDlg);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_RB_PROXY_OPENVPN:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EnableWindow(GetDlgItem(hwndDlg, ID_RB_PROXY_HTTP), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_RB_PROXY_SOCKS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_PORT), FALSE);
            }
            break;

        case ID_RB_PROXY_MSIE:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EnableWindow(GetDlgItem(hwndDlg, ID_RB_PROXY_HTTP), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_RB_PROXY_SOCKS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_PORT), FALSE);
            }
            break;

        case ID_RB_PROXY_MANUAL:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EnableWindow(GetDlgItem(hwndDlg, ID_RB_PROXY_HTTP), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_RB_PROXY_SOCKS), TRUE);

                if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_HTTP) == BST_CHECKED)
                {
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_ADDRESS), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_PORT), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_ADDRESS), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_PORT), FALSE);
                }
                else
                {
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_ADDRESS), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_PORT), FALSE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_ADDRESS), TRUE);
                    EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_PORT), TRUE);
                }
            }
            break;

        case ID_RB_PROXY_HTTP:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_ADDRESS), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_PORT), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_PORT), FALSE);
            }
            break;

        case ID_RB_PROXY_SOCKS:
            if (HIWORD(wParam) == BN_CLICKED)
            {
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_HTTP_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_ADDRESS), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_HTTP_PORT), FALSE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_EDT_PROXY_SOCKS_PORT), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_ADDRESS), TRUE);
                EnableWindow(GetDlgItem(hwndDlg, ID_TXT_PROXY_SOCKS_PORT), TRUE);
            }
            break;
        }
        break;

    case WM_NOTIFY:
        switch ((int)((NMHDR FAR *)lParam)->code)
        {
        case PSN_KILLACTIVE:
            SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, (CheckProxySettings(hwndDlg) ? FALSE : TRUE));
            return TRUE;

        case PSN_APPLY:
            SaveProxySettings(hwndDlg);
            SetWindowLongPtr(hwndDlg, DWLP_MSGRESULT, PSNRET_NOERROR);
            return TRUE;

        case PSN_RESET:
            SetWindowLong(hwndDlg, DWL_MSGRESULT, FALSE);
            return TRUE;
        }
        break;
    }
    return FALSE;
}


/* Check that proxy settings are valid */
int
CheckProxySettings(HWND hwndDlg)
{
    TCHAR text[100];

    if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_MANUAL) == BST_CHECKED)
    {
        if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_HTTP) == BST_CHECKED)
        {
            GetDlgItemText(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS, text, _tsizeof(text));
            if (_tcslen(text) == 0)
            {
                /* proxy address not specified */
                ShowLocalizedMsg(IDS_ERR_HTTP_PROXY_ADDRESS);
                return 0;
            }
            GetDlgItemText(hwndDlg, ID_EDT_PROXY_HTTP_PORT, text, _tsizeof(text));
            if (_tcslen(text) == 0)
            {
                /* proxy port not specified */
                ShowLocalizedMsg(IDS_ERR_HTTP_PROXY_PORT);
                return 0;
            }
            else
            {
                long port = _tcstol(text, NULL, 10);

                if ((port < 1) || (port > 65535))
                {
                    /* proxy port range error */
                    ShowLocalizedMsg(IDS_ERR_HTTP_PROXY_PORT_RANGE);
                    return 0;
                }
            }
        }
        if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_SOCKS) == BST_CHECKED)
        {
            GetDlgItemText(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS, text, _tsizeof(text));
            if (_tcslen(text) == 0)
            {
                /* proxy address not specified */
                ShowLocalizedMsg(IDS_ERR_SOCKS_PROXY_ADDRESS);
                return 0;
            }
            GetDlgItemText(hwndDlg, ID_EDT_PROXY_SOCKS_PORT, text, _tsizeof(text));
            if (_tcslen(text) == 0)
            {
                /* proxy port not specified */
                ShowLocalizedMsg(IDS_ERR_SOCKS_PROXY_PORT);
                return 0;
            }
            else
            {
                long port = _tcstol(text, NULL, 10);

                if ((port < 1) || (port > 65535))
                {
                    /* proxy port range error */
                    ShowLocalizedMsg(IDS_ERR_SOCKS_PROXY_PORT_RANGE);
                    return 0;
                }
            }
        }
    }

    return 1;
}


void
LoadProxySettings(HWND hwndDlg)
{
    /* Set proxy type */
    if (o.proxy_type == http)
    {
        SendMessage(GetDlgItem(hwndDlg, ID_RB_PROXY_HTTP), BM_CLICK, 0, 0);
    }
    else if (o.proxy_type == socks)
    {
        SendMessage(GetDlgItem(hwndDlg, ID_RB_PROXY_SOCKS), BM_CLICK, 0, 0);
    }

    /* Set proxy settings source */
    if (o.proxy_source == config)
    {
        SendMessage(GetDlgItem(hwndDlg, ID_RB_PROXY_OPENVPN), BM_CLICK, 0, 0);
    }
    else if (o.proxy_source == windows)
    {
        SendMessage(GetDlgItem(hwndDlg, ID_RB_PROXY_MSIE), BM_CLICK, 0, 0);
    }
    else if (o.proxy_source == manual)
    {
        SendMessage(GetDlgItem(hwndDlg, ID_RB_PROXY_MANUAL), BM_CLICK, 0, 0);
    }

    /* Set proxy adresses and ports */
    SetDlgItemText(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS, o.proxy_http_address);
    SetDlgItemText(hwndDlg, ID_EDT_PROXY_HTTP_PORT, o.proxy_http_port);
    SetDlgItemText(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS, o.proxy_socks_address);
    SetDlgItemText(hwndDlg, ID_EDT_PROXY_SOCKS_PORT, o.proxy_socks_port);
}


void
SaveProxySettings(HWND hwndDlg)
{
    HKEY regkey;
    DWORD dwDispos;
    TCHAR proxy_source_string[2] = _T("0");
    TCHAR proxy_type_string[2] = _T("0");

    /* Save Proxy Settings Source */
    if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_OPENVPN) == BST_CHECKED)
    {
        o.proxy_source = config;
        proxy_source_string[0] = _T('0');
    }
    else if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_MSIE) == BST_CHECKED)
    {
        o.proxy_source = windows;
        proxy_source_string[0] = _T('1');
    }
    else if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_MANUAL) == BST_CHECKED)
    {
        o.proxy_source = manual;
        proxy_source_string[0] = _T('2');
    }

    /* Save Proxy type, address and port */
    if (IsDlgButtonChecked(hwndDlg, ID_RB_PROXY_HTTP) == BST_CHECKED)
    {
        o.proxy_type = http;
        proxy_type_string[0] = _T('0');

        GetDlgItemText(hwndDlg, ID_EDT_PROXY_HTTP_ADDRESS, o.proxy_http_address,
                    _tsizeof(o.proxy_http_address));
        GetDlgItemText(hwndDlg, ID_EDT_PROXY_HTTP_PORT, o.proxy_http_port,
                    _tsizeof(o.proxy_http_port));
    }
    else
    {
        o.proxy_type = socks;
        proxy_type_string[0] = _T('1');

        GetDlgItemText(hwndDlg, ID_EDT_PROXY_SOCKS_ADDRESS, o.proxy_socks_address,
                    _tsizeof(o.proxy_socks_address));
        GetDlgItemText(hwndDlg, ID_EDT_PROXY_SOCKS_PORT, o.proxy_socks_port,
                    _tsizeof(o.proxy_socks_port));
    }

    /* Open Registry for writing */
    if (RegCreateKeyEx(HKEY_CURRENT_USER, GUI_REGKEY_HKCU, 0, _T(""), REG_OPTION_NON_VOLATILE,
                       KEY_WRITE, NULL, &regkey, &dwDispos) != ERROR_SUCCESS)
    {
        /* error creating Registry-Key */
        ShowLocalizedMsg(IDS_ERR_CREATE_REG_HKCU_KEY, GUI_REGKEY_HKCU);
        return;
    }

    /* Save Settings to registry */
    SetRegistryValue(regkey, _T("proxy_source"), proxy_source_string);
    SetRegistryValue(regkey, _T("proxy_type"), proxy_type_string);
    SetRegistryValue(regkey, _T("proxy_http_address"), o.proxy_http_address);
    SetRegistryValue(regkey, _T("proxy_http_port"), o.proxy_http_port);
    SetRegistryValue(regkey, _T("proxy_socks_address"), o.proxy_socks_address);
    SetRegistryValue(regkey, _T("proxy_socks_port"), o.proxy_socks_port);

    RegCloseKey(regkey);
}


void
GetProxyRegistrySettings()
{
    LONG status;
    HKEY regkey;
    TCHAR proxy_source_string[2] = _T("0");
    TCHAR proxy_type_string[2] = _T("0");

    /* Open Registry for reading */
    status = RegOpenKeyEx(HKEY_CURRENT_USER, GUI_REGKEY_HKCU, 0, KEY_READ, &regkey);
    if (status != ERROR_SUCCESS)
        return;

    /* get registry settings */
    GetRegistryValue(regkey, _T("proxy_http_address"), o.proxy_http_address, _tsizeof(o.proxy_http_address));
    GetRegistryValue(regkey, _T("proxy_http_port"), o.proxy_http_port, _tsizeof(o.proxy_http_port));
    GetRegistryValue(regkey, _T("proxy_socks_address"), o.proxy_socks_address, _tsizeof(o.proxy_socks_address));
    GetRegistryValue(regkey, _T("proxy_socks_port"), o.proxy_socks_port, _tsizeof(o.proxy_socks_port));
    GetRegistryValue(regkey, _T("proxy_source"), proxy_source_string, _tsizeof(proxy_source_string));
    GetRegistryValue(regkey, _T("proxy_type"), proxy_type_string, _tsizeof(proxy_type_string));

    if (proxy_source_string[0] == _T('0'))
    {
        o.proxy_source = config;
    }
    else if (proxy_source_string[0] == _T('1'))
    {
        o.proxy_source = windows;
    }
    else if (proxy_source_string[0] == _T('2'))
    {
        o.proxy_source = manual;
    }

    o.proxy_type = (proxy_type_string[0] == _T('0') ? http : socks);

    RegCloseKey(regkey);
}


INT_PTR CALLBACK
ProxyAuthDialogFunc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    connection_t *c;
    TCHAR buf[50];
    char cmd[70] = "username \"HTTP Proxy\" \"";
    UINT username_len;
    int length;

    switch (msg)
    {
    case WM_INITDIALOG:
        /* Set connection for this dialog */
        SetProp(hwndDlg, cfgProp, (HANDLE) lParam);
        SetForegroundWindow(hwndDlg);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
            c = (connection_t *) GetProp(hwndDlg, cfgProp);
            username_len = GetDlgItemText(hwndDlg, ID_EDT_PROXY_USER, buf, _tsizeof(buf));
            if (username_len == 0)
                return TRUE;
            length = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, buf, -1, cmd + 23, sizeof(cmd) - 23, "_", NULL);
            memcpy(cmd + length + 22, "\"\0", 2);
            ManagementCommand(c, cmd, NULL, regular);

            memcpy(cmd, "password", 8);
            GetDlgItemText(hwndDlg, ID_EDT_PROXY_PASS, buf, _tsizeof(buf));
            length = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, buf, -1, cmd + 23, sizeof(cmd) - 23, "_", NULL);
            memcpy(cmd + length + 22, "\"\0", 2);
            ManagementCommand(c, cmd, NULL, regular);

            /* Clear buffers */
            memset(buf, 'x', sizeof(buf));
            buf[_tsizeof(buf) - 1] = _T('\0');
            SetDlgItemText(hwndDlg, ID_EDT_PROXY_USER, buf);
            SetDlgItemText(hwndDlg, ID_EDT_PROXY_PASS, buf);

            EndDialog(hwndDlg, LOWORD(wParam));
            return TRUE;
        }
        break;

    case WM_CLOSE:
        EndDialog(hwndDlg, LOWORD(wParam));
        return TRUE;

    case WM_NCDESTROY:
        RemoveProp(hwndDlg, cfgProp);
        break;
    }
    return FALSE;
}


/*
 * Construct the proxy options to append to the cmd-line.
 */
void ConstructProxyCmdLine(TCHAR *proxy_string, unsigned int size)
{
    // Clear proxy string
    __sntprintf_0(proxy_string, size, _T(""));

    if (o.proxy_source == manual)
    {
        if (o.proxy_type == http)
        {
            __sntprintf_0(proxy_string, size, _T(" --http-proxy %s %s auto"),
                          o.proxy_http_address, o.proxy_http_port);
        }
        else if (o.proxy_type == socks)
        {
            __sntprintf_0(proxy_string, size, _T(" --socks-proxy %s %s"),
                          o.proxy_socks_address, o.proxy_socks_port);
        }
    }
    else if (o.proxy_source == windows)
    {
        __sntprintf_0(proxy_string, size, _T(" --auto-proxy"));
    }
}
