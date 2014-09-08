/*
 * OS/2 Work Place Shell Sample Program - Common functions, 
 *     DLL initialization, and global variable initialization.
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

#define SOM_NoTest

#include "share.h"
#include "person.h"
#include "addrbk.h"
#include "addrbkv.h"
#include "notify.h"
#include "somlist.h"
#include "shrfoldr.h"

/*
 * Read-only global strings.
 */

CHAR vszShrPersonClass[] = "ShrPerson";
CHAR vszShrAddressBookClass[] = "ShrAddressBook";
CHAR vszAddressNotebook[] = "ShrAddressNotebook";
CHAR vszNullString[1] = "";
CHAR vszDesktopObjectId[] = "<WP_DESKTOP>";
CHAR vszShrUnderstandsProtocol[] = "shrUnderstandsProtocol";
CHAR vszDRMObject[] = "DRM_OBJECT";
CHAR vszDRFObject[] = "DRF_OBJECT";
CHAR vszWcFrameCnrOwner[] = "ShrFrameCnrOwner";
CHAR vszwpModifyPopupMenu[] = "wpModifyPopupMenu";
CHAR vszAppName[] = "ShrExample";
CHAR vszIndexedViewWindowPosKeyName[] = "AddressBook:WindowPos";

/*
 * Global MRI-related strings.
 */

CHAR vszAddressBookTitle[CCHMINSTRING];
CHAR vszPersonTitle[CCHMINSTRING];
CHAR vszPersonInfoTab[CCHMINSTRING];
CHAR vaszIndexTabs[CINDEXTABS][CCHMINSTRING];
CHAR vaszIndexChars[CINDEXCHARS][CCHMINSTRING];
CHAR vszAllIndexChars[CCHMINSTRING];
CHAR vszAddressBookViewTitle[CCHMINSTRING];
CHAR vszAddressBookCnrTitle[CCHMINSTRING];
CHAR vszPersonNewTitle[CCHMINSTRING];
CHAR vszAddress[CCHMINSTRING];
CHAR vszCity[CCHMINSTRING];
CHAR vszState[CCHMINSTRING];
CHAR vszZipCode[CCHMINSTRING];
CHAR vszPhone[CCHMINSTRING];
CHAR vszDefaultAddress[CCHMINSTRING];
CHAR vszDefaultCity[CCHMINSTRING]; 
CHAR vszDefaultState[CCHMINSTRING]; 
CHAR vszDefaultZipCode[CCHMINSTRING]; 
CHAR vszDefaultPhone[CCHMINSTRING]; 

/*
 * Global PM-related variables.
 */

PFNWP vpfnDefaultFrameProc = NULL;
PFNWP vpfnWPSCnrOwnerProc = NULL;

/*
 * Global SOM-related variables.
 */

somId vmidShrUnderstandsProtocol = 0;

/*
 * ShrDefPageDlgProc is a simple dialog procedure which tosses
 * WM_COMMAND's since the default dialog procedure dismisses 
 * the dialog.  Dialogs which are used as pages in the Settings
 * view notebook should use ShrDefPageDlgProc.
 */

extern MRESULT EXPENTRY ShrDefPageDlgProc
    (HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT mr = NULL;

  switch (msg)
  {
    case WM_COMMAND:
      mr = NULL;
      break;

    default:
      mr = WinDefDlgProc(hwndDlg, msg, mp1, mp2);
      break;
  }

  return mr;
}

/*
 * ShrSetDlgEntryFieldText simply sets a dialog box's entry
 * field and clears the entry field's changed bit.
 */

extern VOID ShrSetDlgEntryFieldText
    (HWND hwndDlg, ULONG id, PSZ psz)
{
  WinSetDlgItemText(hwndDlg, id, psz);
  WinSendDlgItemMsg(hwndDlg, id, EM_QUERYCHANGED, NULL, NULL);
}

/*
 * ShrSetDlgMLEText simply sets a dialog box's multi-line
 * entry field and clears the multi-line entry field's changed bit.
 */

extern VOID ShrSetDlgMLEText
    (HWND hwndDlg, ULONG id, PSZ psz)
{
  ULONG ulOffset, ulCopy;
  HWND hwndMLE;

  hwndMLE = WinWindowFromID(hwndDlg, id);

  /*
   * Start by clearing out of the MLE what was there before.
   */

  WinSendMsg(hwndMLE, MLM_SETSEL, MPFROMLONG(0),
      MPFROMLONG(WinSendMsg(hwndMLE, MLM_QUERYTEXTLENGTH, 0, 0)));
  WinSendMsg(hwndMLE, MLM_CLEAR, NULL, NULL);

  /*
   * Define the import buffer address and length.
   */

  ulCopy = strlen(psz);
  WinSendMsg(hwndMLE, MLM_SETIMPORTEXPORT,
      MPFROMP(psz), MPFROMLONG(ulCopy));

  /*
   * Import the text and clear the changed bit.
   */

  ulOffset = 0;
  WinSendMsg(hwndMLE, MLM_IMPORT,
      &ulOffset, MPFROMLONG(ulCopy));
  
  WinSendMsg(hwndMLE, MLM_SETCHANGED,
      MPFROMLONG(FALSE), NULL);
}

/*
 * ShrFrameCnrOwnerWndProc is only necessary to work around
 * a bug in the WPS.
 *
 * A hang results if an object is inserted with wpCnrInsertObject
 * is then deleted from another open view.
 *
 * This bug has been reported to Boca (PMR 3x053).
 */

extern MRESULT EXPENTRY ShrFrameCnrOwnerWndProc
    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  if (!ShrCheckForDeletedObject(hwnd, msg, mp1, mp2))
    return ShrDefFrameProc(hwnd, msg, mp1, mp2);
  else
    return NULL;
}

/*
 * ShrSetObjectString sets an object's instance data string and
 * notifies interested windows.
 */

extern BOOL ShrSetObjectString
    (SOMAny *somObject, PSZ pszNew, PSZ *ppszOld, ULONG ulChangeMsg)
{
  BOOL bHoldingSem = FALSE;
  PSZ psz = NULL;

  /*
   * NOTE!  It is very important that exception handlers be coded in 
   * routines that request mutex semaphores!  The exception condition
   * should relinquish the semaphore if it was successfully
   * requested.  Otherwise, later requestors of the semaphore could
   * wait indefinitely, typically resulting in a hang of one of the
   * WPS threads or the entire WPS.
   *
   * ShrStartTryBlock is a macro that installs an exception
   * handler for this function.  See SHARE.H for more details.
   */

  ShrStartTryBlock;

  bHoldingSem = !_wpRequestObjectMutexSem(somObject, 
      SEM_INDEFINITE_WAIT);

  if (bHoldingSem)
  {
    psz = _wpAllocMem(somObject, strlen(pszNew)+1, NULL);
  
    if (psz)
    {
      strcpy(psz, pszNew);
  
      if (*ppszOld)
        _wpFreeMem(somObject, *ppszOld);
  
      *ppszOld = psz;
  
  
      _wpSaveDeferred(somObject);
    }

    /*
     * Let's notify interested windows AFTER releasing the
     * object's semaphore because the notified windows
     * may want to acquire it. 
     */

    bHoldingSem = _wpReleaseObjectMutexSem(somObject);

    if (psz)
      _shrNotifyInterestedWindows(_shrQueryNotifier(somObject),
          somObject, ulChangeMsg, somObject, psz);
  }

  ShrEndTryBlock;
  return BOOLFROMP(psz);

  /*
   * Exception handling code for this function goes here.
   */

OnException:
  if (bHoldingSem)
    bHoldingSem = _wpReleaseObjectMutexSem(somObject);

  ShrEndTryBlock;
  return FALSE;
}

/*
 * ShrQueryObjectString returns an object's instance data string.
 */

extern ULONG ShrQueryObjectString
    (SOMAny *somObject, PSZ pszReturn, PSZ *ppsz, PSZ pszDefault)
{
  BOOL bHoldingSem = FALSE;
  ULONG cch = 1;
  PSZ pszCopy;

  /*
   * NOTE!  It is very important that exception handlers be coded in 
   * routines that request mutex semaphores!  The exception condition
   * should relinquish the semaphore if it was successfully
   * requested.  Otherwise, later requestors of the semaphore could
   * wait indefinitely, typically resulting in a hang of one of the
   * WPS threads or the entire WPS.
   *
   * ShrStartTryBlock is a macro that installs an exception
   * handler for this function.  See SHARE.H for more details.
   */

  ShrStartTryBlock;

  bHoldingSem = !_wpRequestObjectMutexSem(somObject, 
      SEM_INDEFINITE_WAIT);

  if (bHoldingSem)
  {
    if (*ppsz)
      pszCopy = *ppsz;
    else
      pszCopy = pszDefault;
  
    if (pszCopy)
      cch += strlen(pszCopy);
  
    if (pszReturn)
      if (cch > 1)
        memcpy(pszReturn, pszCopy, cch);
      else
        pszReturn[0] = NULLCHAR;

    bHoldingSem = _wpReleaseObjectMutexSem(somObject);
  }

  ShrEndTryBlock;
  return cch;

  /*
   * Exception handling code for this function goes here.
   */

OnException:
  if (bHoldingSem)
    bHoldingSem = _wpReleaseObjectMutexSem(somObject);

  ShrEndTryBlock;
  return 0;
}

/*
 * ShrEmptyObjectCnr empties an object container before 
 * it is destroyed.
 *
 * WARNING: Remove all the WPS object container records from
 * the container before closing (otherwise the default container 
 * destroy action is to free the records, which throughly screws
 * up WPS since its records are shared).
 */

extern VOID ShrEmptyObjectCnr(HWND hwndCnr)
{
  PMINIRECORDCORE pMiniRecord, pNextMiniRecord;

  pMiniRecord = (PMINIRECORDCORE)
      WinSendMsg(hwndCnr, CM_QUERYRECORD, MPFROMLONG(CMA_FIRST),
      MPFROM2SHORT(CMA_FIRST, CMA_ITEMORDER));

  while (pMiniRecord)
  {
    /*
     * Note when removing records from a container you should
     * get the next record FIRST because the current record
     * will no longer be valid.
     */

    pNextMiniRecord = (PMINIRECORDCORE)
        WinSendMsg(hwndCnr, CM_QUERYRECORD, MPFROMP(pMiniRecord),
        MPFROM2SHORT(CMA_NEXT, CMA_ITEMORDER));
    _wpCnrRemoveObject(OBJECT_FROM_PREC(pMiniRecord), hwndCnr);

    pMiniRecord = pNextMiniRecord;
  }
}

/*
 * ShrOpeningView handles the mundane task of preparing a
 * new view for its introduction to WPS.
 */

extern BOOL ShrOpeningView
    (SOMAny *somSelf, HWND hwndFrame, ULONG ulView, PSZ pszViewTitle)
{
  PUSEITEM pUseItem = NULL;
  BOOL bSuccess = FALSE;

  /*
   * Note!
   *
   * When a view is registered with wpRegisterView, it
   * is subclassed and its window procedure pointer saved
   * in a reserved window words.  The WPS subclass window procedure 
   * handles * all the CN_ messages for WPS object container records
   * inserted into our contents container, along with the system 
   * menu code to display the context menu, and a few WM_COMMAND 
   * messages (delete, copy, move, etc).
   *
   * Save a pointer to the WPS subclass for later we want
   * to resubclass so our window procedure is called first.
   * See "Problem 1" in WPS-PGM.TXT for more details, and
   * ADDRBKV.C for an example.
   */

  bSuccess = _wpRegisterView(somSelf, hwndFrame, pszViewTitle);

  if (bSuccess)
  {
    vpfnWPSCnrOwnerProc = (PFNWP) WinQueryWindowPtr(hwndFrame, QWP_PFNWP);

    pUseItem = (PUSEITEM) _wpAllocMem(somSelf, sizeof(USEITEM)+
        sizeof(VIEWITEM), NULL);
    bSuccess = BOOLFROMP(pUseItem);
  }

  if (bSuccess)
  {
    pUseItem->type = USAGE_OPENVIEW;
    ((PVIEWITEM) (pUseItem+1))->handle = hwndFrame;
    ((PVIEWITEM) (pUseItem+1))->view = ulView;

    /*
     * The WPS will automatically add the "in-use" (hashed)
     * emphasis to the object's icon when an USAGE_OPENVIEW
     * use item is added to the object's use list.
     */
  
    bSuccess = _wpAddToObjUseList(somSelf, pUseItem);
  }

  /*
   * If any of the above fails, free the memory for the use item 
   * (if it was successfully allocated) since it will not be needed.
   */

  if (!bSuccess && pUseItem)
    _wpFreeMem(somSelf, (PBYTE) pUseItem);

  return bSuccess;
}

/*
 * ShrClosingView handles the mundane task of closing a view
 * which is no longer needed.
 */

extern BOOL ShrClosingView
    (SOMAny *somSelf, HWND hwndFrame)
{
  PUSEITEM pUseItem;

  /*
   * Each object has a "use list".  Each structure in the use item
   * list is always followed immediately by a type dependant struct 
   * such as MEMORYITEM, RECORDITEM, VIEWITEM or some other user 
   * defined structure.
   *
   * We're interested in the USAGE_OPENVIEW use items.  They look like:
   *
   *   struct _USEITEM
   *   {
   *      ULONG           type;         // Type of this item
   *      struct _USEITEM *pNext;       // Next item in use list
   *   } USEITEM;
   *  
   * followed immediately in memory by:
   *  
   *   struct _VIEWITEM         
   *   {
   *      ULONG        view;            // Object view this represents
   *      LHANDLE      handle;          // Open handle 
   *      ULONG        ulViewState;     // View State flags
   *      HWND         hwndCnr;         // System use only 
   *      PMINIRECORDCORE pRecord;      // System use only 
   *   } VIEWITEM;
   *  
   * Find the USAGE_OPENVIEW use item in the object's list and remove
   * it.  If this is the last USAGE_OPENVIEW use item for this object,
   * the WPS will automatically remove the "in-use" (hashed) emphasis 
   * of the object's icon.
   */

  pUseItem = _wpFindUseItem(somSelf, USAGE_OPENVIEW, NULL);
  while (pUseItem && ((PVIEWITEM) (pUseItem+1))->handle != hwndFrame)
    pUseItem = _wpFindUseItem(somSelf, USAGE_OPENVIEW, pUseItem);

  if (pUseItem)
  {
    _wpDeleteFromObjUseList(somSelf, pUseItem);
    _wpFreeMem(somSelf, (PBYTE) pUseItem);
  }

  return BOOLFROMP(pUseItem);
}

/*
 * SOM/WPS class initialization
 *
 * When a SOM class is registered, SOM first tries to call
 * the default DLL entry point SOMInitModule.  It is here
 * that we register all the SOM/WPS classes for this DLL.
 *
 * Note that if this module had no SOMInitModule export, SOM
 * would try to call the NewClass entry point, eg, registering
 * class "Animal" would cause AnimalNewClass to be called.
 */

void _System SOMInitModule(ULONG ulMajor, ULONG ulMinor)
{
  /*
   * Initialize SOM/WPS classes.
   */

  ShrListNewClass(0,0);
  ShrNotifierNewClass(0,0);
  ShrFolderNewClass(0, 0);
  ShrAddressBookNewClass(0, 0);
  ShrPersonNewClass(0, 0);

  /*
   * Protocols are used during drag operations to determine
   * if the dragged objects will understand a set of messages.
   * Below we initialize a SOM ID used in determining if
   * a dragged object will respond to the "shrUnderstandsProtocol"
   * API.
   *
   * While in this implementation, only ShrPerson responds to
   * this message, other classes may be added later.  This
   * technique is better than using _somIsA which presupposes
   * a specific location in the class hierarchy.
   */

  vmidShrUnderstandsProtocol = 
      SOM_IdFromString(vszShrUnderstandsProtocol);
}

/*
 * DLL Initialization
 *
 * When the DLL is loaded or unloaded, this entry point
 * will be called.  Initialize/terminate the C runtime
 * environment (see page 234 of the "IBM C Set/2 User's
 * Guide" for more details), and allocate/free global
 * resources.
 *
 * Note: This routine is invoked before SOMInitModule.
 */

int _CRT_init(void);
int _CRT_term(void);

unsigned long _System _DLL_InitTerm
    (unsigned long hmod, unsigned long ulFlag)
{
  CLASSINFO classInfo;
  ULONG i;
  BOOL bSuccess = TRUE;

  /*
   * ulFlag is zero if the DLL is being loaded, and
   * one if the DLL is being unloaded.
   */

  if (ulFlag == 0)
  {
    /*
     * Initialize the C Set/2 runtime.
     */

    if (_CRT_init() == -1)
      bSuccess = FALSE;

    /*
     * Load the MRI strings into global string variables.
     *
     * We are being rather cavalier in assuming WinLoadString
     * cannot fail, but it is a fairly safe bet because the
     * MRI strings are attached to the DLL this code is executing,
     * and the strings are stored in global memory that was
     * allocated when the DLL was loaded.
     */

    WinLoadString(NULLHANDLE, hmod,
        IDS_PERSONINFOTAB, sizeof(vszPersonInfoTab),
        vszPersonInfoTab);
    WinLoadString(NULLHANDLE, hmod,
        IDS_PERSONTITLE, sizeof(vszPersonTitle),
        vszPersonTitle);
    WinLoadString(NULLHANDLE, hmod,
        IDS_ADDRESSBOOKTITLE, sizeof(vszAddressBookTitle),
        vszAddressBookTitle);
    WinLoadString(NULLHANDLE, hmod,
        IDS_ADDRESSBOOKVIEWTITLE, sizeof(vszAddressBookViewTitle),
        vszAddressBookViewTitle);
    WinLoadString(NULLHANDLE, hmod,
        IDS_INDEXCHARSALL, sizeof(vszAllIndexChars),
        vszAllIndexChars);
    WinLoadString(NULLHANDLE, hmod,
        IDS_PERSONNEWTITLE, sizeof(vszPersonNewTitle),
        vszPersonNewTitle);
    WinLoadString(NULLHANDLE, hmod,
        IDS_PHONE, sizeof(vszPhone), vszPhone);
    WinLoadString(NULLHANDLE, hmod,
        IDS_ADDRESS, sizeof(vszAddress), vszAddress);
    WinLoadString(NULLHANDLE, hmod,
        IDS_CITY, sizeof(vszCity), vszCity);
    WinLoadString(NULLHANDLE, hmod,
        IDS_STATE, sizeof(vszState), vszState);
    WinLoadString(NULLHANDLE, hmod,
        IDS_ZIPCODE, sizeof(vszZipCode), vszZipCode);
    WinLoadString(NULLHANDLE, hmod,
        IDS_DEFAULTPHONE, sizeof(vszDefaultPhone), vszDefaultPhone);
    WinLoadString(NULLHANDLE, hmod,
        IDS_DEFAULTADDRESS, sizeof(vszDefaultAddress), vszDefaultAddress);
    WinLoadString(NULLHANDLE, hmod,
        IDS_DEFAULTCITY, sizeof(vszDefaultCity), vszDefaultCity);
    WinLoadString(NULLHANDLE, hmod,
        IDS_DEFAULTSTATE, sizeof(vszDefaultState), vszDefaultState);
    WinLoadString(NULLHANDLE, hmod,
        IDS_DEFAULTZIPCODE, sizeof(vszDefaultZipCode), vszDefaultZipCode);

    /*
     * Load the MRI strings for the tabs, "A - C", "D - F", ...
     * "V - Z", and "Misc".
     */
  
    for (i=0; i < CINDEXTABS; i++)
      WinLoadString(NULLHANDLE, hmod,
          i+IDS_FIRSTINDEXTAB, sizeof(vaszIndexTabs[i]),
          vaszIndexTabs[i]);

    /*
     * Load the MRI strings "ABC", "DEF", ... "VWXYZ"
     * used for determining what objects are shown on each
     * page (see ShrFilterAddressBookRecord in ADDRBKV.C).
     */
  
    for (i=0; i < CINDEXCHARS; i++)
      WinLoadString(NULLHANDLE, hmod,
          i+IDS_FIRSTINDEXCHARS, sizeof(vaszIndexChars[i]),
          vaszIndexChars[i]);
  
    /*
     * Register private window classes.
     */
  
    WinQueryClassInfo(NULLHANDLE, WC_FRAME, &classInfo);
    vpfnDefaultFrameProc = classInfo.pfnWindowProc;
  
    WinRegisterClass(NULLHANDLE, vszWcFrameCnrOwner,
        ShrFrameCnrOwnerWndProc,
        ((classInfo.flClassStyle & ~CS_PUBLIC)),
        classInfo.cbWindowData);
  }
  else if (ulFlag == 1)
  {
    /*
     * Free resources allocated during DLL initialization.
     */

    _CRT_term();

    /*
     * Note we didn't have to free the MRI strings since
     * they're stored in global string buffers that are
     * freed when the DLL is unloaded.
     *
     * If we had allocated other resources, eg, memory,
     * PM pointers, etc, we would check if they were
     * allocated here and free them if necessary.
     */
  }

  return bSuccess;
}

/*
 * Exception handler that returns traps via longjmp().
 */

extern ULONG APIENTRY ShrExceptionHandler
    (PEXCEPTIONREPORTRECORD pReportRecord, 
    PEXCEPTIONREGISTRATIONRECORD pRegRecord,
    PCONTEXTRECORD pContextRecord, PVOID pReserved)
{
  /*
   * The report record is set by the system exception handler
   * to tell us what happen.  The exception registration record
   * pointer points to the address on the stack of the registration
   * record we passed to DosSetExceptionHandler.
   *
   * See the ShrStartTryBlock and ShrEndTryBlock macros in
   * SHARE.H for more details.
   */

  if (pReportRecord->ExceptionNum == XCPT_ACCESS_VIOLATION)
  {
    /*
     * This long, ominous, beep will let the user know something
     * is amiss.  Putting up a message box might be nice,
     * but we'd need to check if we're running on a PM thread
     * before doing that.
     */

    DosBeep(200, 1500);

    longjmp(((PSHREXCEPTIONREGISTRATIONRECORD) pRegRecord)->env, -1);
  }

  /* 
   * If we return to here then we could not handle the exception.
   * The next handler will be invoked (eg, to handle a guard page
   * exception to grow the stack).
   */

  return XCPT_CONTINUE_SEARCH;
}
