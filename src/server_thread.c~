#include <stdio.h>
#include <token_bucket.h>
int stop_server = FALSE;
long double avg_pkt_serv_time = 0.0;
long double sq_avg_time_in_sys = 0.0;
long double avg_time_in_sys = 0.0;
long int pkts_completed = 0;
long double total_time_in_q2 = 0.0;
long double total_time_in_s = 0.0;
void server_thread_routine(){
	packet *p;
	long double t1, t2, time_in_q2 = 0.0;
	for(;;){
		fflush(stdout);
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
			pthread_mutex_unlock(&m);
			time_in_q2 = get_time()-p->q2_entry_time;
			fprintf(stdout,"\n %012.3Lfms : p%ld leaves Q2, time in Q2 = %.03Lfms", get_time(), p->pkt_num, time_in_q2);
			total_time_in_q2 = total_time_in_q2 + time_in_q2;	
			t1 = get_time();
			fprintf(stdout,"\n %012.3Lfms : p%ld begins service at S, requesting %.03Lfms of service time", t1, p->pkt_num, p->service_time/1000);
			usleep(p->service_time); // p->service_time is in milli sec. so convert it to micro secs.
			t2 = get_time();
			avg_pkt_serv_time += (t2-t1);
			total_time_in_s = total_time_in_s + (p->service_time/1000);
			fprintf(stdout,"\n %012.3Lfms : p%ld departs from S, Service time = %.03Lfms, time in system = %.03Lfms", t2, p->pkt_num, (t2-t1), (t2-p->arrival_time));
			pkts_completed ++;
			avg_time_in_sys += (t2-p->arrival_time)/1000; //convert into seconds
			sq_avg_time_in_sys += 	((t2-p->arrival_time)/1000) * ((t2-p->arrival_time)/1000);	
					
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
