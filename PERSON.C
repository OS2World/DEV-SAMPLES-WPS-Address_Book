/*
 * OS/2 Work Place Shell Sample Program - ShrPerson model code
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
#include "notify.h"
#include "personv.h"

/*
 * SOM generates method stubs that
 * cause compiler warnings.  Suspend warnings
 * (but be wary, this can mask errors in the
 * emitted "passthru" sections of the CSC file).
 */

#define ShrPerson_Class_Source
#pragma checkout(suspend)
#include "person.ih"
#pragma checkout(resume)

/*
 * ShrFindPersonClass() is a convenience function
 * which returns a pointer to ShrPerson class.
 */

extern SOMClass *ShrFindPersonClass()
{
  return _somFindClass(SOMClassMgrObject,
      SOM_IdFromString(vszShrPersonClass), 0, 0);
}

/*
 * 
 *  METHOD: shrQuery...                                     ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 *
 *    Return the requested instance variable value.
 *
 */

SOM_Scope ULONG SOMLINK per_shrQueryAddress
    (ShrPerson *somSelf, ULONG cchAddressMax, PCHAR pchAddress)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrQueryObjectString(somSelf, pchAddress,
      &_pszAddress, vszDefaultAddress);
}

SOM_Scope ULONG SOMLINK per_shrQueryCity
    (ShrPerson *somSelf, ULONG cchCityMax, PCHAR pchCity)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrQueryObjectString(somSelf, pchCity, 
      &_pszCity, vszDefaultCity);
}

SOM_Scope ULONG SOMLINK per_shrQueryState
    (ShrPerson *somSelf, ULONG cchStateMax, PCHAR pchState)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrQueryObjectString(somSelf, pchState,
      &_pszState, vszDefaultState);
}

SOM_Scope ULONG SOMLINK per_shrQueryZipCode
    (ShrPerson *somSelf, ULONG cchZipCodeMax, PCHAR pchZipCode)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrQueryObjectString(somSelf, pchZipCode,
      &_pszZipCode, vszDefaultZipCode);
}

SOM_Scope ULONG SOMLINK per_shrQueryPhone
    (ShrPerson *somSelf, ULONG cchPhoneMax, PCHAR pchPhone)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrQueryObjectString(somSelf, pchPhone,
      &_pszPhone, vszDefaultPhone);
}

/*
 * 
 *  METHOD: shrSet...                                       ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 *
 *    Set the requested instance variable value.
 *
 */

SOM_Scope BOOL SOMLINK per_shrSetAddress
    (ShrPerson *somSelf, PSZ pszNewAddress)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrSetObjectString(somSelf, pszNewAddress, 
      &_pszAddress, SHRN_PERSONADDRESSCHANGED);
}

SOM_Scope BOOL SOMLINK per_shrSetCity
    (ShrPerson *somSelf, PSZ pszNewCity)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrSetObjectString(somSelf, pszNewCity,
      &_pszCity, SHRN_PERSONCITYCHANGED);
}

SOM_Scope BOOL SOMLINK per_shrSetState
    (ShrPerson *somSelf, PSZ pszNewState)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrSetObjectString(somSelf, pszNewState,
      &_pszState, SHRN_PERSONSTATECHANGED);
}


SOM_Scope BOOL SOMLINK per_shrSetZipCode
    (ShrPerson *somSelf, PSZ pszNewZipCode)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrSetObjectString(somSelf, pszNewZipCode,
      &_pszZipCode, SHRN_PERSONZIPCODECHANGED);
}


SOM_Scope BOOL SOMLINK per_shrSetPhone
    (ShrPerson *somSelf, PSZ pszNewPhone)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  return ShrSetObjectString(somSelf, pszNewPhone,
      &_pszPhone, SHRN_PERSONPHONECHANGED);
}

/*
 * 
 *  METHOD: shrQueryNotifier                                ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Return an instance of a ShrNotifier.  Windows (views)
 *    register interest in this object with the notifier.
 *    When something is changed in this object that affects
 *    the view(s), they are notified.
 *
 *    Using this notification scheme better separates the
 *    model (PERSON.C) and view (PSERONV.C) code.  It also
 *    helps centralize the view code that handles updates 
 *    of what the user sees when required by changes in the model.
 *
 *    See ShrNotifier for more details, especially the 
 *    shrNotifyInterestedWindows method.  Also see the PERSON.CSC
 *    file for more details about this method.
 */

SOM_Scope SOMObject * SOMLINK per_shrQueryNotifier(ShrPerson *somSelf)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);
  BOOL bHoldingSem = FALSE;

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

  bHoldingSem = !_wpRequestObjectMutexSem(somSelf, SEM_INDEFINITE_WAIT);

  if (bHoldingSem)
  {
    if (!_somNotifier)
      _somNotifier = ShrNotifierNew();

    bHoldingSem = _wpReleaseObjectMutexSem(somSelf);
  }

  ShrEndTryBlock;
  return _somNotifier;

  /*
   * Exception handling code for this function goes here.
   */

OnException:
  if (bHoldingSem)
    bHoldingSem = _wpReleaseObjectMutexSem(somSelf);

  ShrEndTryBlock;
  return NULL;
}

/*
 * 
 *  METHOD: shrUnderstandsProtocol                          ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    This a more flexible method of dynamically determining an 
 *    object protocol (or "type) than _somIsA.  
 *
 *    Developers may want to implement a new-and-improved version 
 *    of this class object that is not a direct descendant of this 
 *    class' superclass.  This would not be possible if client 
 *    classes use _somIsA.  
 *
 *    But note there is a problem -- this class may have methods 
 *    that are not "name lookup" methods, ie, they use the offset 
 *    method, so creating a new class that is not a subclass of 
 *    this class's superclass would require considerable care 
 *    since it would have to be certain to have its version of 
 *    this class has its methods in the same order as the method 
 *    table of this class.  
 *
 */

SOM_Scope BOOL SOMLINK per_shrUnderstandsProtocol
    (ShrPerson *somSelf, ULONG idProtocol)
{
  return (idProtocol == PROTOCOL_PERSON);
}

/*
 * 
 *  OVERRIDE: wpSetTitle                                   ( ) PRIVATE
 *                                                         (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Notify interested windows that the object's title has changed.
 * 
 *    This is necessary to keep the "Name:" field of the "Info" page
 *    on the Settings view in sync if the user uses direct edit on
 *    the person icon (Alt+MB1), or modifies the "Title:" field on 
 *    the "General" page of the Settings view to change the object
 *    title.
 */

SOM_Scope BOOL SOMLINK per_wpSetTitle
    (ShrPerson *somSelf, PSZ pszNewTitle)
{
  BOOL bSuccess;

  bSuccess = parent_wpSetTitle(somSelf,pszNewTitle);

  if (bSuccess)
    _shrNotifyInterestedWindows(_shrQueryNotifier(somSelf),
        somSelf, SHRN_PERSONNAMECHANGED, somSelf, pszNewTitle);

  return bSuccess;
}

/*
 * 
 *  OVERRIDE: wpUnInitData                                  ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Clear up memory that was allocated on wpInitData.
 * 
 */

SOM_Scope void SOMLINK per_wpUnInitData(ShrPerson *somSelf)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  if (_somNotifier)
    _somFree(_somNotifier);

  if (_pszAddress)
    _wpFreeMem(somSelf, _pszAddress);

  if (_pszCity)
    _wpFreeMem(somSelf, _pszCity);

  if (_pszState)
    _wpFreeMem(somSelf, _pszState);

  if (_pszPhone)
    _wpFreeMem(somSelf, _pszPhone);

  parent_wpUnInitData(somSelf);
}

/*
 * 
 *  OVERRIDE: wpSaveState                                   ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Save our state variables.
 * 
 */

SOM_Scope BOOL SOMLINK per_wpSaveState(ShrPerson *somSelf)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);

  if (_pszAddress)
    _wpSaveString(somSelf, vszShrPersonClass, IDK_ADDRESS, _pszAddress);

  if (_pszCity)
    _wpSaveString(somSelf, vszShrPersonClass, IDK_CITY, _pszCity);

  if (_pszState)
    _wpSaveString(somSelf, vszShrPersonClass, IDK_STATE, _pszState);

  if (_pszZipCode)
    _wpSaveString(somSelf, vszShrPersonClass, IDK_ZIPCODE, _pszZipCode);

  if (_pszPhone)
    _wpSaveString(somSelf, vszShrPersonClass, IDK_PHONE, _pszPhone);

  return (parent_wpSaveState(somSelf));
}

/*
 * 
 *  OVERRIDE: wpRestoreState                                ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Retrieve our saved state variables.
 * 
 */

SOM_Scope BOOL SOMLINK per_wpRestoreState
    (ShrPerson *somSelf, ULONG ulReserved)
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);
  ULONG cbData;

  if (_wpRestoreString(somSelf, vszShrPersonClass, 
      IDK_ADDRESS, NULL, &cbData))
  {
    _pszAddress = _wpAllocMem(somSelf, cbData, NULL);

    if (_pszAddress)
      _wpRestoreString(somSelf, vszShrPersonClass, IDK_ADDRESS,
          _pszAddress, &cbData);
  }

  if (_wpRestoreString(somSelf, vszShrPersonClass, 
      IDK_CITY, NULL, &cbData))
  {
    _pszCity = _wpAllocMem(somSelf, cbData, NULL);

    if (_pszCity)
      _wpRestoreString(somSelf, vszShrPersonClass, IDK_CITY,
          _pszCity, &cbData);
  }

  if (_wpRestoreString(somSelf, vszShrPersonClass, 
      IDK_STATE, NULL, &cbData))
  {
    _pszState = _wpAllocMem(somSelf, cbData, NULL);

    if (_pszState)
      _wpRestoreString(somSelf, vszShrPersonClass, IDK_STATE,
          _pszState, &cbData);
  }

  if (_wpRestoreString(somSelf, vszShrPersonClass, 
      IDK_ZIPCODE, NULL, &cbData))
  {
    _pszZipCode = _wpAllocMem(somSelf, cbData, NULL);

    if (_pszZipCode)
      _wpRestoreString(somSelf, vszShrPersonClass, IDK_ZIPCODE,
          _pszZipCode, &cbData);
  }

  if (_wpRestoreString(somSelf, vszShrPersonClass, 
      IDK_PHONE, NULL, &cbData))
  {
    _pszPhone = _wpAllocMem(somSelf, cbData, NULL);

    if (_pszPhone)
      _wpRestoreString(somSelf, vszShrPersonClass, IDK_PHONE,
          _pszPhone, &cbData);
  }

  return (parent_wpRestoreState(somSelf,ulReserved));
}

/*
 * 
 *  METHOD: shrclsAddPersonInfoPage                         ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Adds the "Info" page to the Settings view notebook.
 *    Also see the override of wpAddSettingsPages.
 *
 */

SOM_Scope BOOL SOMLINK per_shrAddPersonInfoPage
    (ShrPerson *somSelf, HWND hwndNotebook)
{
  PAGEINFO pageinfo;

  memset(&pageinfo, 0, sizeof(PAGEINFO));
  pageinfo.cb = sizeof(PAGEINFO);
  pageinfo.usPageStyleFlags = BKA_MAJOR;
  pageinfo.usPageInsertFlags = BKA_FIRST;
  pageinfo.resid = _shrclsQueryModuleHandle(_somGetClass(somSelf));
  pageinfo.pCreateParams = somSelf;

  pageinfo.pszName = vszPersonInfoTab;
  pageinfo.dlgid = IDD_PERSONINFOPAGE;
  pageinfo.pfnwp = ShrPersonInfoPageDlgProc;

  return _wpInsertSettingsPage(somSelf, hwndNotebook, &pageinfo);
}

/*
 * 
 *   OVERRIDE: wpAddSettingsPages                           ( ) PRIVATE
 *                                                          (X) PUBLIC
 *   DESCRIPTION:
 * 
 *     Notification to add settings pages.
 * 
 */

SOM_Scope BOOL SOMLINK per_wpAddSettingsPages
    (ShrPerson *somSelf, HWND hwndNotebook)
{
  BOOL bSuccess;

  bSuccess = parent_wpAddSettingsPages(somSelf, hwndNotebook);

  if (bSuccess)
    _shrAddPersonInfoPage(somSelf, hwndNotebook);

  return bSuccess;
}

/*
 */
/*
 * 
 *   OVERRIDE: wpQueryDetailsData                           ( ) PRIVATE
 *                                                          (X) PUBLIC
 *   DESCRIPTION:
 * 
 *     Return the data for displaying the person in details
 *     view of a folder.
 *
 *     This method must return data matching the structure
 *     described in the wpclsQueryDetailsInfo override.
 * 
 */

SOM_Scope BOOL SOMLINK per_wpQueryDetailsData
    (ShrPerson *somSelf, PVOID *ppDetailsData, PULONG pcb) 
{
  ShrPersonData *somThis = ShrPersonGetData(somSelf);
  BOOL bSuccess;
  PPERSONDETAILS   pPersonDetails;
  ULONG cbSuperclassDetails;

  if (!ppDetailsData) 
  {                    
    /*
     * Only interested in the length of the details data.
     */

    bSuccess = parent_wpQueryDetailsData(somSelf, NULL, pcb);
    *pcb += sizeof(PERSONDETAILS);
  }
  else
  {
    bSuccess = parent_wpQueryDetailsData(somSelf, ppDetailsData, pcb);

    if (bSuccess)
    {
      /*
       * Next see if buffer is big enough to also accomodate our   
       * portion of details data.                                  
       */                                                          
  
      parent_wpQueryDetailsData(somSelf, NULL, &cbSuperclassDetails);
  
      if ((cbSuperclassDetails + sizeof(PERSONDETAILS)) > *pcb)
      {
        bSuccess = FALSE;
      }
      else
      {
        /*
         * Fill in the details for this object.
         */

        pPersonDetails = (PPERSONDETAILS) *ppDetailsData;

        /*
         * If nothing has been set, return the defaults.
         * We could store the defaults in each object
         * by setting these instance varibles in wpInitData,
         * but that would waste memory and disk space.
         *
         * Note the Details view fields are read-only.  If
         * that were to change, we would have to allocate
         * space for each default field to avoid the
         * global MRI variables being modified.
         */

        if (_pszAddress)
          pPersonDetails->pszAddress = _pszAddress;
        else
          pPersonDetails->pszAddress = vszDefaultAddress;

        if (_pszCity)
          pPersonDetails->pszCity = _pszCity;
        else
          pPersonDetails->pszCity = vszDefaultCity;

        if (_pszState)
          pPersonDetails->pszState = _pszState;
        else
          pPersonDetails->pszState = vszDefaultState;

        if (_pszZipCode)
          pPersonDetails->pszZipCode = _pszZipCode;
        else
          pPersonDetails->pszZipCode = vszDefaultZipCode;

        if (_pszPhone)
          pPersonDetails->pszPhone = _pszPhone;
        else
          pPersonDetails->pszPhone = vszDefaultPhone;
      
        *ppDetailsData = pPersonDetails+1;
      }
    }
  }

  return bSuccess;
}

/*
 * Class Methods.
 *
 * IMPORTANT! IMPORTANT! IMPORTANT! IMPORTANT! IMPORTANT!
 * IMPORTANT! IMPORTANT! IMPORTANT! IMPORTANT! IMPORTANT!
 * IMPORTANT! IMPORTANT! IMPORTANT! IMPORTANT! IMPORTANT!
 *
 * The "undef/define" below marks the begining of the
 * class methods.  SOM's macros will generate the
 * wrong code if instance methods are defined after
 * this point (causing strange runtime traps).
 *
 * Be careful when adding new INSTANCE methods to the CSC file because
 * they are automatically appended to the end of the C file, but
 * must appear above the "#undef SOM_CurrentClass".
 */

#undef SOM_CurrentClass
#define SOM_CurrentClass SOMMeta

/*
 * 
 *  METHOD: shrclsQueryModuleHandle                         ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    This method returns the module handle of this class.  If this is the
 *    first invocation, DosQueryModuleHandle is called to save the handle
 *    for future invocations.
 * 
 *  RETURN:
 * 
 *    0              Unsuccessful
 *    non-zero       module handle
 * 
 */

SOM_Scope HMODULE SOMLINK perM_shrclsQueryModuleHandle
    (M_ShrPerson *somSelf)
{
  M_ShrPersonData *somThis = M_ShrPersonGetData(somSelf); 
  APIRET rc = 0;
  PSZ pszPathName;

  if (_hmod == NULLHANDLE)
  {
    pszPathName = _somLocateClassFile(SOMClassMgrObject, 
        SOM_IdFromString(_somGetName(somSelf)), 0, 0);
    rc = DosQueryModuleHandle(pszPathName, &_hmod);
  }

  return (rc ? NULLHANDLE : _hmod);
}

/*
 * 
 *  OVERRIDE: wpclsInitData
 * 
 *  DESCRIPTION:
 * 
 *    Initalize the class data
 * 
 */

SOM_Scope void SOMLINK perM_wpclsInitData(M_ShrPerson *somSelf)
{
  M_ShrPersonData *somThis = M_ShrPersonGetData(somSelf); 
  ULONG i;

  /*
   * Initialize the class field info details (see
   * wpclsQueryDetailsInfo for more information).
   */

  memset(&_acfiPerson, 0, sizeof(_acfiPerson));

  for (i=0; i < NUM_PERSONDETAILS_FIELDS; i++)
  {
    /*
     * The class field information for this class is 
     * simple -- just strings.
     */

    _acfiPerson[i].cb = sizeof(CLASSFIELDINFO);
    _acfiPerson[i].flData = CFA_STRING | CFA_LEFT |
        CFA_SEPARATOR | CFA_FIREADONLY;
    _acfiPerson[i].flTitle = CFA_CENTER | CFA_SEPARATOR | 
        CFA_HORZSEPARATOR | CFA_STRING | CFA_FITITLEREADONLY;
    _acfiPerson[i].flCompare = COMPARE_SUPPORTED | SORTBY_SUPPORTED;
    _acfiPerson[i].DefaultComparison = CMP_GREATER;                 
    _acfiPerson[i].ulLenFieldData = sizeof(PSZ);
    _acfiPerson[i].ulLenCompareValue = CCHMINSTRING;                
  }

  /*
   * Chain the class field information together, making certain
   * the last is NULL.  The order they are chained will determine
   * the order they appear in the details columns.
   */

  _acfiPerson[0].pTitleData = vszPhone;
  _acfiPerson[0].offFieldData = FIELDOFFSET(PERSONDETAILS, pszPhone);
  _acfiPerson[0].pNextFieldInfo = &_acfiPerson[1];

  _acfiPerson[1].pTitleData = vszAddress;
  _acfiPerson[1].offFieldData = FIELDOFFSET(PERSONDETAILS, pszAddress);
  _acfiPerson[1].pNextFieldInfo = &_acfiPerson[2];

  _acfiPerson[2].pTitleData = vszCity;
  _acfiPerson[2].offFieldData = FIELDOFFSET(PERSONDETAILS, pszCity);
  _acfiPerson[2].pNextFieldInfo = &_acfiPerson[3];
 
  _acfiPerson[3].pTitleData = vszState;
  _acfiPerson[3].offFieldData = FIELDOFFSET(PERSONDETAILS, pszState);
  _acfiPerson[3].pNextFieldInfo = &_acfiPerson[4];

  _acfiPerson[4].pTitleData = vszZipCode;
  _acfiPerson[4].offFieldData = FIELDOFFSET(PERSONDETAILS, pszZipCode);
  _acfiPerson[4].pNextFieldInfo = NULL;

  parent_wpclsInitData(somSelf);
}

SOM_Scope ULONG SOMLINK perM_wpclsQueryDetailsInfo
    (M_ShrPerson *somSelf, PCLASSFIELDINFO 
    *ppClassFieldInfo, PULONG pSize)
{
  M_ShrPersonData *somThis = M_ShrPersonGetData(somSelf); 
  ULONG ulSuperclassColumns, i;
  PCLASSFIELDINFO pCFI;

  ulSuperclassColumns = parent_wpclsQueryDetailsInfo(somSelf,
      ppClassFieldInfo, pSize);

  /*
   * If the request was for the size of the details column
   * data.  Add the size needed by the person details.
   */

  if (pSize)
    *pSize += sizeof(PERSONDETAILS);

  /* If the request was for the chained CLASSFIELDINFO structures,
   * link ShrPerson's into the chain.
   *
   * If the beginning of the chain is NULL, assign the address
   * of ShrPerson's first CLASSFIELDINFO structure to 
   * *ppClassFieldInfo.  Otherwise *ppClassFieldInfo points 
   * to the first column description in the chain.  We need 
   * to walk the chain and link our CLASSFIELDINFO structures 
   * at the end.
   */

  if (ppClassFieldInfo)
  {
    pCFI = *ppClassFieldInfo;
  
    if (pCFI)
    {
      for (i=0; i < ulSuperclassColumns; i++)
        if (pCFI->pNextFieldInfo)
          pCFI = pCFI->pNextFieldInfo;
  
      pCFI->pNextFieldInfo = _acfiPerson;
    }
    else
    {
      *ppClassFieldInfo = _acfiPerson;
    }
  }

  return ulSuperclassColumns + NUM_PERSONDETAILS_FIELDS;
}

/*
 * 
 *  OVERRIDE: wpclsQueryTitle                              ( ) PRIVATE
 *                                                         (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Return the string "Person".
 * 
 */

SOM_Scope PSZ SOMLINK perM_wpclsQueryTitle(M_ShrPerson *somSelf)
{
  return vszPersonTitle;
}

/*
 * 
 *  OVERRIDE: wpclsQueryIconData                           ( ) PRIVATE
 *                                                         (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Return the class icon
 * 
 */

SOM_Scope ULONG SOMLINK perM_wpclsQueryIconData
    (M_ShrPerson *somSelf, PICONINFO pIconInfo)
{
  if (pIconInfo)
  {
     pIconInfo->fFormat = ICON_RESOURCE;
     pIconInfo->hmod = _shrclsQueryModuleHandle(somSelf);
     pIconInfo->resid = IDP_PERSON;
  }

  return (sizeof(ICONINFO));
}

/*
 * 
 *  OVERRIDE: wpclsQueryDefaultView                        ( ) PRIVATE
 *                                                         (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Returns the default view for a new instance of this object.
 * 
 */

SOM_Scope ULONG SOMLINK perM_wpclsQueryDefaultView(M_ShrPerson *somSelf)
{
  return OPEN_SETTINGS;
}
