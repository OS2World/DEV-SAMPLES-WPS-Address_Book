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
 * A simple unindex ordered collection implemented as a 
 * double linked list.  Performs well for small lists, and
 * very badly for large lists.
 */

#include <os2.h>          
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "list.h"

/*
 * Macros and constants.
 */

#define BOOLFROMP(p)         ((p) ? TRUE : FALSE)
#define LISTSIGNATURE        0x4E109D30
#define LISTENUMSIGNATURE    0x74C01283
#define ASSOCIATIONSIGNATURE 0xF7A52033

#define ShrValidateList(l,m) \
    (PLIST) ((l && ((PLIST) l)->ulSignature==LISTSIGNATURE) ? \
    l : DebugBox("Invalid List", m))
#define ShrValidateListEnum(l,e,m) (PLISTENUM) \
    ((l && e && ((PLISTENUM) e)->ulSignature==LISTENUMSIGNATURE && \
    ((PLISTENUM) e)->pList==(PLIST)l) ? \
    e : DebugBox("Invalid List Enum", m))

/*
 * Typedef's
 */

typedef struct _LISTNODE
{
  PVOID pValue;
  struct _LISTNODE *pNext;
  struct _LISTNODE *pPrev;
  BOOL bRemoved;
} LISTNODE, *PLISTNODE;

typedef struct _LIST
{
  ULONG ulSignature;
  ULONG ulCount;
  ULONG ulEnumCount;
  ULONG ulRemovedCount;
  PLISTNODE pHead;
  PLISTNODE pTail;
  PFNLISTCOMPARE pfnCompare;
} LIST, *PLIST;

typedef struct
{
  ULONG ulSignature;
  PLIST pList;
  PLISTNODE pNext;
} LISTENUM, *PLISTENUM;

/*
 * Local function declarations.
 */

static PVOID DebugBox(PSZ pszTitle, PSZ pszText);

static LONG ShrCompareExactString(PVOID psz1, PVOID psz2);
static LONG ShrCompareString(PVOID psz1, PVOID psz2);
static LONG ShrCompareValue(PVOID pv1, PVOID pv2);

static PLISTNODE ShrListInsertBefore
    (PLIST pList, PVOID pValue, PLISTNODE pBeforeListNode);
static PLISTNODE ShrListInsertAfter
    (PLIST pList, PVOID pValue, PLISTNODE pBeforeListNode);
static PLISTNODE ShrListFindNode(PLIST pList, PVOID pValue);
static VOID ShrListRemoveNode(PLIST pList, PLISTNODE pListNode);

/*
 * List creation for the standard list
 * types (string, string exact, long). 
 */

extern PVOID ShrExactStringListNew()
{
  return ShrGenericListNew(ShrCompareExactString);
}

extern PVOID ShrStringListNew()
{
  return ShrGenericListNew(ShrCompareString);
}

extern PVOID ShrValueListNew()
{
  return ShrGenericListNew(ShrCompareValue);
}

/*
 * List creation and destruction.
 */

extern PVOID ShrGenericListNew(PFNLISTCOMPARE pfnCompare)
{
  PLIST pList;

  pList = (PLIST) malloc(sizeof(LIST));

  if (pList)
  {
    memset(pList, 0, sizeof(LIST));
    pList->pfnCompare = pfnCompare;

    /*
     * The "signature" below is used to test whether a valid 
     * list pointer is being passed into the list functions.
     * See the ShrValidateList macro for details.
     */

    pList->ulSignature = LISTSIGNATURE;
  }

  return (PVOID) pList;
}

extern BOOL ShrListFree(PVOID pListParm)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListFree");
  PLISTNODE pListNode, pListNextNode;
  CHAR szText[128];
  BOOL bSuccess = FALSE;

  if (pList)
  {
    bSuccess = TRUE;

    pListNode = pList->pHead;
    while (pListNode)
    {
      pListNextNode = pListNode->pNext;
      free(pListNode);
      pListNode = pListNextNode;
    }

    if (pList->ulEnumCount)
    {
      sprintf(szText, 
          "ShrListFree warning: ShrListBeginEnum called %u times"
          " without calling ShrListEndEnum", pList->ulEnumCount);
      DebugBox("ShrListFree", szText);

      bSuccess = FALSE;
    }

    memset(pList, 0, sizeof(LIST));
    free(pList);
  }

  return bSuccess;
}

/*
 * List and list item statistics.
 */

extern ULONG ShrListCount(PVOID pListParm)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListCount");
  ULONG ulCount = 0;

  if (pList)
    ulCount = pList->ulCount - pList->ulRemovedCount;

  return ulCount;
}

extern BOOL ShrListIncludes
    (PVOID pListParm, PVOID pValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListIncludes");
  BOOL bFound = FALSE;

  if (pList)
    bFound = BOOLFROMP(ShrListFindNode(pList, pValue));
 
  return bFound;
}

/*
 * Adding list items.
 */

extern BOOL ShrListAddLast(PVOID pListParm, PVOID pValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListAddLast");
  PLISTNODE pListNode = NULL;

  if (pList)
  {
    if (pList->pTail)
    {
      pListNode = ShrListInsertAfter(pList, pValue, pList->pTail);
    }
    else
    {
      pListNode = (PLISTNODE) malloc(sizeof(LISTNODE));

      if (pListNode)
      {
        memset(pListNode, 0, sizeof(LISTNODE));
        pListNode->pValue = pValue;

        pList->pHead = pList->pTail = pListNode;
        pList->ulCount = 1;
      }
    }
  }

  return BOOLFROMP(pListNode);
}

extern BOOL ShrListAddFirst(PVOID pListParm, PVOID pValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListAddFirst");
  PLISTNODE pListNode = NULL;

  if (pList)
  {
    if (pList->pHead)
    {
      pListNode = ShrListInsertBefore(pList, pValue, pList->pHead);
    }
    else
    {
      pListNode = (PLISTNODE) malloc(sizeof(LISTNODE));

      if (pListNode)
      {
        memset(pListNode, 0, sizeof(LISTNODE));
        pListNode->pValue = pValue;

        pList->pHead = pList->pTail = pListNode;
        pList->ulCount = 1;
      }
    }
  }

  return BOOLFROMP(pListNode);
}

extern BOOL ShrListAddAfter
    (PVOID pListParm, PVOID pValue, PVOID pAfterValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListAddAfter");
  PLISTNODE pAfterListNode, pListNode = NULL;

  if (pList)
  {
    pAfterListNode = ShrListFindNode(pList, pAfterValue);

    if (pAfterListNode)
      pListNode = ShrListInsertAfter(pList, pValue, pAfterListNode);
  }

  return BOOLFROMP(pListNode);
}

extern BOOL ShrListAddBefore
    (PVOID pListParm, PVOID pValue, PVOID pBeforeValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListAddBefore");
  PLISTNODE pBeforeListNode, pListNode = NULL;

  if (pList)
  {
    pBeforeListNode = ShrListFindNode(pList, pBeforeValue);

    if (pBeforeListNode)
      pListNode = ShrListInsertBefore(pList, pValue, pBeforeListNode);
  }

  return BOOLFROMP(pListNode);
}

/*
 * List item removal.
 */

extern BOOL ShrListRemove(PVOID pListParm, PVOID pValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListRemove");
  PLISTNODE pListNode = NULL;

  if (pList)
  {
    pListNode = ShrListFindNode(pList, pValue);

    if (pListNode)
      ShrListRemoveNode(pList, pListNode);
  }

  return BOOLFROMP(pListNode);
}

extern PVOID ShrListRemoveFirst(PVOID pListParm)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListRemoveFirst");
  PLISTNODE pListNode = NULL;
  PVOID pValue = NULL;

  if (pList)
  {
    pListNode = pList->pHead;

    if (pListNode)
    {
      pValue = pListNode->pValue;
      ShrListRemoveNode(pList, pListNode);
    }
  }

  return pValue;
}

extern BOOL ShrListRemoveAll(PVOID pListParm)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListRemoveAll");
  PLISTNODE pListNextNode, pListNode;
  BOOL bSuccess = FALSE;

  if (pList)
  {
    pListNode = pList->pHead;
    bSuccess = TRUE;

    while (pListNode)
    {
      pListNextNode = pListNode->pNext;
      ShrListRemoveNode(pList, pListNode);
      pListNode = pListNextNode;
    }
  }

  return bSuccess;
}

/*
 * List traversal.
 */

extern PVOID ShrListBeginEnum(PVOID pListParm)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListBeginEnum");
  PLISTENUM pListEnum = NULL;

  if (pList)
  {
    pListEnum = (PLISTENUM) malloc(sizeof(LISTENUM));

    if (pListEnum)
    {
      pListEnum->pList = pList;
      pListEnum->pNext = pList->pHead;

      pList->ulEnumCount++;

      /*
       * The "signature" below is used to test whether a valid
       * enum pointer is being passed into the list functions.
       * See the ShrValidateListEnum macro for details.
       */

      pListEnum->ulSignature = LISTENUMSIGNATURE;
    }
  }

  return (PVOID) pListEnum;
}

extern BOOL ShrListEndEnum(PVOID pListParm, PVOID pListEnumParm)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListEndEnum");
  PLISTENUM pListEnum = 
      ShrValidateListEnum(pListParm, pListEnumParm, "ShrListEndEnum");
  PLISTNODE pListNode, pListNextNode;

  if (pList && pListEnum)
  {
    memset(pListEnum, 0, sizeof(LISTENUM));
    free(pListEnum);

    pList->ulEnumCount--;

    if (pList->ulEnumCount == 0 && pList->ulRemovedCount)
    {
      pListNode = pList->pHead;

      while (pListNode && pList->ulRemovedCount)
      {
        pListNextNode = pListNode->pNext;

        if (pListNode->bRemoved)
          ShrListRemoveNode(pList, pListNode);

        pListNode = pListNextNode;
      }
    }
  }

  return (pList && pListEnum);
}

extern BOOL ShrListNext
    (PVOID pListParm, PVOID pListEnumParm, PVOID *ppValue)
{
  PLIST pList = ShrValidateList(pListParm, "ShrListNext");
  PLISTENUM pListEnum = 
      ShrValidateListEnum(pListParm, pListEnumParm, "ShrListNext");

  if (pList && pListEnum)
  {
    while (pListEnum->pNext)
    {
      if (!pListEnum->pNext->bRemoved)
      {
        *ppValue = pListEnum->pNext->pValue;
        pListEnum->pNext = pListEnum->pNext->pNext;

        return TRUE;
      }

      pListEnum->pNext = pListEnum->pNext->pNext;
    }
  }

  return FALSE;
}

/*
 * Internal functions (note they assume the
 * parameters have been validated).
 */

static PLISTNODE ShrListFindNode(PLIST pList, PVOID pValue)
{
  PLISTNODE pListNode;

  pListNode = pList->pHead;

  while (pListNode)
    if (!pListNode->bRemoved &&
        (*pList->pfnCompare)(pListNode->pValue, pValue) == 0)
      return pListNode;
    else
      pListNode = pListNode->pNext;

  return NULL;
}

static PLISTNODE ShrListInsertAfter
    (PLIST pList, PVOID pValue, PLISTNODE pAfterListNode)
{
  PLISTNODE pListNode = NULL;

  pListNode = (PLISTNODE) malloc(sizeof(LISTNODE));

  if (pListNode)
  {
    memset(pListNode, 0, sizeof(LISTNODE));
    pListNode->pValue = pValue;

    pListNode->pPrev = pAfterListNode;
    pListNode->pNext = pAfterListNode->pNext;

    if (pAfterListNode->pNext)
      pAfterListNode->pNext->pPrev = pListNode;

    pAfterListNode->pNext = pListNode;

    if (pList->pTail == pAfterListNode)
      pList->pTail = pListNode;

    pList->ulCount++;
  }

  return pListNode;
}

static PLISTNODE ShrListInsertBefore
    (PLIST pList, PVOID pValue, PLISTNODE pBeforeListNode)
{
  PLISTNODE pListNode = NULL;

  pListNode = (PLISTNODE) malloc(sizeof(LISTNODE));

  if (pListNode)
  {
    memset(pListNode, 0, sizeof(LISTNODE));
    pListNode->pValue = pValue;

    pListNode->pNext = pBeforeListNode;
    pListNode->pPrev = pBeforeListNode->pPrev;

    if (pBeforeListNode->pPrev)
      pBeforeListNode->pPrev->pNext = pListNode;

    pBeforeListNode->pPrev = pListNode;

    if (pList->pHead == pBeforeListNode)
      pList->pHead = pListNode;

    pList->ulCount++;
  }

  return pListNode;
}

static VOID ShrListRemoveNode(PLIST pList, PLISTNODE pListRemoveNode)
{
  if (pList->ulEnumCount == 0)
  {
    pList->ulCount--;

    if (pList->pHead == pListRemoveNode)
      pList->pHead = pListRemoveNode->pNext;
    if (pList->pTail == pListRemoveNode)
      pList->pTail = pListRemoveNode->pPrev;
    
    if (pListRemoveNode->pPrev)
      pListRemoveNode->pPrev->pNext = pListRemoveNode->pNext;
    
    if (pListRemoveNode->pNext)
      pListRemoveNode->pNext->pPrev = pListRemoveNode->pPrev;

    if (pListRemoveNode->bRemoved)
      pList->ulRemovedCount--;

    free(pListRemoveNode);
  }
  else
  {
    pListRemoveNode->bRemoved = TRUE;
    pList->ulRemovedCount++;
  }
  
  return;
}

static PVOID DebugBox(PSZ pszTitle, PSZ pszText)
{
  WinMessageBox( HWND_DESKTOP, HWND_DESKTOP, pszText, pszTitle,
     20, MB_OK | MB_INFORMATION | MB_MOVEABLE);

  return NULL;
}

/*
 * List item comparision functions for the standard list
 * types (string, string exact, long).  Use ShrGenericListNew
 * to provide your own list item type.
 */

static LONG ShrCompareExactString(PVOID psz1, PVOID psz2)
{
  return strcmp((PSZ) psz1, (PSZ) psz2);
}

static LONG ShrCompareString(PVOID psz1, PVOID psz2)
{
  return stricmp((PSZ) psz1, (PSZ) psz2);
}

static LONG ShrCompareValue(PVOID pv1, PVOID pv2)
{
  return (LONG) pv1 - (LONG) pv2;
}
