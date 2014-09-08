#***********************************************************************
#
#  OS/2 Work Place Shell Sample Program - ShrAddressBook
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

include <shrfoldr.sc>

#***********************************************************************
#   Define the new class
#***********************************************************************

class: ShrAddressBook,
       external stem   = adr,
       local,
       external prefix = adr_,
       classprefix     = adrM_,
       major version   = 1,
       minor version   = 2;

--
-- CLASS: ShrAddressBook
--
-- CLASS HIERARCHY:
--
--     SOMObject
--       ÀÄÄ WPObject
--             ÀÄÄ WPFileSystem
--                   ÀÄÄ ShrFolder
--                       ÀÄÄ ShrAddressBook
--
-- DESCRIPTION:
--
--   See ADDRBK.C and ADDRBKV.C for more details.
--

#***********************************************************************
#   Specify the parent class
#***********************************************************************

parent: ShrFolder;

#***********************************************************************
#   Passthru IMPLEMENTATION definitions to the .ih file
#***********************************************************************

passthru: C.ih;

endpassthru;   /* .ih */

#***********************************************************************
#   Passthru PUBLIC definitions to the .h file
#***********************************************************************

passthru: C.h, after;

extern SOMClass *ShrFindAddressBookClass(VOID);

endpassthru;   /* C.h */

#***********************************************************************
#   Define instance data
#***********************************************************************

data:

   SOMObject *somNotifier;              // see shrQueryNotifier
                                        //   in ADDRBK.C

   HMODULE hmod, class;                 // see shrclsQueryModuleHandle
                                        //   in ADDRBK.C

#***********************************************************************
#   Define methods
#***********************************************************************

methods:

#***********************************************************************
#   Define instance methods
#***********************************************************************

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
#   Specify instance methods being overridden from WPS
#***********************************************************************

override wpInitData;

override wpModifyPopupMenu;
override wpMenuItemSelected;
override wpMenuItemHelpSelected;

override wpOpen;

override wpDragOver;

#***********************************************************************
#   Specify instance methods being overridden from ShrFolder
#***********************************************************************

override shrAddedToContent;
override shrRemovedFromContent;

#***********************************************************************
#   Define class methods
#***********************************************************************

HMODULE shrclsQueryModuleHandle(), class, name lookup;
SOMObject *shrclsValidateObject(SOMAny *somObject), class;

#***********************************************************************
#   Specify class methods being overridden
#***********************************************************************

override wpclsQueryTitle, class;
override wpclsQueryIconData, class;
override wpclsQueryDefaultView, class;
