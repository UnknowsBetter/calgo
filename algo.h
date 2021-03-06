// Header file guard
////////////////////////////////////////////////////

#ifndef ralgorithm
#define ralgorithm

// Runtime assertions
////////////////////////////////////////////////////

#ifndef _STDIO_H
#error include stdio.h file before include this one
#endif
#define dbg_output_file (1)
#define println(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)

#ifdef DEBUG
#define AssertRuntime(expr) 		\
	do {			\
		if (!(expr)) {	\
			char buffer[1024] = {0};								\
			int len = sprintf(buffer, "(%s) Assertion Failed! at: %s:%d\n", #expr, __FILE__, __LINE__); \
			printf("%s", buffer); \
		}		\
	} while (0)
#else
#define AssertRuntime(expr) 
#endif

// General macros
////////////////////////////////////////////////////

#define ContainerOf(member, container, memberField) \
	(container*) ((UCHAR*)member - (UCHAR*)(&((container*)(0))->memberField) )

#define TypeCheck(type_name, type_value) \
	({\
		type_name a;\
		typeof (type_value) b;\
		(void)(&a == &b);\
		1;\
	})

// Basic type definition
////////////////////////////////////////////////////

#define  UINT 	unsigned int
#define  INT 	int
#define  ULONG  unsigned long long
#define  LONG   long long
#define  BOOL   unsigned char 
#define  TRUE  	((BOOL)(1))
#define  FALSE 	((BOOL)(0))

#define  UCHAR 	unsigned char
#define  CHAR  	char
#define  BYTE 	unsigned char

#define  ERROR_FAILED       0xffffffff
#define  ERROR_SUCCESS      0x0

#define STATIC static
#define INLINE inline
#define VOID void

#define IN 
#define OUT
#define INOUT

// Containers
/////////////////////////////////////////////////////

typedef struct tagRListNode HLIST_NODE_T;

/* list's node */
typedef struct tagRListNode
{
	HLIST_NODE_T * pstNext;
	HLIST_NODE_T ** ppstPrevious;
} HLIST_NODE_T;

/* list's head, do not store any entity message. */
typedef struct tagRList
{
	HLIST_NODE_T * pstFirst;
} HLIST_HEAD_T;

typedef UINT (*LIST_COMPARE_PF)(IN HLIST_NODE_T* a, IN HLIST_NODE_T *b);

STATIC VOID hlist_Init(HLIST_HEAD_T *pstList)
{
	pstList->pstFirst = NULL;
}

STATIC VOID hlist_AddNode(HLIST_HEAD_T *pstList, HLIST_NODE_T *pstNode)
{
	HLIST_NODE_T * pstFirst = NULL;

	pstFirst = pstList->pstFirst;
	if (pstFirst != NULL)
	{
		pstFirst->ppstPrevious = &pstNode->pstNext;
	}
	pstNode->ppstPrevious = &(pstList->pstFirst);
	pstNode->pstNext = pstFirst;
	pstList->pstFirst = pstNode;
}

STATIC VOID hlist_RemoveNode(HLIST_NODE_T *pstNode)
{
	HLIST_NODE_T* pstPrevious = NULL;
	HLIST_NODE_T* pstNext = NULL;
	
	pstPrevious = (HLIST_NODE_T*) pstNode->ppstPrevious;
	pstNext = (HLIST_NODE_T*) pstNode->pstNext;

	pstPrevious->pstNext = pstNode->pstNext;
	if (pstNext != NULL)
	{
		pstNext->ppstPrevious = (HLIST_NODE_T**)pstPrevious;
	}
}

STATIC VOID hlist_RemoveAllNode(IN HLIST_HEAD_T *pstHead)
{
	while (pstHead->pstFirst != NULL)
	{
		hlist_RemoveNode(pstHead->pstFirst);
	}
}


#define hlist_Walk(pstList) \
	TypeCheck(HLIST_HEAD_T *, pstList); \
	for (\
		HLIST_NODE_T * iter = pstList->pstFirst;\
		iter != NULL;\
		iter = iter->pstNext\
	) 

// hash function 
////////////////////////////////////////////////////


STATIC ULONG rhash_HashBytes(VOID *pBytes, UINT uiLength)
{
	UCHAR *pucBytes = NULL;
	ULONG ulConstant = 5381;
	ULONG ulResult = ulConstant;
	
	pucBytes = pBytes;

	while (uiLength > 0)
	{
		ulResult = (ulResult << 5) + ulResult + pucBytes[uiLength];
		uiLength--;
	}
	return ulResult;
}

STATIC ULONG rhash_HashInt32(VOID *pBytes, UINT uiLength)
{
	UINT uiKey = *(UINT*)pBytes;
	ULONG ulResult = 0;

	uiKey += ~(uiKey << 15);
	uiKey ^= (uiKey >> 10);

	uiKey += (uiKey << 3);
	uiKey ^= (uiKey >> 6);

	uiKey += ~(uiKey << 11);
	uiKey ^= (uiKey >> 16);

	return ulResult = uiKey;
}

// hash table
////////////////////////////////////////////////////

#ifndef _STDLIB_H
#error include stdlib.h before include this one
#endif

typedef ULONG (*HASH_FUNC_PF)(VOID *pKey);
typedef ULONG (*HASH_COMPARE_PF)(HLIST_NODE_T *a, VOID *pKey);

typedef struct taghHashTable
{
	HLIST_HEAD_T    *pstBuckets;
	ULONG		  	ulBucketSize;
	HASH_FUNC_PF    pfHash;
	HASH_COMPARE_PF pfCompare;
}HASH_TABLE_T;

#define hash_GetBucketIndex(pstTable, pKey, _ulIdx) \
({\
	_ulIdx = pstTable->pfHash(pKey); \
	_ulIdx = _ulIdx % (pstTable->ulBucketSize);\
})

STATIC ULONG hash_InitTable(HASH_TABLE_T *pstTable, 
					 HASH_FUNC_PF    pfHash,
					 HASH_COMPARE_PF pfCompare,
					 UINT uiBktSize)
{
	pstTable->pfCompare 	= pfCompare;
	pstTable->pfHash 		= pfHash;
	pstTable->ulBucketSize  = uiBktSize;
	pstTable->pstBuckets 	= malloc(sizeof(HLIST_HEAD_T) * uiBktSize);

	for (size_t i = 0; i < uiBktSize; i++)
	{
		hlist_Init(&pstTable->pstBuckets[i]);
	}

	return pstTable->pstBuckets != NULL ? 
		   ERROR_SUCCESS : ERROR_FAILED;
}

/**
 * @brief 
 * dtor of the hash table.
 * @param pstTable 
 * @return STATIC 
 */
STATIC ULONG hash_FiniTable(HASH_TABLE_T *pstTable)
{
	for (size_t i = 0; i < pstTable->ulBucketSize; i++)
	{
		hlist_RemoveAllNode(&pstTable->pstBuckets[i]);
	}
	
	free(pstTable->pstBuckets);
	memset(pstTable, 0, sizeof(HASH_TABLE_T));
}

/**
 * @brief 
 * add a new node to the hash table
 * @param pstTable 
 * @param pKey 
 * @param pstNode 
 * @return STATIC 
 */
STATIC ULONG hash_AddNode(HASH_TABLE_T *pstTable, VOID* pKey, HLIST_NODE_T *pstNode)
{
	ULONG ulIndex = 0;

	if (pstTable == NULL || pstNode == NULL)
		return ERROR_FAILED;

	hash_GetBucketIndex(pstTable, pKey, ulIndex);
	hlist_AddNode(&pstTable->pstBuckets[ulIndex], pstNode);

	return ERROR_SUCCESS;
}

/**
 * @brief 
 * find the hash node specified by the key from the table.
 * @param pstTable 
 * @param pKey 
 * @return STATIC* 
 */
STATIC HLIST_NODE_T* hash_FindNode(HASH_TABLE_T *pstTable, void *pKey)
{
	ULONG 			ulIndex = 0;
	HLIST_HEAD_T 	*pstHead = NULL;
	HLIST_NODE_T 	*pstNode = NULL;
	HASH_COMPARE_PF pfCompare = NULL;

	pfCompare = pstTable->pfCompare;

	if (pstTable == NULL || pKey == NULL)
		return pstNode;

	hash_GetBucketIndex(pstTable, pKey, ulIndex);

	pstHead = pstTable->pstBuckets + ulIndex;
	hlist_Walk(pstHead)
	{
		if (0 == pfCompare(iter, pKey))
		{
			pstNode = (HLIST_NODE_T*) iter;
		}
	}

	return pstNode;
}

/**
 * @brief 
 * remove node from the hash table, the node is specified 
 * by the `pKey` param.
 * @param pstTable from
 * @param pKey 
 * @return HLIST_NODE_T* which is the node removed.
 */
STATIC HLIST_NODE_T* hash_RemoveNode(HASH_TABLE_T *pstTable, void *pKey)
{
	HLIST_NODE_T *pstNode = NULL;

	pstNode = hash_FindNode(pstTable, pKey);
	hlist_RemoveNode(pstNode);

	return pstNode;
}

#define hash_Walk(pstTable) \
	for (ULONG ulBktIdx = 0; ulBktIdx < pstTable->ulBucketSize; ulBktIdx++) \
	for (HLIST_NODE_T *iter = pstTable->pstBuckets[ulBktIdx].pstFirst; \
		iter != NULL; \
		iter = iter->pstNext )

// Stack using list
////////////////////////////////////////////////////

typedef struct tagHStack
{
	HLIST_HEAD_T stFrame;
	UINT	     uiSize;
}HSTACK_T;

INLINE STATIC BOOL hstack_IsEmpty(HSTACK_T *pstStack)
{
	AssertRuntime(pstStack != NULL);
	return (pstStack == NULL) || 
		   (pstStack->stFrame.pstFirst == NULL);
}

STATIC HLIST_NODE_T * hstack_Pop(HSTACK_T *pstStack)
{
	HLIST_NODE_T *pstNode = NULL;

	AssertRuntime(pstStack != NULL);

	if (TRUE != hstack_IsEmpty(pstStack)) {
		pstNode = pstStack->stFrame.pstFirst;
		hlist_RemoveNode(pstNode);
	}

	return pstNode;
}

STATIC HLIST_NODE_T * hstack_Peek(HSTACK_T *pstStack)
{
	AssertRuntime(pstStack != NULL);

	return pstStack->stFrame.pstFirst;
}

STATIC VOID hstack_Push(HSTACK_T *pstStack, HLIST_NODE_T* pstNode)
{
	AssertRuntime(pstStack != NULL);
	AssertRuntime(pstNode != NULL);
	pstStack->uiSize++;
	hlist_AddNode(&pstStack->stFrame, pstNode);
}


STATIC UINT hstack_size(HSTACK_T *pstStack)
{
	AssertRuntime(pstStack != NULL);

	return pstStack->uiSize;
}

#define hstack_Walk(pstStack) hlist_Walk((&pstStack->stFrame))

// Queue using list
////////////////////////////////////////////////////

typedef struct tagHQueue
{
	HLIST_HEAD_T stFrame;
	UINT	     uiSize;
	HLIST_NODE_T *pstQueueHead;
}HQUEUE_T;

STATIC VOID hqueue_init(HQUEUE_T *pstQueue)
{
	memset(pstQueue, 0, sizeof(HQUEUE_T));
}

STATIC BOOL hqueue_IsEmpty(HQUEUE_T *pstQueue)
{
	return pstQueue->pstQueueHead == NULL;
}

#ifndef _STRING_H
#error include string.h before this one
#endif
STATIC VOID hqueue_In(HQUEUE_T *pstQueue, HLIST_NODE_T *pstQNode)
{
	AssertRuntime(pstQNode != NULL);
	AssertRuntime(pstQueue != NULL);
	hlist_AddNode(&pstQueue->stFrame, pstQNode);

	if (pstQueue->pstQueueHead == NULL)
	{
		pstQueue->pstQueueHead = pstQNode;
	}
}

STATIC HLIST_NODE_T * hqueue_Out(HQUEUE_T *pstQueue)
{
	HLIST_NODE_T *pstHead = NULL;

	AssertRuntime(pstQueue != NULL);

	pstHead = pstQueue->pstQueueHead;
	pstQueue->pstQueueHead = (HLIST_NODE_T*)pstQueue->pstQueueHead->ppstPrevious;

	hlist_RemoveNode(pstHead);

	if (pstQueue->pstQueueHead == &pstQueue->stFrame)
	{
		pstQueue->pstQueueHead = NULL;
	}

	return pstHead;
}

#endif // end of header file guard
