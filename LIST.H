/*
 * OS/2 Work Place Shell Sample Program - Simple ordered list functions
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
 * Simple list.  Note that PVOID is used for list to allow
 * easy conversion to a SOM object.
 */

#ifndef LIST_H
#define LIST_H

/*
 * Comparision functions for searching list.
 */

typedef LONG (FNLISTCOMPARE)(PVOID, PVOID);
typedef FNLISTCOMPARE *PFNLISTCOMPARE;

/*
 * Create or destroy a list.
 */

extern PVOID ShrGenericListNew(PFNLISTCOMPARE pfnCompare);
extern PVOID ShrExactStringListNew(void);
extern PVOID ShrStringListNew(void);
extern PVOID ShrValueListNew(void);

extern BOOL ShrListFree(PVOID pListParm);

/*
 * Add an item to a list.
 */

extern BOOL ShrListAddLast(PVOID pListParm, PVOID pValue);
extern BOOL ShrListAddFirst(PVOID pListParm, PVOID pValue);
extern BOOL ShrListAddBefore
    (PVOID pListParm, PVOID pValue, PVOID pBeforeValue);
extern BOOL ShrListAddAfter
    (PVOID pListParm, PVOID pValue, PVOID pAfterValue);

/*
 * Remove an item from list.
 */

extern BOOL ShrListRemove(PVOID pListParm, PVOID pValue);
extern PVOID ShrListRemoveFirst(PVOID pListParm);
extern BOOL ShrListRemoveAll(PVOID pListParm);

/* 
 * Enumerate items in a list.
 */

extern PVOID ShrListBeginEnum(PVOID pListParm);
extern BOOL ShrListEndEnum(PVOID pListParm, PVOID pListEnum);
extern BOOL ShrListNext
    (PVOID pListParm, PVOID pListEnum, PVOID *ppValue);

/* 
 * List statistics.
 */

extern ULONG ShrListCount(PVOID pListParm);

/*
 * Membership.
 */

extern BOOL ShrListIncludes(PVOID pListParm, PVOID pValue);

#endif
