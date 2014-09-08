/*
 * OS/2 Work Place Shell Sample Program - ShrAddressBook model code 
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
#define ShrAddressBook_Class_Source

#include "share.h"
#include "addrbkv.h"
#include "notify.h"
#include "person.h"

/*
 * SOM generates method stubs that
 * cause compiler warnings.  Suspend warnings
 * (but be wary, this can mask errors in the
 * emitted "passthru" sections of the CSC file).
 */

#pragma checkout(suspend)
#include "addrbk.ih"
#pragma checkout(resume)

#include <wpshadow.h>

/*
 * ShrFindAddressBookClass() is a convenience function
 * which returns a pointer to ShrAddressBook class.
 */

extern SOMClass *ShrFindAddressBookClass()
{
  return _somFindClass(SOMClassMgrObject,
      SOM_IdFromString(vszShrAddressBookClass), 0, 0);
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
 *    model (ADDRBK.C) and view (ADDRBKV.C) code.  It also
 *    helps centralize the view code that handles updates 
 *    of what the user sees when required by changes in the model.
 *
 *    See ShrNotifier for more details, especially the 
 *    shrNotifyInterestedWindows method.  Also see the PERSON.CSC
 *    file for more details about this method.
 */

SOM_Scope SOMObject * SOMLINK adr_shrQueryNotifier(ShrAddressBook *somSelf)
{
  ShrAddressBookData *somThis = ShrAddressBookGetData(somSelf);

  if (!_somNotifier)
    _somNotifier = ShrNotifierNew();

  return _somNotifier;
}


/*
 * 
 *  OVERRIDE: wpInitData                                   ( ) PRIVATE
 *                                                         (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Initialize our state variables. Allocate any extra memory that
 *    we might need.
 * 
 */

SOM_Scope void SOMLINK adr_wpInitData(ShrAddressBook *somSelf)
{
  SOMClass *somClass;

  /*
   * Call the parent superclass first.
   */

  parent_wpInitData(somSelf);

  /*
   * We're only interested in displaying the details of ShrPerson
   * objects.  By specifying ShrPerson as the details class for
   * the folder, only the details as determined by ShrPerson's
   * wpclsQueryDetailsInfo method are shown in the folder's
   * "Details" view (in this case, "Phone", "Address", etc).
   */

  somClass = ShrFindPersonClass();

  if (somClass)
    _wpSetFldrDetailsClass(somSelf, somClass);
}

/*
 * 
 *  OVERRIDE: wpModifyPopupMenu                             ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Add our extra option(s) to the context menu.
 * 
 */

SOM_Scope BOOL SOMLINK adr_wpModifyPopupMenu
    (ShrAddressBook *somSelf, HWND hwndMenu, HWND hwndCnr, ULONG iPosition)
{
  BOOL bSuccess;

  /*
   * We would like to insert "Create person" following the "Open"
   * choice, ie:
   *
   *  +----------------+
   *  |Open           >|
   *  |Create person   |
   *  |Refresh         |
   *  |----------------|
   *  |Create another >|
   * ///   ...etc...  ///
   *  |----------------|
   *  |Arrange         |
   *  +----------------+
   *                   
   * But because of a WPS bug, the iPosition parameter in
   * wpInsertPopupMenuItems is ignored.  Therefore, "Create person"
   * is appended after the standard edit menu choices.
   */                                        
                       
  _wpInsertPopupMenuItems(somSelf, hwndMenu, 1,
      _shrclsQueryModuleHandle(_somGetClass(somSelf)),
       IDM_ADDRESSBOOK, 0);

  _wpInsertPopupMenuItems(somSelf, hwndMenu, 0,
        _shrclsQueryModuleHandle(_somGetClass(somSelf)),
        IDM_OPENADDRESSBOOK, WPMENUID_OPEN);

  bSuccess = parent_wpModifyPopupMenu(somSelf,
        hwndMenu, hwndCnr, iPosition);

  return bSuccess;
}

/*
 * 
 *  OVERRIDE: wpMenuItemSelected                            ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Process input from the extra menu option that we added.
 * 
 */

SOM_Scope BOOL SOMLINK adr_wpMenuItemSelected
    (ShrAddressBook *somSelf, HWND hwndFrame, ULONG ulMenuId)
{
  BOOL bSuccess;
  SOMClass *somClass;

  switch (ulMenuId)
  {
    case ID_OPENADDRESSINDEXVIEW:
       /*
        * We could call wpOpen here, but, if the object is already opened,
        * the following API determines whether the object should be
        * resurfaced, or if multiple views are desired.
        */

       if (!_wpViewObject(somSelf, NULLHANDLE, 
           ID_OPENADDRESSINDEXVIEW, 0))
         bSuccess = FALSE;
       break;

    case ID_CREATEPERSON:
      /*
       * Create a new instance of a person for the address book.
       */

      somClass = ShrFindPersonClass();

      if (somClass)
        _wpclsNew(somClass, vszPersonNewTitle, NULL, somSelf, TRUE);
      break;

    default:
       bSuccess = 
           parent_wpMenuItemSelected(somSelf, hwndFrame, ulMenuId);
       break;
  }

  return bSuccess;
}

/*
 * 
 *  OVERRIDE: wpMenuItemHelpSelected                        ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Process input from the extra menu option that we added.
 * 
 */

SOM_Scope BOOL SOMLINK adr_wpMenuItemHelpSelected
    (ShrAddressBook *somSelf, ULONG MenuId)
{
  /*
   * Process help requests on menu options we added
   * (ID_OPENADDRESSINDEXVIEW and ID_CREATEPERSON, see
   * wpMenuItemSelected above).
   *
   * On-line help is not provided in the SHARE package,
   * but here's where calls to wpDisplayHelp would be added.
   */

  return (parent_wpMenuItemHelpSelected(somSelf,MenuId));
}

/*
 * 
 *   OVERRIDE: wpOpen                                       ( ) PRIVATE
 *                                                          (X) PUBLIC
 *   DESCRIPTION:
 * 
 *     Opens the object.
 * 
 */

SOM_Scope HWND SOMLINK adr_wpOpen
    (ShrAddressBook *somSelf, HWND hwndCnr, ULONG ulView, ULONG param)
{
  HWND hwndFrame;

  /*
   * Our superclass, WPFolder, provides Icons, Details, Tree,
   * and Settings view.  ShrAddressBook's view code adds the
   * "Indexed View".  If the user has requested Indexed view
   * (either by double-clicking or choosing it from the menu),
   * ask ShrAddressBook's view code to display a new view
   * (see ADDRBKV.C for more details).
   */

  if (ulView == ID_OPENADDRESSINDEXVIEW)
    hwndFrame = ShrOpenAddressBook(somSelf);
  else
    hwndFrame = parent_wpOpen(somSelf, hwndCnr, ulView, param);

  return hwndFrame;
}

/*
 * 
 *   OVERRIDE: shrAddedToContent                            ( ) PRIVATE
 *             shrRemovedFromContent                        (X) PUBLIC
 *                                              
 *   DESCRIPTION:
 * 
 *     Update the view when an object is added or remove from
 *     the address book model.
 * 
 */

SOM_Scope VOID SOMLINK adr_shrAddedToContent
   (ShrAddressBook *somSelf, WPObject *somAddedObject)
{
  _shrNotifyInterestedWindows(_shrQueryNotifier(somSelf),
      somSelf, SHRN_ADDRESSBOOKPERSONADDED, somSelf, somAddedObject);
}

SOM_Scope VOID SOMLINK adr_shrRemovedFromContent
   (ShrAddressBook *somSelf, WPObject *somRemovedObject)
{
  _shrNotifyInterestedWindows(_shrQueryNotifier(somSelf),
       somSelf, SHRN_ADDRESSBOOKPERSONREMOVED, somSelf,
       somRemovedObject);
}

/*
 * 
 *   OVERRIDE: wpDragOver                                   ( ) PRIVATE
 *                                                          (X) PUBLIC
 *   DESCRIPTION:
 * 
 *     Gives the object a chance to accept/reject objects
 *     being dragged over it.
 * 
 */

SOM_Scope MRESULT SOMLINK adr_wpDragOver
    (ShrAddressBook *somSelf, HWND hwndCnr, PDRAGINFO pDragInfo)
{
  PDRAGITEM pDragItem;
  ULONG i;
  USHORT usDrop;
  USHORT usDefaultOp;
  MRESULT mr;

  mr = parent_wpDragOver(somSelf, hwndCnr, pDragInfo);
  
  usDrop = SHORT1FROMMP(mr);
  usDefaultOp = SHORT2FROMMP(mr);
  
  for (i=0; usDrop == DOR_DROP && i < pDragInfo->cditem; i++)
  {
    pDragItem = DrgQueryDragitemPtr(pDragInfo, i);

    /*
     * For each drag item, verify it is a WPS object
     * ("DRM_OBJECT","DRF_OBJECT").  If it is, pass it
     * along to the shrclsValidateObject method for more
     * careful evaluation.
     */

    if (DrgVerifyRMF(pDragItem, vszDRMObject, vszDRFObject))
    {
      if (!_shrclsValidateObject(ShrFindAddressBookClass(),
          OBJECT_FROM_PREC(pDragItem->ulItemID)))
        usDrop = DOR_NEVERDROP;
    }
    else
    {
      usDrop = DOR_NEVERDROP;
    }
  }

  return MRFROM2SHORT(usDrop, usDefaultOp);
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

SOM_Scope HMODULE SOMLINK adrM_shrclsQueryModuleHandle
    (M_ShrAddressBook *somSelf)
{
  M_ShrAddressBookData *somThis = M_ShrAddressBookGetData(somSelf); 
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
 *  METHOD: shrclsValidateObject                            ( ) PRIVATE
 *                                                          (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Returns the given object if it acceptable, otherwise
 *    it returns NULL.
 */

SOM_Scope SOMObject * SOMLINK adrM_shrclsValidateObject
    (M_ShrAddressBook *somSelf, SOMAny *somObject)
{
  SOMAny *somTestObject;
  SOMAny *somShadowedObject = NULL;

  /*
   * ShrAddressBook, a subclass of WPFolder, accepts only
   * ShrPerson objects or shadows of ShrPerson objects.
   *
   * ShrAddressBook is an example of "restrictive containment".
   * It will hold objects, but only those of a specific type.
   * There are other examples of restrictive containment in
   * the OS/2 WPS.  The Printer object only allows print jobs
   * within it; the Minimized Window Viewer only allows
   * minimized windows within it.
   *
   * Restrictive containment can effectively be used to guide
   * the user to the purpose of a container.  But be certain
   * that such restriction is considered reasonable to the user.
   *
   * Accept either person's or shadows of persons.
   */

  if (_somIsA(somObject, _WPShadow))
    somTestObject = somShadowedObject =  
        _wpQueryShadowedObject(somObject, TRUE);
  else
    somTestObject = somObject;

  /*
   * Verify the test object is a person by checking that it
   * responds to "shrUnderstandsProtocol" and its confirms
   * its protocol is that of a person object.
   *
   * Note protocol verification as done below is more flexible
   * than using "_somIsA".  For more details, see
   * "Determining an Object's Protocol" in README.TXT.
   */

  if (somTestObject)
  {
    if (!(_somRespondsTo(somTestObject, vmidShrUnderstandsProtocol) &&
        _shrUnderstandsProtocol(somTestObject, PROTOCOL_PERSON)))
      somTestObject = NULL;
  }

  /*
   * Don't forget to unlock the shadow if we got one, otherwise
   * it will never be made dormant (ie, "garbage collected").
   */

  if (somShadowedObject)
    _wpUnlockObject(somShadowedObject);

  return somTestObject;
}

/*
 * 
 *  OVERRIDE: wpclsQueryTitle                              ( ) PRIVATE
 *                                                         (X) PUBLIC
 *  DESCRIPTION:
 * 
 *    Return the string "Address Book"
 * 
 */

SOM_Scope PSZ SOMLINK adrM_wpclsQueryTitle(M_ShrAddressBook *somSelf)
{
  return vszAddressBookTitle;
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

SOM_Scope ULONG SOMLINK adrM_wpclsQueryIconData
    (M_ShrAddressBook *somSelf, PICONINFO pIconInfo)
{
  if (pIconInfo)
  {
     pIconInfo->fFormat = ICON_RESOURCE;
     pIconInfo->hmod = _shrclsQueryModuleHandle(somSelf);
     pIconInfo->resid = IDP_ADDRESSBOOK;
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

SOM_Scope ULONG SOMLINK adrM_wpclsQueryDefaultView(M_ShrAddressBook *somSelf)
{
  return ID_OPENADDRESSINDEXVIEW;
}
