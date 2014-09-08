#***********************************************************************
#
#  OS/2 Work Place Shell Sample Program - ShrNotifier
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
#***********************************************************************

#***********************************************************************
#   Include the class definition file for the parent class
#***********************************************************************

include <somobj.sc>

#***********************************************************************
#   Define the new class
#***********************************************************************

class: ShrNotifier,
       external stem   = ntf,
       local,
       external prefix = ntf_,
       major version   = 1,
       minor version   = 2;

--
-- CLASS: ShrNotifier
--
-- CLASS HIERARCHY:
--
--     SOMObject
--       ÀÄÄ ShrNotifier
--
-- DESCRIPTION:
--
--   See NOTIFY.C for more details.
--   See ADDRBK.C and PERSON.C for example uses.
--

#***********************************************************************
#   Specify the parent class
#***********************************************************************

parent: SOMObject;

#***********************************************************************
#   Define instance data
#***********************************************************************

data:

   SOMObject *somWindowNotifyList;      // List of interested windows

#***********************************************************************
#   Define methods
#***********************************************************************

methods:

#***********************************************************************
#   Define instance methods
#***********************************************************************

SOMObject *shrQueryWindowNotifyList(), private;

BOOL shrAddInterestedWindow(HWND hwndInterested);
BOOL shrRemoveInterestedWindow(HWND hwndInterested);
ULONG shrNotifyInterestedWindows
    (SOMAny *somChangedObject, ULONG ulChangeMsg, MPARAM mp1, MPARAM mp2);

#***********************************************************************
#   Specify instance methods being overridden
#***********************************************************************

override somUninit;
