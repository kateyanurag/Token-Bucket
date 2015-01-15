#include "my402list.h"
#include "stdio.h"
#include "stdlib.h"

extern int My402ListLength(My402List* linkedList){
	return linkedList->num_members;	
}

extern int My402ListEmpty(My402List* linkedList){
    
	int flag = TRUE;
	if(linkedList->num_members==0){
	}else{
		flag = FALSE;
	}
	return flag;
	
}

int My402ListAppend(My402List* linkedList, void* obj){
	
	int flag = FALSE;
	My402ListElem* elem = (My402ListElem*)malloc(sizeof(My402ListElem)); 
	if(!elem){
		flag = FALSE;
	}else{
		
		elem->obj = obj;

		if(linkedList->num_members == 0){
			linkedList->anchor.prev = elem;
               	 	elem->next = &(linkedList->anchor);
	               	linkedList->anchor.next = elem;
			elem->prev = &(linkedList->anchor);
        	}
        	else{
			My402ListElem* last = My402ListLast(linkedList);
			last->next = elem;
			elem->prev = last;
			elem->next = &(linkedList->anchor);
			linkedList->anchor.prev = elem;
        	}
		linkedList->num_members++;
		flag = TRUE;
	}
	return flag;

}

int My402ListPrepend(My402List* linkedList, void* obj){
	
	int flag = FALSE;
        My402ListElem* elem = (My402ListElem*)malloc(sizeof(My402ListElem));
        if(!elem){
                flag = FALSE;
        }else{
                elem->obj = obj;
                if(linkedList->num_members == 0){
                	linkedList->anchor.next = elem;
                	elem->prev = &(linkedList->anchor);
                        linkedList->anchor.prev = elem;
                        elem->next = &(linkedList->anchor);
                }
                // If list already has some elememts
                else{
			My402ListElem* first = My402ListFirst(linkedList);
                        first->prev = elem;
                        elem->next = first;
			elem->prev = &(linkedList->anchor);
			linkedList->anchor.next = elem;
                }
                linkedList->num_members++;
		flag=TRUE;
        }
        return flag;
}

void My402ListUnlink(My402List* linkedList, My402ListElem* elem){
	
	if(elem != NULL && linkedList != NULL){
		if(linkedList->num_members == 1){
			linkedList->anchor.prev = &(linkedList->anchor);
			linkedList->anchor.next = &(linkedList->anchor);
		}else if(elem->prev == &(linkedList->anchor)){
			linkedList->anchor.next = elem->next;
			elem->next->prev = &(linkedList->anchor);
		}else if(elem->next == &(linkedList->anchor)){
			linkedList->anchor.prev = elem->prev;
			elem->prev->next = &(linkedList->anchor);
		}else{
			elem->prev->next = elem->next;
			elem->next->prev = elem->prev;
		}
	}
	free(elem);
	linkedList->num_members--;
}

extern void My402ListUnlinkAll(My402List* linkedList){

	My402ListElem *ptr1, *ptr2;
	ptr1 = linkedList->anchor.next;
	ptr2 = ptr1;
	while(ptr1!=&(linkedList->anchor)){
		ptr1 = ptr1->next;
		free(ptr2);
		ptr2=ptr1;
	}
	My402ListInit(linkedList);
}

int My402ListInsertAfter(My402List* linkedList, void* obj, My402ListElem* elem){
	int flag = FALSE;
		
	if(elem == NULL || elem->next == &(linkedList->anchor)){
		flag = My402ListAppend(linkedList, obj);
	}else{
		My402ListElem* e = (My402ListElem*)malloc(sizeof(My402ListElem));
		if(e){
		e->obj = obj;
		e->next = elem->next;
		e->prev = elem;
		e->next->prev = e;
		elem->next = e;	
		linkedList->num_members++;
		flag = TRUE;
		}	
	}
	return flag;	
}

int My402ListInsertBefore(My402List* linkedList, void* obj, My402ListElem* elem){
	int flag = FALSE;
	
	
	if(elem == NULL || elem->prev == &(linkedList->anchor)){
		flag = My402ListPrepend(linkedList, obj);
	}else{
		My402ListElem* e = (My402ListElem*)malloc(sizeof(My402ListElem));
		if(e){
			e->obj = obj;
			e->next = elem;
			e->prev = elem->prev;
			elem->prev = e;
			e->prev->next = e;
			linkedList->num_members++;
			flag = TRUE;
		}
	}
	return flag;
}

My402ListElem* My402ListFirst(My402List* linkedList){

	My402ListElem* e = NULL;
	if(linkedList->num_members != 0)
		e = linkedList->anchor.next;
	return e;
}


My402ListElem* My402ListLast(My402List* linkedList){
       return (linkedList->anchor).prev;
}

My402ListElem* My402ListNext(My402List* linkedList, My402ListElem* elem){
	My402ListElem* e = NULL;
	if(elem->next != &(linkedList->anchor)){
		e = elem->next;
	}
	return e;
}

My402ListElem* My402ListPrev(My402List* linkedList, My402ListElem* elem){
        My402ListElem* e = NULL;
	if(elem->prev != &(linkedList->anchor)){
		e = elem->prev;
	}
	return e;
}

My402ListElem* My402ListFind(My402List* linkedList, void* obj){
        My402ListElem* e;
	My402ListElem* ptr;
	e = NULL;
	ptr = linkedList->anchor.next;
	while(ptr!=&(linkedList->anchor)){
		if(ptr->obj == obj){
			e = ptr;
			break;
		}
		ptr = ptr->next;
	}
	return e;
}

extern int My402ListInit(My402List* linkedList){
	linkedList->anchor.obj = NULL;
	linkedList->anchor.next = &(linkedList->anchor);
	linkedList->anchor.prev = &(linkedList->anchor);
	linkedList->num_members = 0;
	return TRUE;
}

