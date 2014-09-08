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

/*
 * The pragma below tells the compiler that it should put
 * string constants in a read-only segment, ie:
 *
 *   PSZ psz = "bay";
 *   *psz = 'd';
 *
 * would cause a runtime exception (trap).  Of course, using this
 * pragma is a matter of the programmer's preference, but the author
 * believes doing so avoids potential coding mistakes that are 
 * difficult to debug.
 */

#pragma strings(readonly)

/*
 * Include the toolkit headers that are needed by just about every
 * module in this example.
 */

#define INCL_WIN
#define INCL_DOS
#include <os2.h>
#include <som.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <setjmp.h>
#include <ctype.h>
#include <wpobject.h>

/*
 * Typedef's.
 */

typedef struct 
{
  struct _EXCEPTIONREGISTRATIONRECORD * volatile prev_structure;
   _ERR * volatile ExceptionHandler;
   jmp_buf env;
} SHREXCEPTIONREGISTRATIONRECORD, *PSHREXCEPTIONREGISTRATIONRECORD;

/*
 * Misc constants
 */

#define NULLCHAR                        '\0'
#define CCHMAXSTRING                    260
#define CCHMINSTRING                    64

/*
 * Dialog IDs (IDD_)
 */

#define IDD_PERSONINFOPAGE              100

/*
 * Pointer (icon) IDs (IDP_)
 */
                                        
#define IDP_PERSON                      1
#define IDP_ADDRESSBOOK                 2

/*
 * Dialog control IDs.
 */
                                        
#define ID_NAME                         1
#define ID_ADDRESS                      2
#define ID_CITY                         3
#define ID_STATE                        4
#define ID_ZIPCODE                      5
#define ID_PHONE                        6

/*
 * Pop-up menu IDs.  Note these must start with WPMENUID_USER
 * to be assured they don't overlap those defined by the WPS.
 */

#define ID_OPENADDRESSINDEXVIEW         (WPMENUID_USER+0)
#define ID_CREATEPERSON                 (WPMENUID_USER+1)

/*
 * Menu control IDs (IDM_)
 */

#define IDM_OPENADDRESSBOOK             1
#define IDM_ADDRESSBOOK                 2                  
                                                           
/*                                                         
 * String table IDs (IDS_)                                 
 */                                                        
                                                           
#define IDS_ADDRESSBOOKVIEWTITLE        1       // "Indexed View"
#define IDS_PERSONINFOTAB               2       // "Info"
#define IDS_PERSONTITLE                 3       // "Person"
#define IDS_ADDRESSBOOKTITLE            4       // "Address Book"

#define IDS_INDEXTAB1                   5       // "A - C"
#define IDS_INDEXTAB2                   6       // "D - F"
#define IDS_INDEXTAB3                   7       // "G - I"
#define IDS_INDEXTAB4                   8       // "J - L"
#define IDS_INDEXTAB5                   9       // "M - O"
#define IDS_INDEXTAB6                   10      // "P - R"
#define IDS_INDEXTAB7                   11      // "S - U"
#define IDS_INDEXTAB8                   12      // "V - Z"
#define IDS_INDEXTAB9                   13      // "Misc"
                                                           
#define IDS_FIRSTINDEXTAB               IDS_INDEXTAB1
#define IDS_LASTINDEXTAB                IDS_INDEXTAB9
#define CINDEXTABS \
    (IDS_LASTINDEXTAB-IDS_FIRSTINDEXTAB+1)
#define IDS_INDEXTABMISC                IDS_LASTINDEXTAB

#define IDS_INDEXCHARSALL               15      // "ABC...XYZ"
#define IDS_INDEXCHARS1                 16      // "ABC"
#define IDS_INDEXCHARS2                 17      // "DEF"
#define IDS_INDEXCHARS3                 18      // "GHI"
#define IDS_INDEXCHARS4                 19      // "JKL"
#define IDS_INDEXCHARS5                 20      // "MNO"
#define IDS_INDEXCHARS6                 21      // "PQR"
#define IDS_INDEXCHARS7                 22      // "STU"
#define IDS_INDEXCHARS8                 23      // "VWXYZ"
 
#define IDS_FIRSTINDEXCHARS             IDS_INDEXCHARS1
#define IDS_LASTINDEXCHARS              IDS_INDEXCHARS8
#define CINDEXCHARS \
    (IDS_LASTINDEXCHARS-IDS_FIRSTINDEXCHARS+1)

#define IDS_PERSONNEWTITLE              24      // "Last name..."
#define IDS_ADDRESS                     25      // "Address"
#define IDS_CITY                        26      // "City"
#define IDS_STATE                       27      // "State"
#define IDS_ZIPCODE                     28      // "Zip Code"
#define IDS_PHONE                       29      // "Phone"
#define IDS_DEFAULTPHONE                30      // "(919) 469-6000"
#define IDS_DEFAULTADDRESS              31      // "11000 Regency..."
#define IDS_DEFAULTCITY                 32      // "Cary"
#define IDS_DEFAULTSTATE                34      // "NC"
#define IDS_DEFAULTZIPCODE              35      // "27513"

/*
 * Protocol IDs (used for determining if an object
 * responds to a given set of methods -- see
 * shrUnderstandsProtocol in ShrPerson for more details).
 */

#define PROTOCOL_PERSON                 1

/*
 * Function declarations
 */

extern BOOL ShrSetObjectString
    (SOMAny *somObject, PSZ pszNew, PSZ *ppszOld, ULONG ulChangeMsg);
extern ULONG ShrQueryObjectString
    (SOMAny *, PSZ pszReturn, PSZ *ppsz, PSZ pszDefault);

extern BOOL ShrOpeningView
    (SOMAny *somSelf, HWND hwndFrame, ULONG ulView, PSZ pszViewTitle);
extern BOOL ShrClosingView
    (SOMAny *somSelf, HWND hwndFrame);
extern MRESULT EXPENTRY ShrDefPageDlgProc
    (HWND hwndDlg, ULONG msg, MPARAM mp1, MPARAM mp2);
extern VOID ShrSetDlgEntryFieldText
    (HWND hwndDlg, ULONG id, PSZ psz);
extern VOID ShrSetDlgMLEText
    (HWND hwndDlg, ULONG id, PSZ psz);

extern VOID ShrEmptyObjectCnr(HWND hwndCnr);

extern ULONG APIENTRY ShrExceptionHandler
    (PEXCEPTIONREPORTRECORD pReportRecord, 
    PEXCEPTIONREGISTRATIONRECORD pRegRecord,
    PCONTEXTRECORD pContextRecord, PVOID pReserved);

/*
 * Read-only global strings.
 */

extern CHAR vszShrPersonClass[];
extern CHAR vszShrAddressBookClass[];
extern CHAR vszAddressNotebook[];
extern CHAR vszNullString[];
extern CHAR vszDesktopObjectId[];
extern CHAR vszShrUnderstandsProtocol[];
extern CHAR vszDRMObject[];
extern CHAR vszDRFObject[];
extern CHAR vszWcFrameCnrOwner[];
extern CHAR vszwpModifyPopupMenu[];
extern CHAR vszAppName[];
extern CHAR vszIndexedViewWindowPosKeyName[];

/*
 * Global MRI-related strings.
 */

extern CHAR vszPersonTitle[CCHMINSTRING];
extern CHAR vszAddressBookTitle[CCHMINSTRING];
extern CHAR vszAddressBookViewTitle[CCHMINSTRING];
extern CHAR vszPersonInfoTab[CCHMINSTRING];
extern CHAR vaszIndexTabs[CINDEXTABS][CCHMINSTRING];
extern CHAR vaszIndexChars[CINDEXCHARS][CCHMINSTRING];
extern CHAR vszAllIndexChars[CCHMINSTRING];
extern CHAR vszAddressBookCnrTitle[CCHMINSTRING];
extern CHAR vszPersonNewTitle[CCHMINSTRING];
extern CHAR vszAddress[CCHMINSTRING];
extern CHAR vszCity[CCHMINSTRING];
extern CHAR vszState[CCHMINSTRING];
extern CHAR vszZipCode[CCHMINSTRING];
extern CHAR vszPhone[CCHMINSTRING];
extern CHAR vszDefaultAddress[CCHMINSTRING];
extern CHAR vszDefaultCity[CCHMINSTRING];
extern CHAR vszDefaultState[CCHMINSTRING];
extern CHAR vszDefaultZipCode[CCHMINSTRING];
extern CHAR vszDefaultPhone[CCHMINSTRING];

/*
 * Global PM-related variables.
 */

extern PFNWP vpfnDefaultFrameProc;
extern PFNWP vpfnDefaultNotebookProc;
extern PFNWP vpfnWPSCnrOwnerProc;

/*
 * Global SOM-related variables.
 */

extern somId vmidShrUnderstandsProtocol;

/*
 * Notification message constants
 * (see NOTIFY.CSC for details).
 */

#define SHRN_PERSONNAMECHANGED          (WM_USER+0)
#define SHRN_PERSONADDRESSCHANGED       (WM_USER+1)
#define SHRN_PERSONCITYCHANGED          (WM_USER+2)
#define SHRN_PERSONSTATECHANGED         (WM_USER+3)
#define SHRN_PERSONZIPCODECHANGED       (WM_USER+4)
#define SHRN_PERSONPHONECHANGED         (WM_USER+5)

#define SHRN_ADDRESSBOOKPERSONADDED     (WM_USER+6)
#define SHRN_ADDRESSBOOKPERSONREMOVED   (WM_USER+7)

/*
 * User defined PM messages.
 */

#define SHR_SETOBJECT                   (WM_USER+100)
#define SHR_QUERYOBJECT                 (WM_USER+101)

/*
 * Macros.
 */

#define ShrDefFrameProc(h,m,p1,p2) \
    (*vpfnDefaultFrameProc)(h,m,p1,p2)
#define ShrDefWPSCnrOwnerProc(h,m,p1,p2) \
    (*vpfnWPSCnrOwnerProc)(h,m,p1,p2)

#define ShrRemoveMenuItem(hwndMenu,id) \
    WinSendMsg(hwndMenu,MM_REMOVEITEM,MPFROM2SHORT(id,TRUE),NULL)
#define ShrDeleteMenuItem(hwndMenu,id) \
    WinSendMsg(hwndMenu,MM_DELETEITEM,MPFROM2SHORT(id,TRUE),NULL)

#define BOOLFROMP(p)  ((p) ? TRUE: FALSE)

/*
 * These two simple macros are used to set a trap exception handler.
 * ShrStartTryBlock should be used at the beginning of the function
 * following the data declarations.  ShrEndTryBlock must precede
 * the return statement.
 *
 * OS/2 and the WPS use exception handlers to catch invalid pointers
 * that would otherwise cause a trap.  Exception handlers should be
 * used whenever it is critical that a routine clean up resources:
 *
 * 1. Free allocated memory.
 * 2. Destroy unneeded windows.
 *
 * and MOST IMPORTANTLY...
 *
 * 3. Release owned semaphores.
 *
 * It is very, very important that owned semaphores be released,
 * otherwise others that request the semaphore could wait indefinitely.
 *
 * For example, if an object's semaphore is requested with
 * wpRequestObjectMutexSem, an exception handler must be registered
 * so wpReleaseObjectMutexSem will be called if the routine traps.
 * Otherwise, the WPS will hang when it requests the semaphore.
 *
 * Note: In general, it is wise to use wpRequestObjectMutexSem and
 * wpReleaseObjectMutexSem within your own defined methods which
 * access instance data to assure the data is not modified by more
 * than one thread (eg, the wpMenuItemSelected method is invoked
 * from the main UI thread; wpSaveState is invoked from another
 * lower priority thread).  
 *
 * WARNING: The exception handler chain is corrupted if ShrEndTryBlock
 * is not called before exiting a routine.  Having two return
 * statements per function, one for "no exception" and another for 
 * handling "exception" is a good habit to have in order to
 * avoid this difficult to debug problem, eg:
 *
 *   extern BOOL Trapper(PSZ psz)
 *   {
 *     ShrStartTryBlock;
 *
 *     ...exception handling set, go about in safety...
 *
 *     ShrEndTryBlock;
 *     return TRUE;
 *
 *   OnException:
 *
 *     ...free any resources that were allocated, relinquish
 *        semaphores that were successfully gotten, etc...
 *
 *     ShrEndTryBlock;
 *     return FALSE;
 *   }
 *
 * It is also wise to compile with the /Kb+ option specified
 * to detect uninitialized variables and unprototyped functions.
 */

#define ShrStartTryBlock \
    SHREXCEPTIONREGISTRATIONRECORD shrExceptionRegRecord; \
    shrExceptionRegRecord.prev_structure = NULL; \
    shrExceptionRegRecord.ExceptionHandler = ShrExceptionHandler; \
    DosSetExceptionHandler \
        ((PEXCEPTIONREGISTRATIONRECORD) &shrExceptionRegRecord); \
    if (setjmp(shrExceptionRegRecord.env)) \
      goto OnException;

#define ShrEndTryBlock DosUnsetExceptionHandler  \
    ((PEXCEPTIONREGISTRATIONRECORD) &shrExceptionRegRecord);

