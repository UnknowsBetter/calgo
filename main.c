#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algo.h"

typedef struct tagInteger
{
	UINT uiValue;
	HLIST_NODE_T stListNode;
} INTEGER_T;


ULONG hash_HashIntegerT(VOID *pKey)
{
	return rhash_HashInt32(pKey, 0);
}

ULONG hash_CompareIntegerT (HLIST_NODE_T *a, VOID *pKey)
{
	INTEGER_T *pstInteger = NULL;
	pstInteger = ContainerOf(a, INTEGER_T, stListNode);
	return pstInteger->uiValue - *(UINT*)pKey;
}

int main(int argc, char* argv[])
{

	HASH_TABLE_T stHashTable;
	INTEGER_T    astIntegers[200];
	UINT uiKey = 0;

	hash_InitTable(&stHashTable, hash_HashIntegerT, hash_CompareIntegerT, 20);

	for (size_t i = 0; i < 200; i++)
	{
		astIntegers[i].uiValue = i;
		hash_AddNode(&stHashTable, &astIntegers[i].uiValue, &astIntegers[i].stListNode);	
	}
	
	HLIST_NODE_T *pstNode = hash_FindNode(&stHashTable, &uiKey);
	INTEGER_T *pstInteger = ContainerOf(pstNode, INTEGER_T, stListNode);

	assert_runtime(pstInteger == astIntegers + uiKey);

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
	assert_runtime(stList.pstFirst == &stNode3);
	assert_runtime(stNode3.pstNext == &stNode2);
	assert_runtime(stNode2.pstNext == &stNode1);
	assert_runtime(stNode1.pstNext == NULL);

	assert_runtime(stNode1.ppstPrevious == (HLIST_NODE_T**)&stNode2);
	assert_runtime(stNode2.ppstPrevious == (HLIST_NODE_T**)&stNode3);
	assert_runtime(stNode3.ppstPrevious == (HLIST_NODE_T**)&(stList.pstFirst));

	hlist_RemoveNode(&stNode1);
	assert_runtime(stNode2.pstNext == NULL);

	hlist_RemoveNode(&stNode2);
	hlist_RemoveNode(&stNode3);

	assert_runtime(stList.pstFirst == NULL);

	rlist_foreach((&stList))
 	{
 		printf("iter: %p, next: %p, pprev: %p, *pprev: %p\n",
			   iter,
			   iter->pstNext,
			   iter->ppstPrevious,
			   *(iter->ppstPrevious));
	}


}