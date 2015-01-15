#include <stdio.h>
#include <token_bucket.h>
int stop_server = FALSE;
void server_thread_routine(){
	packet *p;
	long double t1, t2;
	for(;;){
		pthread_mutex_lock(&m);
		if(signal_caught){
			pthread_mutex_unlock(&m);
			break;		
		}
		if(My402ListEmpty(queue2) && My402ListEmpty(queue1) && pkts_expected == FALSE){
			// stop processing
			pthread_mutex_unlock(&m);
			stop_server = TRUE;
			break;		
		}
		if(My402ListEmpty(queue2) && (!My402ListEmpty(queue1) || pkts_expected)){ 
		// if Q2 is empty, wait for queue_not_empty condition to be signaled
			
			pthread_cond_wait(&queue_not_empty, &m);	
		}
		if(!My402ListEmpty(queue2)){
		// If Queue2 is not empty, then deque a pkt from it and process it
			My402ListElem *elem = (My402ListElem *)My402ListFirst(queue2);
			p = (packet *)elem->obj;
			My402ListUnlink(queue2, elem);	
			fprintf(stdout,"\n %Lfms : p%ld leaves Q2, time in Q2 = %Lfms", get_time(), p->pkt_num, get_time()-p->q2_entry_time);	
			pthread_mutex_unlock(&m);	
			t1 = get_time();
			fprintf(stdout,"\n %Lfms : p%ld begins service at S, requesting %Lfms of service time", t1, p->pkt_num, p->service_time);
			usleep(p->service_time);
			t2 = get_time();
			fprintf(stdout,"\n %Lfms : p%ld departs from S, Service time = %Lfms, time in system = %Lfms", t2, p->pkt_num, t2-t1, t2-p->arrival_time);
			free(p);
		}else{
		// If Queue2 is empty
			pthread_mutex_unlock(&m);
		}
		
	} // end for
	if(stop_server){
		// cancel signal threads and stop server thread i.e return
		pthread_cancel(signal_thread);
	}
	return;
}
