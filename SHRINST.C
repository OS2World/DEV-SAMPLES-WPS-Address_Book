/*
 * OS/2 Work Place Shell Sample Program - Installation
 *
 * Copyright (C) 1993 IBM Corporation
 *
 *   DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
 *   sample code created by IBM Corporation.  This sample code is
 *   not part of any standard or IBM product and is provided to you
 *   solely for the purpose of assisting you in the development of
 *   your applications.  The code is provided "AS IS".  ALL
 *   WARRANTIES ARE EXPRESSLY DISCLAIMED, INCLUDING THE IMPLIED
 *   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *   PURPOSE.  IBM shall not be liable for any damages arising out
 *   of your use of the sample code, even if IBM has been advised of
 *   the possibility of such damages.
 */

#define INCL_ERRORS
#define INCL_WPCLASS
#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <string.h>
#include <stdio.h>

#include "shrinst.h"

CHAR vszMessageTitle[] = "Address Book - Installation";
CHAR vszSysDLLPath[CCHMAXPATH];

extern BOOL ShrInstall(PSZ pszDLL)
{
  CHAR szText[256];
  USHORT usResponse;
  APIRET rc;
  HMODULE hmod;
  PSZ psz;

  sprintf(szText, "Register Address Book DLL '%s'?", pszDLL);
  usResponse = WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szText, 
      vszMessageTitle, 0, MB_YESNO | MB_MOVEABLE | MB_ICONQUESTION);

  if (usResponse != MBID_YES)
    return FALSE;

  rc = DosLoadModule(NULL, 0, pszDLL, &hmod);

  if (rc == ERROR_FILE_NOT_FOUND)
  {
    sprintf(szText, 
        "Address Book DLL '%s' not found.", pszDLL);

    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szText,
       vszMessageTitle, 0, 
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }
  else if (rc == ERROR_PATH_NOT_FOUND)
  {
    psz = strrchr(pszDLL, '\\');
    *psz = '\0';

    sprintf(szText, "Path '%s' is not valid.", pszDLL);

    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szText,
       vszMessageTitle, 0, 
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }
  else if (rc == ERROR_BAD_EXE_FORMAT)
  {
    sprintf(szText, 
        "Address Book DLL '%s' is corrupted.  "
        "Verify you downloaded it correctly and try again.",
        pszDLL);

    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szText,
       vszMessageTitle, 0, 
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }
  else if (rc != 0)
  {
    sprintf(szText, 
        "DosLoadModule failed with a return code of %u.  "
        "Type HELP SYS%u at an OS/2 command prompt for "
        "a list of possible causes of the error.",
        rc, rc);
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szText,
       vszMessageTitle, 0, 
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }

  if (!WinRegisterObjectClass("ShrPerson", pszDLL))
  {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, 
       "Unable to register Person class.", vszMessageTitle, 0,
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }

  if (!WinRegisterObjectClass("ShrFolder", pszDLL))
  {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, 
       "Unable to register ShrFolder class.",
       vszMessageTitle, 0,
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }

  if (!WinRegisterObjectClass("ShrAddressBook", pszDLL))
  {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, 
       "Unable to register Address Book class.", 
       vszMessageTitle, 0,
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }

  if (!WinCreateObject("ShrAddressBook", "Address Book", " ",
      "<WP_DESKTOP>", CO_REPLACEIFEXISTS))
  {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, 
       "Unable to create Address Book.",
       vszMessageTitle, 0,
       MB_ENTER | MB_MOVEABLE | MB_ERROR);
    return FALSE;
  }
  else
  {
    WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, 
       "Address Book created.",
       vszMessageTitle, 0,
       MB_ENTER | MB_MOVEABLE);
  }

  return TRUE;
}

extern MRESULT EXPENTRY ShrInstallDlgProc
    (HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT mr = NULL;
  CHAR szDLL[CCHMAXPATH];
  CHAR szText[256];
  PSZ psz;
  HWND hwndEntryField;

  switch (msg)
  {
    case WM_INITDLG:
      hwndEntryField = WinWindowFromID(hwndDlg, ID_PATH);
      WinSetWindowText(hwndEntryField, (PSZ) mp2);
      mr = (MRESULT) WinSetFocus(HWND_DESKTOP, hwndEntryField);
      break;

    case WM_CLOSE:
      WinDismissDlg(hwndDlg, MBID_CANCEL);
      break;

    case WM_COMMAND:
      if (SHORT1FROMMP(mp1) == MBID_OK)
      {
        WinQueryDlgItemText(hwndDlg, ID_PATH, sizeof(szDLL), szDLL);
        if (strlen(szDLL) >= 2)
        {
          psz = strchr(szDLL, '\0') - 1;
          if (*psz == '\\')
            strcat(szDLL, "SHARE.DLL");
          else
            strcat(szDLL, "\\SHARE.DLL");

          strupr(szDLL);

          if (ShrInstall(szDLL))
            WinDismissDlg(hwndDlg, MBID_OK);
        }
        else
        {
          sprintf(szText, "The path must include the drive letter."
              "  For example, %s.  Please try again.", vszSysDLLPath);
          WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szText,
              vszMessageTitle, 0, 
              MB_ENTER | MB_MOVEABLE | MB_WARNING);
        }
      }
      else if (SHORT1FROMMP(mp1) == MBID_CANCEL)
      {
        WinDismissDlg(hwndDlg, MBID_CANCEL);
      }
      break;

    default:
      mr = WinDefDlgProc(hwndDlg, msg, mp1, mp2);
      break;
  }
  
  return mr;
}

extern int main(VOID);

int main()
{
  HMQ hmq;
  HAB hab;
  CHAR szPMWIN[CCHMAXPATH];
  HMODULE hmodPMWIN;
  PSZ psz;
  HWND hwndDlg;
  SWP swp;
 
  hab = WinInitialize(0);
  hmq = WinCreateMsgQueue(hab, 0);

  if (!DosLoadModule(NULL, 0, "PMWIN", &hmodPMWIN))
  {
    DosQueryModuleName(hmodPMWIN, sizeof(szPMWIN), szPMWIN);
    psz = strrchr(szPMWIN, '\\');
    *psz = '\0';

    strcpy(vszSysDLLPath, szPMWIN);
  }
 
  hwndDlg = WinLoadDlg(HWND_DESKTOP,
      NULLHANDLE, ShrInstallDlgProc,
      NULLHANDLE,
      IDD_INSTALL, vszSysDLLPath);

  WinQueryWindowPos(hwndDlg, &swp);
  swp.x = (WinQuerySysValue(HWND_DESKTOP, SV_CXSCREEN) - swp.cx) / 2;
  swp.y = (WinQuerySysValue(HWND_DESKTOP, SV_CYSCREEN) - swp.cy) / 2;
  swp.fl |= SWP_ACTIVATE | SWP_MOVE | SWP_ZORDER;
  swp.hwndInsertBehind = HWND_TOP;

  WinSetMultWindowPos(NULLHANDLE, &swp, 1);

  WinProcessDlg(hwndDlg);

  WinDestroyMsgQueue(hmq);
  WinTerminate(hab);

  return 0;
}
