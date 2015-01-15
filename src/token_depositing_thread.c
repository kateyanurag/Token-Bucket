# include <stdio.h>
# include <token_bucket.h>
# include <string.h>

long int tokens_arrived = 0;
long int tokens_dropped = 0;

long double total_time_in_q1 = 0.0; // total time spent by all packets in Q1

int move_pkt_from_q1_to_q2(){
	
	int flag = FALSE;
	long double time_in_q1 =0.0;
	// Q1 is not empty then deque a packet from it.
	My402ListElem *elem = (My402ListElem *)My402ListFirst(queue1);
	packet *p = (packet *)elem->obj;
	if(p->tokens_req <= token_count){
		// if the required no of tokens are available in bucket, then take out the pkt from Q1 and decrement the bucket count
		token_count -= p->tokens_req;
		My402ListUnlink(queue1, elem);
		time_in_q1 = get_time()-p->q1_entry_time;
		printf("\n %012.3Lfms : p%ld leaves queue1, time in queue1 %.03Lf, Bucket now has %ld tokens", get_time(), p->pkt_num, time_in_q1, token_count);
		total_time_in_q1 = total_time_in_q1 + time_in_q1;
		printf("\n %012.3Lfms : p%ld enters queue2",get_time(),p->pkt_num);
		p->q2_entry_time = get_time();	
		My402ListAppend(queue2, p);
		flag = TRUE;
	}
	return flag;		
}

void token_depositing_routine(){
	int i;
	if(strcmp(mode, "deterministic") == 0){ 
			if((1/token_arr_rate)>(long double)10){
				token_arr_rate = 10.00;
			}		
		}
	for(i=1;;i++){
		fflush(stdout);
		usleep(1000000/token_arr_rate);
		pthread_mutex_lock(&m);	
		if(signal_caught){
			// <cntrl+c> pressed, so exit.
			pthread_mutex_unlock(&m);
			break;		
		}
		if(token_count < bucket_capacity){
			// Increment token count
			token_count++;
			printf("\n %012.3Lfms : token t%d arrives, token bucket now has %ld tokens ", get_time(), i, token_count);
					
		}else{
			// Bucket Overflow
			printf("\n %012.3Lfms : token t%d arrives, dropped ", get_time(), i);
			tokens_dropped ++;
		}

		tokens_arrived ++;

		if(My402ListEmpty(queue1)){ 
			// no packets in Q1
			if(pkts_expected == FALSE || all_pkts_dropped == 'y'){ 
				// Q1 empty and no more packets are expected to be in Q1. So exit. 			
				pthread_mutex_unlock(&m);
				break;
			}else{
				// Q1 empty but packets may arrive in it. So go to sleep;
				pthread_mutex_unlock(&m);
				continue;			
			}
		}else{ 
			// if Q1 is not empty then move packet from Q1 to Q2
			if(!move_pkt_from_q1_to_q2()){
				// if False returned i.e. required tokens not available, then go to sleep and generate tokens
				pthread_mutex_unlock(&m);
				continue;			
			}	
		}
		// check if Q2 was empty before. if yes, then signal queue_not_empty condtion;
		if(My402ListLength(queue2) == 1){ // only one element which is added just now
			pthread_cond_signal(&queue_not_empty);
		}
		
		pthread_mutex_unlock(&m);
	}// end for
	pthread_mutex_lock(&m);
	pthread_cond_signal(&queue_not_empty);
	pthread_mutex_unlock(&m);
	return;
}
