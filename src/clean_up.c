#include <token_bucket.h>
#include <stdio.h>
void clean_up(){
	int len = 0;
	My402ListElem *elem;
	packet *p;
	if(!My402ListEmpty(queue1)){
		len = My402ListLength(queue1);	
		while(len>0){
			elem = My402ListFirst(queue1);
			p = (packet *)elem->obj;
			fprintf(stdout, "\n << p%ld removed from Queue1. >>", p->pkt_num);
			My402ListUnlink(queue1, elem);
			len --;		
		}
	}
	if(!My402ListEmpty(queue2)){
		len = My402ListLength(queue2);		
		while(len>0){
			elem = My402ListFirst(queue2);
			p = (packet *)elem->obj;
			fprintf(stdout, "\n << p%ld removed from Queue2. >>", p->pkt_num);
			My402ListUnlink(queue2, elem);	
			len --;	
		}
	}
	My402ListUnlinkAll(queue1);
	My402ListUnlinkAll(queue2);
	free(queue1);
	free(queue2);

}
