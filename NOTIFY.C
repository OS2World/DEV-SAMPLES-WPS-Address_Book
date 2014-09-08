/*
 * OS/2 Work Place Shell Sample Program - ShrNotifier model code
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

#define ShrNotifier_Class_Source
#define SOM_NoTest

#include "share.h"
#include "somlist.h"
#include "shrfoldr.h"

/*
 * SOM generates method stubs that
 * cause compiler warnings.  Suspend warnings
 * (but be wary, this can mask errors in the
 * emitted "passthru" sections of the CSC file).
 */

#pragma checkout(suspend)
#include "notify.ih"
#pragma checkout(resume)

/*
 * 
 *  METHOD: shrQueryWindowNotifyList                        (X) PRIVATE
 *                                                          ( ) PUBLIC
 *  DESCRIPTION:
 * 
 *    Lazy query of the window notify list which holds the
 *    window handles of view interested in significant
 *    changes in an observable model.
 *
 */

SOM_Scope SOMObject * SOMLINK ntf_shrQueryWindowNotifyList
    (ShrNotifier *somSelf)
{
  ShrNotifierData *somThis = ShrNotifierGetData(somSelf);

  if (!_somWindowNotifyList)
    _somWindowNotifyList = _shrclsValueListNew(ShrListNewClass(0,0));

  return _somWindowNotifyList;
}

/*
 * 
 *  METHOD: shrAddInterestedWindow                         ( ) PRIVATE
 *          shrRemoveInterestedWindow                      (X) PUBLIC
 *                                         
 *  DESCRIPTION:
 * 
 *    Ask the notifier to add or remove interest in 
 *    significant changes in the model.
 */

SOM_Scope BOOL SOMLINK ntf_shrAddInterestedWindow
    (ShrNotifier *somSelf, HWND hwndInterested)
{
  return _shrAddLast(_shrQueryWindowNotifyList(somSelf), 
      (PVOID) hwndInterested);
}

SOM_Scope BOOL SOMLINK ntf_shrRemoveInterestedWindow
    (ShrNotifier *somSelf, HWND hwndInterested)
{
  return _shrRemove(_shrQueryWindowNotifyList(somSelf),
      (PVOID) hwndInterested);
}

/*
 * 
 *  METHOD: shrNotifyInterestedWindow                       ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Tells windows (views) that have registered with the
 *    shrNotifyInterestedWindows that a significant change
 *    has occurred to the model they are monitoring.
 *
 */

SOM_Scope ULONG SOMLINK ntf_shrNotifyInterestedWindows
    (ShrNotifier *somSelf, SOMAny *somChangedObject, 
    ULONG ulChangeMsg, MPARAM mp1, MPARAM mp2)
{
  ShrNotifierData *somThis = ShrNotifierGetData(somSelf);
  HWND hwndInterested = NULLHANDLE;
  ULONG cInterested = 0;
  PVOID pEnum;

  if (_somWindowNotifyList)
  {
    pEnum = _shrBeginEnum(_somWindowNotifyList);

    while (_shrNext(_somWindowNotifyList, pEnum, 
        (PVOID *) &hwndInterested))
    {
      cInterested++;
      WinSendMsg(hwndInterested, ulChangeMsg, mp1, mp2);
    }

    _shrEndEnum(_somWindowNotifyList, pEnum);
  }

  ShrCnrRefreshDetails(somChangedObject);

  return cInterested;
}

/*
 * 
 *  OVERRIDE: somUninit                                     ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Free resources before going away.
 *
 */

SOM_Scope VOID SOMLINK ntf_somUninit(ShrNotifier *somSelf)
{
  ShrNotifierData *somThis = ShrNotifierGetData(somSelf);

  if (_somWindowNotifyList)
    _somFree(_somWindowNotifyList);

  parent_somUninit(somSelf);
}
