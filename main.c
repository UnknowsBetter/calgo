#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

typedef struct tagInteger
{
	UINT uiValue;

	/* never use a single node to add this structure node 
	   to multiple containers. */
	HLIST_NODE_T stListNode;
	HLIST_NODE_T stStackNode;

} UINTEGER_T;

#define SIZEOF_UINTEGER_T sizeof(UINTEGER_T)
#define UINTEGER_T_CONTAINEROF(pstListNode) ContainerOf(pstListNode, UINTEGER_T, stListNode)

STATIC UINTEGER_T * integer_Create(UINT uiValue)
{
	UINTEGER_T * pstInteger = NULL;

	pstInteger = malloc(SIZEOF_UINTEGER_T);
	if (pstInteger)
	{
		memset(pstInteger, 0, sizeof(UINTEGER_T));
		pstInteger->uiValue = uiValue;
	}
	return pstInteger;
}

STATIC VOID integer_Destroy(UINTEGER_T * pstInteger)
{
	if (pstInteger)
	{
		free(pstInteger);
	}
}

STATIC ULONG hash_HashIntegerT(VOID *pKey)
{
	return rhash_HashInt32(pKey, 0);
}

STATIC ULONG hash_CompareIntegerT (HLIST_NODE_T *a, VOID *pKey)
{
	UINTEGER_T *pstInteger = NULL;
	pstInteger = ContainerOf(a, UINTEGER_T, stListNode);
	return pstInteger->uiValue - *(UINT*)pKey;
}

int main(int argc, char* argv[])
{

	HASH_TABLE_T stHashTable;
	HSTACK_T	 stStack;
	UINTEGER_T    astIntegers[200];
	UINT uiKey = 0;
	UINT uiCounter = 1;

	memset(&stStack, 0, sizeof(HSTACK_T));

	hash_InitTable(&stHashTable, hash_HashIntegerT, hash_CompareIntegerT, 20);

	for (size_t i = 0; i < 200; i++)
	{
		astIntegers[i].uiValue = i;
		hash_AddNode(&stHashTable, &astIntegers[i].uiValue, &astIntegers[i].stListNode);	
		hstack_Push (&stStack, &astIntegers[i].stStackNode);
	}
	
	hash_Walk((&stHashTable))
	{
		UINTEGER_T* pstIteger = UINTEGER_T_CONTAINEROF(iter);

		printf("%03llu, %p: %-3u%s", ulBktIdx, iter, pstIteger->uiValue,
			   (uiCounter++%4 == 0) ? "\n" : ",    \t");
	}

	printf("Stack size: %u\n ", stStack.uiSize);
	hstack_Walk((&stStack))
	{
		UINTEGER_T* pstIteger = ContainerOf(iter, UINTEGER_T, stStackNode);
		printf("%u,", pstIteger->uiValue);
	}


	HLIST_NODE_T *pstStackNode = NULL;

	pstStackNode = hstack_Pop(&stStack);
	while (pstStackNode != NULL)
	{
		UINTEGER_T* pstIteger = ContainerOf(pstStackNode, UINTEGER_T, stStackNode);
		printf("%u, \n", pstIteger->uiValue);

		pstStackNode = hstack_Pop(&stStack);
	}

	hash_FiniTable(&stHashTable);

	return 0;
}



















void test_rlist()
{
	HLIST_HEAD_T  	stList;
	HLIST_NODE_T  	stNode1;
	HLIST_NODE_T  	stNode2;
	HLIST_NODE_T  	stNode3;

	memset(&stList, 0, sizeof(stList));
	memset(&stNode1, 0, sizeof(stNode1));
	memset(&stNode2, 0, sizeof(stNode2));
	memset(&stNode3, 0, sizeof(stNode3));
	
	hlist_AddNode(&stList, &stNode1);
	hlist_AddNode(&stList, &stNode2);
	hlist_AddNode(&stList, &stNode3);
 
	/* check the insertion results */
	AssertRuntime(stList.pstFirst == &stNode3);
	AssertRuntime(stNode3.pstNext == &stNode2);
	AssertRuntime(stNode2.pstNext == &stNode1);
	AssertRuntime(stNode1.pstNext == NULL);

	AssertRuntime(stNode1.ppstPrevious == (HLIST_NODE_T**)&stNode2);
	AssertRuntime(stNode2.ppstPrevious == (HLIST_NODE_T**)&stNode3);
	AssertRuntime(stNode3.ppstPrevious == (HLIST_NODE_T**)&(stList.pstFirst));

	hlist_RemoveNode(&stNode1);
	AssertRuntime(stNode2.pstNext == NULL);

	hlist_RemoveNode(&stNode2);
	hlist_RemoveNode(&stNode3);

	AssertRuntime(stList.pstFirst == NULL);

	hlist_Walk((&stList))
 	{
 		printf("iter: %p, next: %p, pprev: %p, *pprev: %p\n",
			   iter,
			   iter->pstNext,
			   iter->ppstPrevious,
			   *(iter->ppstPrevious));
	}


}