/*
 * OS/2 Work Place Shell Sample Program - SOM simple ordered list
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
 * Simple ordered list based on the list functions provided by LIST.C.
 */

#define SOM_NoTest
#define ShrList_Class_Source

#include <os2.h>

#pragma checkout(suspend)
#include "somlist.ih"
#pragma checkout(resume)

/*
 * Query and Set instance variable(s).
 */

SOM_Scope PVOID SOMLINK ls_shrQueryList(ShrList *somSelf)
{
  ShrListData *somThis = ShrListGetData(somSelf);

  return _pList;
}

SOM_Scope BOOL SOMLINK ls_shrSetList(ShrList *somSelf,
                PVOID pNewList)
{
  ShrListData *somThis = ShrListGetData(somSelf);

  _pList = pNewList;

  return TRUE;
}

/*
 *
 * Add an item to a list.
 */

SOM_Scope BOOL SOMLINK ls_shrAddLast(ShrList *somSelf,
                PVOID pValue)
{
  return ShrListAddLast(_shrQueryList(somSelf), pValue);
}

SOM_Scope BOOL SOMLINK ls_shrAddFirst(ShrList *somSelf,
                PVOID pValue)
{
  return ShrListAddFirst(_shrQueryList(somSelf), pValue);
}

SOM_Scope BOOL SOMLINK ls_shrAddBefore(ShrList *somSelf,
                PVOID pValue,
                PVOID pBeforeValue)
{
  return ShrListAddBefore(_shrQueryList(somSelf), pValue, pBeforeValue);
}

SOM_Scope BOOL SOMLINK ls_shrAddAfter(ShrList *somSelf,
                PVOID pValue,
                PVOID pAfterValue)
{
  return ShrListAddAfter(_shrQueryList(somSelf), pValue, pAfterValue);
}

/*
 *
 * Remove an item from list.
 */

SOM_Scope BOOL SOMLINK ls_shrRemove(ShrList *somSelf,
                PVOID pValue)
{
  return ShrListRemove(_shrQueryList(somSelf), pValue);
}

SOM_Scope BOOL SOMLINK ls_shrRemoveAll(ShrList *somSelf)
{
  return ShrListRemoveAll(_shrQueryList(somSelf));
}

/*
 *
 * Enumerate items in a list.
 */

SOM_Scope PVOID SOMLINK ls_shrBeginEnum(ShrList *somSelf)
{
  return ShrListBeginEnum(_shrQueryList(somSelf));
}

SOM_Scope BOOL SOMLINK ls_shrEndEnum(ShrList *somSelf,
                PVOID pListEnum)
{
  return ShrListEndEnum(_shrQueryList(somSelf), pListEnum);
}

SOM_Scope BOOL SOMLINK ls_shrNext(ShrList *somSelf,
                PVOID pListEnum,
                PVOID *ppValue)
{
  return ShrListNext(_shrQueryList(somSelf), pListEnum, ppValue);
}

/*
 *
 * List statistics.
 */

SOM_Scope ULONG SOMLINK ls_shrCount(ShrList *somSelf)
{
  return ShrListCount(_shrQueryList(somSelf));
}

/*
 *
 * Membership.
 */

SOM_Scope BOOL SOMLINK ls_shrIncludes(ShrList *somSelf,
                PVOID pValue)
{
  return ShrListIncludes(_shrQueryList(somSelf), pValue);
}

/*
 *
 * When destroying a list.
 */

SOM_Scope VOID SOMLINK ls_somUninit(ShrList *somSelf)
{
  PVOID pList = _shrQueryList(somSelf);

  if (pList)
    ShrListFree(pList);

  parent_somUninit(somSelf);
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

SOM_Scope SOMObject * SOMLINK lsM_shrclsExactStringListNew(M_ShrList *somSelf)
{
  SOMObject *newList;
  PVOID pList = NULL;

  newList = _somNew(somSelf);

  if (newList)
  {
    pList = ShrExactStringListNew();

    if (pList)
    {
      _shrSetList(newList, pList);
    }
    else
    {
      _somFree(newList);
      newList = NULL;
    }
  }

  return newList;
}

SOM_Scope SOMObject * SOMLINK lsM_shrclsStringListNew(M_ShrList *somSelf)
{
  SOMObject *newList;
  PVOID pList = NULL;

  newList = _somNew(somSelf);

  if (newList)
  {
    pList = ShrStringListNew();

    if (pList)
    {
      _shrSetList(newList, pList);
    }
    else
    {
      _somFree(newList);
      newList = NULL;
    }
  }

  return newList;
}

SOM_Scope SOMObject * SOMLINK lsM_shrclsValueListNew(M_ShrList *somSelf)
{
  SOMObject *newList;
  PVOID pList = NULL;

  newList = _somNew(somSelf);

  if (newList)
  {
    pList = ShrValueListNew();

    if (pList)
    {
      _shrSetList(newList, pList);
    }
    else
    {
      _somFree(newList);
      newList = NULL;
    }
  }

  return newList;
}

SOM_Scope SOMObject * SOMLINK lsM_shrclsNew(M_ShrList *somSelf,
                PFNLISTCOMPARE pfnCompare)
{
  SOMObject *newList;
  PVOID pList = NULL;

  newList = _somNew(somSelf);

  if (newList)
  {
    pList = ShrGenericListNew(pfnCompare);

    if (pList)
    {
      _shrSetList(newList, pList);
    }
    else
    {
      _somFree(newList);
      newList = NULL;
    }
  }

  return newList;
}
