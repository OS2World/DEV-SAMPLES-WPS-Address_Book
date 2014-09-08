#***********************************************************************
#
#  OS/2 Work Place Shell Sample Program - ShrPerson
#
#  Copyright (C) 1993 IBM Corporation
#
#    DISCLAIMER OF WARRANTIES.  The following [enclosed] code is
#    sample code created by IBM Corporation.  This sample code is
#    not part of any standard or IBM product and is provided to you
#    solely for the purpose of assisting you in the development of
#    your applications.  The code is provided "AS IS".  ALL
#    WARRANTIES ARE EXPRESSLY DISCLAIMED, INCLUDING THE IMPLIED
#    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#    PURPOSE.  IBM shall not be liable for any damages arising out
#    of your use of the sample code, even if IBM has been advised of
#    the possibility of such damages.
#
#
#***********************************************************************

#***********************************************************************
#   Include the class definition file for the parent class
#***********************************************************************

include <wpabs.sc>

#***********************************************************************
#   Define the new class
#***********************************************************************

class: ShrPerson,
       external stem   = per,
       local,
       external prefix = per_,
       classprefix     = perM_,
       major version   = 1,
       minor version   = 2;

--
-- CLASS: ShrPerson
--
-- CLASS HIERARCHY:
--
--     SOMObject
--       ÀÄÄ WPObject
--             ÀÄÄ WPAbstract
--                   ÀÄÄ ShrPerson
--
-- DESCRIPTION:
--
--   See PERSON.C and PERSONV.C for more details.
--

#***********************************************************************
#   Specify the parent class
#***********************************************************************

parent: WPAbstract;

#***********************************************************************
#   Passthru IMPLEMENTATION definitions to the .ih file
#***********************************************************************

passthru: C.ih;

/*
 * Data keys for wpSaveString (see wpSaveState and wpRestoreState).
 */

#define IDK_ADDRESS                     1
#define IDK_CITY                        2
#define IDK_STATE                       3
#define IDK_ZIPCODE                     4
#define IDK_PHONE                       5

/*
 * Details structure (see wpclsQueryDetailsInfo).
 */

typedef struct
{
  PSZ pszPhone;
  PSZ pszAddress;
  PSZ pszCity;
  PSZ pszState;
  PSZ pszZipCode;
} PERSONDETAILS, *PPERSONDETAILS;

#define NUM_PERSONDETAILS_FIELDS        5

endpassthru;   /* .ih */

#***********************************************************************
#   Passthru PUBLIC definitions to the .h file
#***********************************************************************

passthru: C.h, after;

extern SOMClass *ShrFindPersonClass(VOID);

endpassthru;   /* C.h */

#***********************************************************************
#   Define instance data
#***********************************************************************

data:

   SOMObject *somNotifier;              // see shrQueryNotifier
                                        //   in PERSON.C

   PSZ pszAddress;
   PSZ pszCity;
   PSZ pszState;
   PSZ pszZipCode;
   PSZ pszPhone;

   HMODULE hmod, class;                 // see shrclsQueryModuleHandle
                                        //   in PERSON.C

   CLASSFIELDINFO acfiPerson[NUM_PERSONDETAILS_FIELDS], class;
                                        // see wpclsQueryDetailsInfo
                                        //   in PERSON.C

#***********************************************************************
#   Define methods
#***********************************************************************

methods:

#***********************************************************************
#   Define instance methods
#***********************************************************************

BOOL shrAddPersonInfoPage(HWND hwndNotebook);

#***********************************************************************
#   Define instance methods for setting instance data
#***********************************************************************

BOOL shrSetAddress(PSZ pszNewAddress);
BOOL shrSetCity(PSZ pszNewCity);
BOOL shrSetState(PSZ pszNewState);
BOOL shrSetZipCode(PSZ pszNewZipCode);
BOOL shrSetPhone(PSZ pszNewPhone);

#***********************************************************************
#   Define instance methods for querying instance data
#***********************************************************************

ULONG shrQueryAddress(ULONG cchAddressMax, PCHAR pchAddress);
ULONG shrQueryCity(ULONG cchCityMax, PCHAR pchCity);
ULONG shrQueryState(ULONG cchStateMax, PCHAR pchState);
ULONG shrQueryZipCode(ULONG cchZipCodeMax, PCHAR pchZipCode);
ULONG shrQueryPhone(ULONG cchPhoneMax, PCHAR pchPhone);

#***********************************************************************
# Generally, methods are invoked on the basis of information provided 
# the class that introduces the method definition (called "offset
# resolution").  However, this requires that the class of the method's
# target object be known at compile time.  Sometimes a method 
# definition is introduced by several classes.  For such methods,
# "name lookup" (which invokes the method using the method's name
# as a key) is a more appropriate choice.
#
# In this case shrQueryNotifier and shrclsQueryModuleHandle are
# defined by ShrAddressBook and ShrPerson.  See "Determining an 
# Object's Protocol" in README.TXT for more details. 
#***********************************************************************

SOMObject *shrQueryNotifier(), name lookup;

#***********************************************************************
# The shrUnderstandsProtocol method, while currently only defined by
# ShrPerson, would likely be defined in other classes, so it has
# been declared "name lookup" below.
#***********************************************************************

BOOL shrUnderstandsProtocol(ULONG idProtocol), name lookup;

#***********************************************************************
#   Specify instance methods being overridden
#***********************************************************************

override wpUnInitData;

override wpSetTitle;

override wpSaveState;
override wpRestoreState;

override wpAddSettingsPages;

override wpQueryDetailsData;

#***********************************************************************
#   Define class methods
#***********************************************************************

HMODULE shrclsQueryModuleHandle(), class, name lookup;

#***********************************************************************
#   Specify class methods being overridden
#***********************************************************************

override wpclsInitData, class;

override wpclsQueryDetailsInfo, class;

override wpclsQueryTitle, class;
override wpclsQueryIconData, class;
override wpclsQueryDefaultView, class;
