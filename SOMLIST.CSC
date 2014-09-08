
#***********************************************************************
#
#  OS/2 Work Place Shell Sample Program - ShrList
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

include <somobj.sc>

#***********************************************************************
#   Define the new class
#***********************************************************************

class: ShrList,
       external stem   = ls,
       local,
       external prefix = ls_,
       classprefix     = lsM_,
       major version   = 1,
       minor version   = 2;

--
-- CLASS: ShrList
--
-- CLASS HIERARCHY:
--
--     SOMObject
--       ÀÄÄ ShrList
--
-- DESCRIPTION:
--
--   See SOMLIST.C and LIST.C for more details.
--

#***********************************************************************
#   Specify the parent class
#***********************************************************************

parent: SOMObject;

#***********************************************************************
#   Passthru PUBLIC definitions to the .h file
#***********************************************************************

passthru: C.h, before;

#include "list.h"

endpassthru;   /* C.h */

#***********************************************************************
#   Define instance data
#***********************************************************************

data:

   PVOID   pList;

#***********************************************************************
#   Define methods
#***********************************************************************

methods:

#***********************************************************************
#   Define instance methods
#***********************************************************************

PVOID shrQueryList();
BOOL shrSetList(PVOID pNewList);

/*
 * Add an item to a list.
 */

BOOL shrAddLast(PVOID pValue);
BOOL shrAddFirst(PVOID pValue);
BOOL shrAddBefore(PVOID pValue, PVOID pBeforeValue);
BOOL shrAddAfter(PVOID pValue, PVOID pAfterValue);

/*
 * Remove an item from list.
 */

BOOL shrRemove(PVOID pValue);
BOOL shrRemoveAll();

/* 
 * Enumerate items in a list.
 */

PVOID shrBeginEnum();
BOOL shrEndEnum(PVOID pListEnum);
BOOL shrNext(PVOID pListEnum, PVOID *ppValue);

/* 
 * List statistics.
 */

ULONG shrCount();

/*
 * Membership.
 */

BOOL shrIncludes(PVOID pValue);

#***********************************************************************
#   Specify instance methods being overridden
#***********************************************************************

override somUninit;

#***********************************************************************
#   Define class methods
#***********************************************************************

SOMObject *shrclsExactStringListNew(), class;
SOMObject *shrclsStringListNew(), class;
SOMObject *shrclsValueListNew(), class;
SOMObject *shrclsNew(PFNLISTCOMPARE pfnCompare), class;
