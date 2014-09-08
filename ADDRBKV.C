/*
 * OS/2 Work Place Shell Sample Program - ShrAddressBook view code
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
 * The address book view window structure is shown below
 * along with the corresponding window procedures:
 *
 *   hwndFrame (ShrAddressBookFrameWndProc)
 *        |
 *   hwndNotebook (WC_NOTEBOOK)
 *        |
 *   hwndCnrOwner (ShrFrameCnrOwnerWndProc)
 *        |
 *   hwndCnr (WC_CONTAINER)
 *
 * All the notebook pages point to the same window, hwndCnrOwner.  
 * The contents of the container is filtered based on the current top
 * page.  
 */

#define SOM_NoTest

#include "share.h"
#include "addrbk.h"
#include "addrbkv.h"
#include "notify.h"
#include "person.h"

/*
 * A pointer to the view data for the Address Book is kept
 * in the extra window word (QW_USER) of the frame.
 */

#define setAddressBookViewDataFromFrame(h,p) \
    WinSetWindowULong(h,QWL_USER,(ULONG)p)
#define queryAddressBookViewDataFromFrame(h) \
    (PADDRESSBOOKVIEWDATA) WinQueryWindowULong(h,QWL_USER)

/*
 * Below is the data that is kept for each Address
 * Book's Indexed View.  It would be better if this structure
 * was converted to a SOM object with methods like shrOpen,
 * shrClose, shrPersonAdded, etc.
 */

typedef struct
{
  SOMAny *somSelf;
  HWND hwndCnr;
  HWND hwndNotebook;
} ADDRESSBOOKVIEWDATA, *PADDRESSBOOKVIEWDATA;

/*
 * ShrFilterAddressBookRecord is a container filter function.
 *
 * It allows objects who's first character below to the set
 * of characters for the page.  For example, "Smith" belongs in 
 * "S - V" page.  The pszIndexChars parameter would point to
 * the string "STUV" when filtering for that page (see CM_FILTER
 * message for more details).
 */

extern BOOL EXPENTRY ShrFilterAddressBookRecord
    (PMINIRECORDCORE pRecord, PSZ pszIndexChars)
{
  BOOL bInclude = FALSE;
  PSZ pszTitle;
  CHAR chFirst;
  SOMAny *somObject;

  /*
   * In theory, no non-person objects can get into the
   * address book since ShrAddressBook's wpDragOver
   * method refuses non-person objects.
   *
   * But check anyway just in case the user opened a command line
   * and copied something to the address book subdirectory.
   *
   * Note that _somIsA is not used here.  That is because later
   * developers may want to implement a new-and-improved version
   * of the person object that is not a direct descendant of
   * ShrPerson.  But note there is a problem -- ShrPerson
   * has many methods that are not "name lookup" methods, ie, they
   * use the offset method, so creating a new person class that is
   * not a subclass of ShrPerson would require considerable care since
   * it would have to be certain to have its version of ShrPerson
   * methods in the same order in the method table as ShrPerson's.
   */

  somObject = _shrclsValidateObject(ShrFindAddressBookClass(),
      OBJECT_FROM_PREC(pRecord));

  if (somObject)
  {
    pszTitle = _wpQueryTitle(somObject);

    /*
     * When the user creates a new person for the address
     * book, it is given the default name "Last name, First name".
     *
     * We'd like to make it easy to find the newly created person,
     * so the user doesn't have to search through the pages 
     * (ie, the "J - L" page would contain "Last name, First name").  
     * So add a filter check for the default name, and if this object
     * has it, allow it on all pages.
     */

    if (strstr(pszTitle, vszPersonNewTitle))
    {
      bInclude = TRUE;
    }
    else
    {
      chFirst = toupper(pszTitle[0]);
  
      /*
       * The index characters (pszIndexChars) is a list of those
       * characters allowed in the address book page that is 
       * currently active. For example, the "A - C" page index 
       * characters string is "ABC", "V - Z" page uses "VWXYZ".
       *
       * If it is NULL, then we're filtering for the "Misc" page
       * which contains all others that don't belong in the
       * "A" to "Z" pages (eg, "1st Federal Bank").  We check if
       * the first character doesn't fit in the "A" to "Z" pages
       * by searching "vszAllIndexChars" which contains the 
       * characters "ABCDEFGHIJKLMNOPQRSTUVWXYZ".
       */
  
      if (pszIndexChars)
        bInclude = BOOLFROMP(strchr(pszIndexChars, chFirst));
      else
        bInclude = !strchr(vszAllIndexChars, chFirst);
    }
  }

  return bInclude;
}

/*
 * ShrSortAddressBookRecord is a container sort function.
 * It simply sorts by object title.  
 *
 * See the pSortRecord field in the CNRINFO structure, 
 * the CM_SETCNRINFO message, and the CM_SORTRECORD message for
 * more details.
 */

extern SHORT EXPENTRY ShrSortAddressBookRecord
    (PMINIRECORDCORE pRecord1, PMINIRECORDCORE pRecord2, PVOID pUnused)
{
  return stricmp(_wpQueryTitle(OBJECT_FROM_PREC(pRecord1)),
      _wpQueryTitle(OBJECT_FROM_PREC(pRecord2)));
}

/*
 * ShrCnrInsertObject inserts the object into the given container.
 *
 * No positioning is done, so CCS_AUTOPOSITION should be specified
 * when creating the container.
 */

extern PMINIRECORDCORE ShrCnrInsertObject
    (SOMAny *somInsertObject, HWND hwndCnr)
{
  RECORDINSERT recordInsert;
  POINTL ptlIcon;
  PMINIRECORDCORE pMiniRecord;

  /*
   * Note the record is inserted without invalidating.
   * This is because the container records will later
   * be filtered with CM_FILTER which invalidates all
   * the records (see ShrAddressBookUpdateTopPage for
   * more details).
   */

  ptlIcon.x = ptlIcon.y = 0;

  memset(&recordInsert, 0, sizeof(recordInsert));
  recordInsert.cb = sizeof(RECORDINSERT);
  recordInsert.pRecordOrder = (PRECORDCORE) CMA_END;
  recordInsert.zOrder = CMA_TOP;
  recordInsert.cRecordsInsert = 1;
  recordInsert.fInvalidateRecord = FALSE;

  pMiniRecord = _wpCnrInsertObject(somInsertObject, hwndCnr,
      &ptlIcon, NULL, &recordInsert);

  return pMiniRecord;
}

/*
 * ShrAddressBookUpdateTopPage handles the user switching to a new
 * page in the address book.
 */

extern VOID ShrAddressBookUpdateTopPage
    (SOMAny *somSelf, HWND hwndNotebook, HWND hwndCnr)
{
  ULONG ulPageId;
  PSZ pszIndexChars;
  BOOKTEXT booktext;
  CNRINFO cnrInfo;

  /*
   * The notebook page data points to a static string containing
   * the list of valid first-characters for the page, eg:
   * "ABC" for "A - C".  If the index characters pointer is NULL,
   * then the page is the "Misc" page, ie, it holds all the persons
   * whose names do not fit into one of the other pages (eg,
   * "1st Federal Bank").
   */

  ulPageId = (ULONG) WinSendMsg(hwndNotebook, BKM_QUERYPAGEID,
        MPFROMLONG(BKA_TOP), MPFROMLONG(BKA_TOP));
  pszIndexChars = (PSZ) WinSendMsg(hwndNotebook, BKM_QUERYPAGEDATA,
        MPFROMLONG(ulPageId), NULL);
  WinSendMsg(hwndCnr, CM_FILTER,
        MPFROMP(ShrFilterAddressBookRecord), pszIndexChars);

  /*
   * OS/2 2.0 GA container had a bug which did not re-sort the
   * containers when the icon text was changed even though
   * CMA_PSORTRECORD was specified on CM_SETCNRINFO.
   *
   * Make an extra call to CM_SORTRECORD in case we're running
   * on a 2.0 machine.
   */

  WinSendMsg(hwndCnr, CM_SORTRECORD,
        MPFROMP(ShrSortAddressBookRecord), NULL);

  /*
   * The address book container title is set to the tab of the
   * top page to make it a little more obvious what page the user
   * is dealing with.
   * 
   * Note the container title buffer must be static, ie, not on the
   * stack, since the container does not make a copy of it
   * but strictly stores your pointer to it.
   */

  booktext.pString = vszAddressBookCnrTitle;
  booktext.textLen = sizeof(vszAddressBookCnrTitle);
  WinSendMsg(hwndNotebook, 
        BKM_QUERYTABTEXT, MPFROMLONG(ulPageId),
        &booktext);

  cnrInfo.cb = sizeof(cnrInfo);
  cnrInfo.flWindowAttr = CA_CONTAINERTITLE | 
      CA_TITLEREADONLY | CA_TITLESEPARATOR;
  cnrInfo.pszCnrTitle = vszAddressBookCnrTitle;

  WinSendMsg(hwndCnr, CM_SETCNRINFO, MPFROMP(&cnrInfo), 
      MPFROMLONG(CMA_FLWINDOWATTR | CMA_CNRTITLE));
}

/*
 * ShrAddressBookPersonAdded is called when the open view 
 * notified via SHRN_ADDRESSBOOKPERSONADDED
 * page in the address book.

 * Note: The address book view is notified because it has 
 * registered interest via the shrAddInterestedWindow method
 * (see ShrNotifier for more details).
 */

extern VOID ShrAddressBookPersonAdded
    (SOMAny *somSelf, HWND hwndNotebook, HWND hwndCnr, 
    SOMAny *somNewPerson)
{
  ShrCnrInsertObject(somNewPerson, hwndCnr);
  ShrAddressBookUpdateTopPage(somSelf, hwndNotebook, hwndCnr);
}

/*
 * ShrAddressBookDestroy is called when the address book frame
 * receives WM_DESTROY.
 */

extern VOID ShrAddressBookDestroy
    (SOMAny *somSelf, HWND hwndFrame, HWND hwndCnr)
{
  /*
   * This view is going away, so remove interest in the model.
   */

  _shrRemoveInterestedWindow(_shrQueryNotifier(somSelf), hwndFrame);

  /*
   * Remove all the WPS object container records from
   * the container before closing (otherwise
   * the default container destroy action is to free the records,
   * which screws up WPS since its records are shared).
   */

  ShrEmptyObjectCnr(hwndCnr);

  /*
   * Save the colors, fonts, size and position of the
   * address book frame.
   */

  WinStoreWindowPos(vszAppName, vszIndexedViewWindowPosKeyName,
      hwndFrame);

  /*
   * Remove and free the USAGE_OPENVIEW item from the address
   * book use list.
   */

  ShrClosingView(somSelf, hwndFrame);
}

/*
 * ShrCnrRegisterOwner is used to register a FRAME window as
 * a view of a WPS object.  Calling this method associates the
 * frame with the object so WPS can figure out what frames belong
 * to which objects during drag operations.
 *
 * Note: wpRegisterView will subclass the frame window to add
 * standard WPS container object behavior (drag/drop, context
 * menus, etc).  The wpRegisterView method expects top-level frame
 * windows since it adds it to the Window List.  But we have to
 * call it anyway to get WPS to subclass this frame, even though
 * it isn't a top level window.  As fate would have it, the call
 * to WinAddSwitchEntry fails for this frame is not top-level.
 *
 * See ShrOpenAddressBook for more details.
 *
 * A better long-term solution would be for WPS to add 
 * wpCnrRegisterOwner method that does all that wpRegisterView
 * does today EXCEPT for trying to add the frame to the Window List.
 * (see "Problem 1" in WPS-PGM.TXT for more details).
 */

extern BOOL ShrCnrRegisterOwner(SOMAny *somObject, HWND hwndCnrOwner)
{
  return _wpRegisterView(somObject, hwndCnrOwner, vszNullString);
}

/*
 * ShrAddressBookFrameWndProc
 */

extern MRESULT EXPENTRY ShrAddressBookFrameWndProc
    (HWND hwnd, ULONG msg, MPARAM mp1, MPARAM mp2)
{
  MRESULT mr = NULL;
  PADDRESSBOOKVIEWDATA pViewData;

  switch (msg)
  {
    /*
     * SHRN_ADDRESSBOOKPERSONADDED is sent when a person
     * is added to the address book model.  Update the
     * view to match.
     */

    case SHRN_ADDRESSBOOKPERSONADDED:
      pViewData = queryAddressBookViewDataFromFrame(hwnd);

      ShrAddressBookPersonAdded(pViewData->somSelf,
              pViewData->hwndNotebook, pViewData->hwndCnr,
              (SOMAny *) mp2);
      break;

    /*
     * WM_CONTROL is sent when an owned control has something
     * about it that has changed.  We're only interested in the
     * page turning activity of the notebook.
     */

    case WM_CONTROL:
      pViewData = queryAddressBookViewDataFromFrame(hwnd);

      if (SHORT1FROMMP(mp1) == FID_CLIENT)
      {
        if (SHORT2FROMMP(mp1) == BKN_PAGESELECTED)
          ShrAddressBookUpdateTopPage(pViewData->somSelf,
              pViewData->hwndNotebook, pViewData->hwndCnr);
      }
      else
      {
        mr = ShrDefWPSCnrOwnerProc(hwnd, msg, mp1, mp2);
      }
      break;

    /*
     * WM_DESTROY is sent as the last message to the window.
     * Make certain WM_DESTROY is forwarded to the superclass
     * so frame resources are freed.
     *
     * Here we have to check the view data pointer because
     * the frame may have been successfully created, but
     * one of the children could not be created.  Under
     * all other cases this check is not necessary.
     */

    case WM_DESTROY:
      pViewData = queryAddressBookViewDataFromFrame(hwnd);

      if (pViewData)
      {
        ShrAddressBookDestroy(pViewData->somSelf,
            hwnd, pViewData->hwndCnr);
        _wpFreeMem(pViewData->somSelf, (PBYTE) pViewData);
      }

      ShrDefWPSCnrOwnerProc(hwnd, msg, mp1, mp2);
      break;

    /*
     * WM_SYSCOMMAND is sent from the system menu.  We only
     * handle the close case.
     */

    case WM_SYSCOMMAND:
      if (SHORT1FROMMP(mp1) == SC_CLOSE)
        WinDestroyWindow(hwnd);
      else
        mr = ShrDefWPSCnrOwnerProc(hwnd, msg, mp1, mp2);
      break;

    default:
      mr = ShrDefWPSCnrOwnerProc(hwnd, msg, mp1, mp2);
      break;
  }

  return mr;
}

/*
 * ShrOpenAddressBook creates the address book view and displays it.
 */
 
extern HWND ShrOpenAddressBook(SOMAny *somSelf)
{
  HWND hwndFrame = NULLHANDLE;
  PADDRESSBOOKVIEWDATA pViewData = NULL;
  BOOL bHoldingSem = FALSE;
  HWND hwndNotebook;
  HWND hwndCnrOwner;
  HWND hwndCnr;
  FRAMECDATA fcdata;
  HPS hps;
  SHORT sHeight = 0, sWidth = 0, sTemp;
  ULONG i, ulPageId;
  RECTL rectl;
  SOMAny *somObject;
  CNRINFO cnrInfo;
  SWP swp;

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

  /*
   * Create the Address Book view.  It's parent/child 
   * window structure is:
   *
   *   hwndFrame, unowned (ShrAddressBookFrameWndProc)
   *      |
   *    hwndNotebook, owned by hwndFrame (WC_NOTEBOOK)
   *       |
   *     hwndCnrOwner, owned by hwndNotebook (ShrFrameCnrOwnerWndProc)
   *        |
   *      hwndCnr, owned by hwndCnrOwner (WC_CONTAINER)
   */

  memset(&fcdata, 0, sizeof(fcdata));
  fcdata.cb = sizeof(FRAMECDATA);
  fcdata.flCreateFlags = FCF_TITLEBAR | FCF_SYSMENU | FCF_MINMAX |
      FCF_SIZEBORDER;

  hwndFrame = WinCreateWindow(HWND_DESKTOP,
     WC_FRAME, vszNullString,
     WS_ANIMATE, 0,0,0,0, NULLHANDLE, HWND_TOP, 0, &fcdata, NULL);

  /*
   * The notebook is initially created unowned to avoid
   * unnecessary updates when the first page is inserted, ie,
   * WM_CONTROL (BKN_PAGESELECTED).
   *
   * Once all the pages are inserted, the notebook owner
   * is set to the top frame (hwndFrame).
   */

  hwndNotebook = WinCreateWindow(hwndFrame, WC_NOTEBOOK,
      vszNullString,
      (WS_VISIBLE | BKS_SPIRALBIND | BKS_SQUARETABS |
      BKS_BACKPAGESBR | BKS_MAJORTABBOTTOM  |
      BKS_TABTEXTCENTER), 0,0,0,0,
      NULLHANDLE, HWND_TOP, FID_CLIENT, NULL, NULL);

  /* OS/2 GA bug:
   *
   * WPS assumes that the container is a direct child of
   * the frame, and that the frame has a FID_SYSMENU.  If
   * it does not have a system menu, then the context menu 
   * created by clicking on the whitespace of the container 
   * (which brings up the address book context menu) erroneously 
   * sends its menu selections to the desktop in OS/2 2.0 GA.
   *
   * In OS/2 2.1 beta, the menu selections are sent to the
   * correct window, but context menu emphasis is not set.
   *
   * This bug has been report to Boca (4/2/93).
   */

  fcdata.flCreateFlags = 0;
  hwndCnrOwner = WinCreateWindow(hwndNotebook,
     vszWcFrameCnrOwner, vszNullString,
     0, 0,0,0,0, hwndNotebook, HWND_TOP, 0, &fcdata, NULL);

  /*
   * If a container is to hold WPS objects (ie, it will be used as 
   * hwndCnr parameter in calls to the wpCnrInsertObject 
   * and wpCnrRemoveObject method), ITS OWNER AND PARENT MUST BE
   * A SUBCLASS OF WC_FRAME.  In addition, the container ID must
   * be FID_CLIENT.
   *
   * WPS makes assumptions about the container owner that are only
   * valid for WC_FRAME subclasses.
   */

  hwndCnr = WinCreateWindow(hwndCnrOwner,
      WC_CONTAINER, vszNullString,
      (WS_VISIBLE | CCS_EXTENDSEL | CCS_AUTOPOSITION |
      CCS_VERIFYPOINTERS | CCS_MINIRECORDCORE),
      0,0,0,0, hwndCnrOwner, HWND_BOTTOM,
      FID_CLIENT, NULL, NULL);

  /*
   * Allocate and save the view's instance data.  It would be a better
   * implementation to create a "view" object as a subclass of 
   * SOMObject with methods like _shrOpenView, _shrCloseView,
   * _shrPageSelected, etc.
   *
   * I'll leave that as an exercise for the user.
   */

  pViewData = (PADDRESSBOOKVIEWDATA) 
      _wpAllocMem(somSelf, sizeof(ADDRESSBOOKVIEWDATA), NULL);
  pViewData->somSelf = somSelf;
  pViewData->hwndNotebook = hwndNotebook;
  pViewData->hwndCnr = hwndCnr;
  setAddressBookViewDataFromFrame(hwndFrame, pViewData);

  /*
   * When a view is registered with wpRegisterView, it
   * is subclassed and its window procedure pointer saved
   * in a reserved window words.  The WPS subclass handles
   * all the CN_ messages for WPS object container records
   * inserted into our contents container, along with the
   * system menu code to display the context menu, and
   * a few WM_COMMAND (delete, copy, move, etc).
   *
   * We're going to re-subclass so ShrAddressBookFrameWndProc
   * gets called before the WPS version.  This is necessary
   * because the WPS version does not forward any
   * WM_CONTROL messages to its superclass, and
   * ShrAddressBookFrameWndProc needs the notebook notifications.
   *
   * Note the top frame was registered with the default
   * frame window procedure -- that will be the superclass
   * the WPS window procedure calls when it doesn't
   * handle a message, ie:
   *
   *   1. ShrAddressBookFrameWndProc, first, followed by
   *   2. WPS container owner window subclass, followed by
   *   3. WC_FRAME's window procedure, followed by
   *   4. WinDefWindowProc last
   *
   * WARNING: The notebook subclasses the page on
   * BKM_SETPAGEWINDOWHWND if it is the topmost page
   * and when a page is made the top page by the user clicking
   * on a tab.  Later, when the page is no longer the top
   * page, the notebook tries to unsubclass the window
   * by replacing QWP_PFNWP with the pointer that was
   * there before.  So WPS must subclass first so the
   * notebook does not undo the WPS subclass.
   */

  ShrOpeningView(somSelf, hwndFrame, 
      ID_OPENADDRESSINDEXVIEW, vszAddressBookViewTitle);
  WinSubclassWindow(hwndFrame, ShrAddressBookFrameWndProc);

  /*
   * The WPS window procedure requires that the container
   * owner and parent be the same.  Otherwise, the container
   * will not have standard WPS container behavior.
   *
   * Note: The BKM_SETPAGEWINDOWHWND message changes
   * hwndCnrOwner's parent to one of the notebooks
   * children.  This ensures that the page window draws
   * only within the top page boundaries.
   */

  ShrCnrRegisterOwner(somSelf, hwndCnrOwner);

  /*
   * Add the notebook pages, set their page data, and calculate
   * the size of the largest tab.
   */

  hps = WinGetPS(hwndNotebook);

  for (i = 0; i < CINDEXTABS; i++)
  {
    ulPageId = (ULONG) WinSendMsg(hwndNotebook, BKM_INSERTPAGE, 0,
        MPFROM2SHORT((BKA_MAJOR|BKA_AUTOPAGESIZE), BKA_LAST));

    /*
     * All the pages of the notebook are associated the same
     * container.  The contents of the container is filtered based 
     * on the current top page.  
     */

    WinSendMsg(hwndNotebook, BKM_SETPAGEWINDOWHWND,
        MPFROMP(ulPageId), MPFROMLONG(hwndCnrOwner));
    WinSendMsg(hwndNotebook, BKM_SETTABTEXT,
        MPFROMLONG(ulPageId), vaszIndexTabs[i]);

    /*
     * There is one extra page for those persons that don't
     * belong in any other page (eg, '1st Federal Bank').
     * All the other pages have page data that points to a
     * string containing valid first characters for person's
     * on their page, eg, "ABC" for the "A - C" page.
     *
     * See ShrFilterAddressBookRecord and the CM_FILTER
     * message for more details.
     *
     * Note that CINDEXCHARS is one less than CINDEXTABS.
     * The extra tab is for the "Misc" page, and its page data
     * pointer is NULL.
     */

    if (i < CINDEXCHARS)
      WinSendMsg(hwndNotebook, BKM_SETPAGEDATA,
          MPFROMLONG(ulPageId), vaszIndexChars[i]);

    /*
     * Calculate the largest tab size.  All the tabs in the notebook
     * are the same size, and we don't want to clip the tab text.
     *
     * WinDrawText determines the size needed to properly draw tab
     * text.  The actual drawing of the tabs is done by the notebook.
     */

    rectl.xLeft = rectl.yBottom = 0;
    rectl.xRight = rectl.yTop = 1;
    WinDrawText(hps, -1, vaszIndexTabs[i], 
        &rectl, 0, 0, DT_QUERYEXTENT);
  
    sTemp = (SHORT) (rectl.xRight - rectl.xLeft);
    if (sTemp > sWidth)
      sWidth = sTemp;
  
    sTemp = (SHORT) (rectl.yTop - rectl.yBottom);
    if (sTemp > sHeight)
      sHeight = sTemp;
  }

  WinReleasePS(hps);

  /*
   * Set the major tab size to the size of the largest tab
   * text, and set the minor tabs to (0,0) since the address
   * book view doesn't use them.  Note the fudge factor added
   * below to allow for tab cursor emphasis.
   */

  WinSendMsg(hwndNotebook, BKM_SETDIMENSIONS,
      MPFROM2SHORT(sWidth+8, sHeight+8), MPFROMSHORT(BKA_MAJORTAB));
  WinSendMsg(hwndNotebook, BKM_SETDIMENSIONS,
      MPFROM2SHORT(0, 0), MPFROMSHORT(BKA_MINORTAB));

  /*
   * Set the container sort function.
   *
   * The container sort function assures that each record
   * is sorted when inserted into the container.  Setting the
   * pSortRecord in the CNRINFO structure will sort the container
   * as records are inserted until CM_SETCNRINFO is called again
   * with CMA_PSORTRECORD and pSortRecord == NULL.
   *
   * CM_SORTRECORD, on the other hand, is a one-shot sort that 
   * doesn't affect subsequent record insertions.  CM_FILTER works
   * the same way as CM_SORTRECORD, ie, it must be called after 
   * record insertions to re-filter the records.
   */

  cnrInfo.cb = sizeof(cnrInfo);
  cnrInfo.pSortRecord = (PVOID) ShrSortAddressBookRecord;

  WinSendMsg(hwndCnr, CM_SETCNRINFO, MPFROMP(&cnrInfo), 
      MPFROMLONG(CMA_PSORTRECORD));

  /*
   * Size and position the address book view and force an
   * update of the screen so the user gets the feeling something
   * is happening.
   *
   * Note we're saving only one set of fonts, colors, size,
   * etc with WinStoreWindowPos for all the indexed views
   * of address books, so we need to call
   * WinQueryTaskSizePos to get a new suggestion position.
   * We could save a separate window position key for each
   * instance (like WPS does) using wpQueryHandle to create
   * a unique key, but that wastes space in the INI file
   * (and this is just an example, afterall).
   */

  WinQueryTaskSizePos(NULLHANDLE, 0, &swp);
  swp.hwnd = hwndFrame;
  swp.hwndInsertBehind = HWND_TOP;
  swp.fl |= (SWP_ZORDER | SWP_ACTIVATE | SWP_SHOW);

  if (WinRestoreWindowPos(vszAppName,
      vszIndexedViewWindowPosKeyName, hwndFrame))
    swp.fl &= ~SWP_SIZE;

  WinSetMultWindowPos(NULLHANDLE, &swp, 1);
  WinUpdateWindow(hwndFrame);

  /*
   * It would be better if wpPopulate was called from another
   * thread so we don't hold up PM while the address book (folder)
   * is filled.  
   *
   * But all the person objects are WPAbstract subclasses
   * which can be created very quickly, and we don't expect a lot
   * of them (famous last words).
   */

  WinSetPointer(HWND_DESKTOP,
     WinQuerySysPointer(HWND_DESKTOP, SPTR_WAIT, FALSE));

  _wpPopulate(somSelf, NULLHANDLE, NULL, FALSE);

  WinSetPointer(HWND_DESKTOP,
     WinQuerySysPointer(HWND_DESKTOP, SPTR_ARROW, FALSE));

  /*
   * Fill the address book view container with the person objects.
   * Request its semaphore to avoid objects being added/removed while 
   * in the loop below.
   */

  bHoldingSem = !_wpRequestObjectMutexSem(somSelf, SEM_INDEFINITE_WAIT);

  if (bHoldingSem)
  {
    somObject = _wpQueryContent(somSelf, NULL, QC_First);
    while (somObject)
    {
      ShrCnrInsertObject(somObject, hwndCnr);

      /*
       * Each call to wpPopulate increments the object's
       * lock count. The locking count is used to determine 
       * when an object is ready for garbage collection.  
       *                                                               
       * If you're going to access an object from a second thread, 
       * use wpRequestObjectMutexSem and wpReleaseObjectMutexSem.  
       * The wpRequestObjectMutexSem method will will prevent 
       * other threads from modifying your object's instance data 
       * while you do (of course, this assumes new methods you add 
       * that query/set instance data use wpRequestObjectMutexSem 
       * and wpReleaseObjectMutexSem, too).  
       *                                                               
       * WPS is extremely multithreaded.  The 
       * wpRequest/ReleaseObjectMutexSem methods are very 
       * important.  It is wise to request/release the object 
       * mutex semaphore in each method that queries/sets object
       * instance data.  In addition, the method should include 
       * exception protection should the method trap to make 
       * certain it releases the object's semaphore.  
       */

      _wpUnlockObject(somObject);
  
      somObject = _wpQueryContent(somSelf, somObject, QC_Next);
    }

    bHoldingSem = _wpReleaseObjectMutexSem(somSelf);
  }

  /*
   * Now that the address book view container is filled, update the
   * top page.  We put off setting the notebook owner till now to
   * prevent the notebook from sending a page selection notification
   * when we added the first page.
   */

  WinSetOwner(hwndNotebook, hwndFrame);
  ShrAddressBookUpdateTopPage(somSelf, hwndNotebook, hwndCnr);

  /*
   * Add interest in the address book so the open view will be
   * notified if something of interest occurs to it (eg, a person
   * is added to the address book).
   *
   * Interest is removed when the frame is destroyed.
   */

  _shrAddInterestedWindow(_shrQueryNotifier(somSelf), hwndFrame);

  ShrEndTryBlock;
  return hwndFrame;

  /*
   * Exception handling code for this function goes here.
   */

OnException:
  if (bHoldingSem)
    bHoldingSem = _wpReleaseObjectMutexSem(somSelf);

  if (pViewData)
    _wpFreeMem(somSelf, (PBYTE) pViewData);

  if (hwndFrame)
  {
    /*
     * Set the view instance data pointer to NULL since
     * it is tested in WM_DESTROY.
     */

    setAddressBookViewDataFromFrame(hwndFrame, NULL);
    WinDestroyWindow(hwndFrame);
  }

  ShrEndTryBlock;
  return NULLHANDLE;
}
