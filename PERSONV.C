/*
 * OS/2 Work Place Shell Sample Program - ShrPerson view code
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
 *
 * The person object has one view, the "Settings" view with
 * an additional page, the "Info" page.
 */

#define SOM_NoTest

#include "share.h"
#include "person.h"
#include "personv.h"
#include "notify.h"

/*
 * ShrPersonInfoSetObject initializes the person "Info" page.
 */

extern MRESULT ShrPersonInfoSetObject
    (HWND hwndDlg, SOMAny *somSelf)
{
  CHAR szText[512];

  WinSetWindowULong(hwndDlg, QWL_USER, (ULONG) somSelf);

  ShrSetDlgMLEText(hwndDlg, ID_NAME, _wpQueryTitle(somSelf));

  _shrQueryAddress(somSelf, sizeof(szText), szText);
  ShrSetDlgEntryFieldText(hwndDlg, ID_ADDRESS, szText);

  _shrQueryCity(somSelf, sizeof(szText), szText);
  ShrSetDlgEntryFieldText(hwndDlg, ID_CITY, szText);

  _shrQueryState(somSelf, sizeof(szText), szText);
  ShrSetDlgEntryFieldText(hwndDlg, ID_STATE, szText);

  _shrQueryZipCode(somSelf, sizeof(szText), szText);
  ShrSetDlgEntryFieldText(hwndDlg, ID_ZIPCODE, szText);

  _shrQueryPhone(somSelf, sizeof(szText), szText);
  ShrSetDlgEntryFieldText(hwndDlg, ID_PHONE, szText);
  
  _shrAddInterestedWindow(_shrQueryNotifier(somSelf), hwndDlg);
  
  return (MRESULT) WinSetFocus(HWND_DESKTOP,
      WinWindowFromID(hwndDlg, ID_NAME));
}

/*
 * ShrPersonInfoDlgEntryFieldKillFocus is called when one
 * of the Info page's entry fields looses the focus.
 * The model (person object) is updated to the new value.
 */

extern VOID ShrPersonInfoDlgEntryFieldKillFocus
    (SOMAny *somSelf, USHORT id, HWND hwndEntryField)
{
  CHAR szText[512];

  if (WinSendMsg(hwndEntryField, EM_QUERYCHANGED, 0, 0))
  {
    WinQueryWindowText(hwndEntryField, sizeof(szText), szText);

    /*
     * Set the model string data.
     */

    if (id == ID_ADDRESS)
      _shrSetAddress(somSelf, szText);
    else if (id == ID_CITY)
      _shrSetCity(somSelf, szText);
    else if (id == ID_STATE)
      _shrSetState(somSelf, szText);
    else if (id == ID_ZIPCODE)
      _shrSetZipCode(somSelf, szText);
    else if (id == ID_PHONE)
      _shrSetPhone(somSelf, szText);
  }
}

/*
 * ShrPersonInfoDlgMLEKillFocus is called when one
 * of the Info page's multi-line entry fields looses the focus.
 * The model (person object) is updated to the new value.
 */

extern VOID ShrPersonInfoDlgMLEKillFocus
    (SOMAny *somSelf, USHORT id, HWND hwndMLE)
{
  ULONG ulOffset, ulCopy;
  CHAR szText[512];

  if (WinSendMsg(hwndMLE, MLM_QUERYCHANGED, 0, 0))
  {
    /*
     * Set the export buffer address and length.
     */

    WinSendMsg(hwndMLE, MLM_SETIMPORTEXPORT,
        MPFROMP(szText), MPFROMLONG(sizeof(szText)));
  
    /*
     * Export the MLE text, append a NULL character (since the
     * MLE doesn't).
     */

    ulOffset = 0;
    ulCopy = sizeof(szText) - 1;
    ulCopy = (ULONG) WinSendMsg(hwndMLE, MLM_EXPORT,
        &ulOffset, &ulCopy);
    szText[ulCopy] = NULLCHAR;

    /*
     * Set the model string data.
     */

    if (id == ID_NAME)
      _wpSetTitle(somSelf, szText);
  }
}

/*
 * ShrPersonInfoPageDlgProc is the dialog procedure
 * for the Info dialog when it is displayed as a page
 * in the notebook of the Settings view.
 */

extern MRESULT EXPENTRY ShrPersonInfoPageDlgProc
    (HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT mr = NULL;
  SOMAny *somSelf;

  switch (msg)
  {
    case WM_INITDLG:
      mr = WinSendMsg(hwndDlg, SHR_SETOBJECT, mp2, NULL);
      break;

    case SHR_QUERYOBJECT:
      mr = (MRESULT) WinQueryWindowULong(hwndDlg, QWL_USER);
      break;

    case SHR_SETOBJECT:
      mr = ShrPersonInfoSetObject(hwndDlg, (SOMAny *) mp1);
      break;

    case WM_CONTROL:
      somSelf = (SOMAny *) WinSendMsg(hwndDlg, 
          SHR_QUERYOBJECT, NULL, NULL);

      switch (SHORT1FROMMP(mp1))
      {
        case ID_NAME:
          if (SHORT2FROMMP(mp1) == MLN_KILLFOCUS)
            ShrPersonInfoDlgMLEKillFocus(somSelf,
                SHORT1FROMMP(mp1), 
                WinWindowFromID(hwndDlg, SHORT1FROMMP(mp1)));
          break;

        case ID_ADDRESS:
        case ID_CITY:
        case ID_STATE:
        case ID_ZIPCODE:
        case ID_PHONE:
          if (SHORT2FROMMP(mp1) == EN_KILLFOCUS)
            ShrPersonInfoDlgEntryFieldKillFocus(somSelf,
                SHORT1FROMMP(mp1), HWNDFROMMP(mp2));
          break;
      }
      break;

    case SHRN_PERSONNAMECHANGED:
      ShrSetDlgMLEText(hwndDlg, ID_NAME, (PSZ) mp2);
      break;

    case SHRN_PERSONADDRESSCHANGED:
      ShrSetDlgEntryFieldText(hwndDlg, ID_ADDRESS, (PSZ) mp2);
      break;

    case SHRN_PERSONCITYCHANGED:
      ShrSetDlgEntryFieldText(hwndDlg, ID_CITY, (PSZ) mp2);
      break;

    case SHRN_PERSONSTATECHANGED:
      ShrSetDlgEntryFieldText(hwndDlg, ID_STATE, (PSZ) mp2);
      break;

    case SHRN_PERSONZIPCODECHANGED:
      ShrSetDlgEntryFieldText(hwndDlg, ID_ZIPCODE, (PSZ) mp2);
      break;

    case SHRN_PERSONPHONECHANGED:
      ShrSetDlgEntryFieldText(hwndDlg, ID_PHONE, (PSZ) mp2);
      break;

    case WM_DESTROY:
      somSelf = (SOMAny *) WinSendMsg(hwndDlg, 
          SHR_QUERYOBJECT, NULL, NULL);

      if (somSelf)
        _shrRemoveInterestedWindow(_shrQueryNotifier(somSelf), hwndDlg);

      mr = ShrDefPageDlgProc(hwndDlg, msg, mp1, mp2);
      break;

    default:
      mr = ShrDefPageDlgProc(hwndDlg, msg, mp1, mp2);
      break;
  }
  
  return mr;
}
