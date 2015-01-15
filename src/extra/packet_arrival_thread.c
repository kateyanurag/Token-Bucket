#include <stdio.h>
#include <token_bucket.h>

int pkts_expected = TRUE;

// This is a special variable which is used to kill Token and Server threads when all the incoming packets are dropped due to insufficient tokens
char all_pkts_dropped = 'y'; // initialize with yes
packet *get_packet(int i, long int tokens_req){
	packet *p = NULL;		
	p = (packet *)malloc(sizeof(packet));
	if(p == NULL){
		printf("\n packet cannot be created !");
		exit(0);
	}
	p->pkt_num = i;
	p->arrival_time = get_time();
	p->tokens_req = tokens_req;
	p->service_time = 500000.00;
	return p;
}

void arrival_thread_routine(){

	
	packet *p = NULL;
	int i = 1;
	long double curr_pkt_time, prev_pkt_time = 0.0;
	long double iat; // inter-arrival time

	for(i=1;i<=3;i++){
		// sleep for pkt inter arrival time
		//usleep((double)1000/pkt_arr_rate);// microseconds
		usleep(1000000);
		// after waking up create a packet
		p = get_packet(i, tokens_req);
		curr_pkt_time = get_time();
		// check if tokens required exceed the bucket capacity
		if(p->tokens_req > bucket_capacity){
			printf("\n %Lfms : p%ld arrives, needs %ld tokens, inter arrival time = %Lfms, dropped!! .. too many tokens required", curr_pkt_time, p->pkt_num, tokens_req, curr_pkt_time-prev_pkt_time); 
			free(p);
			pthread_mutex_unlock(&m);
			continue;
		}
		iat = curr_pkt_time-prev_pkt_time;
		printf("\n %Lfms : p%ld arrives, needs %ld tokens, inter arrival time = %Lfms", curr_pkt_time, p->pkt_num, tokens_req, iat);
		// increment avg_pkt_iat		
		avg_pkt_iat += iat;
		pthread_mutex_lock(&m);	
		if(signal_caught){
			printf("\n %Lfms : p%ld arrives, needs %ld tokens, inter arrival time = %Lfms, dropped <cntrl+c> pressed", curr_pkt_time, p->pkt_num, tokens_req, curr_pkt_time-prev_pkt_time);
			free(p);
			pkts_expected = FALSE;
			pthread_mutex_unlock(&m);
			break;
		}// end if
		prev_pkt_time = curr_pkt_time;

		// enque packet in Q1
		printf("\n %Lfms : p%ld enters queue1",get_time(),p->pkt_num);
		p->q1_entry_time = get_time();
		My402ListAppend(queue1, p);	
		all_pkts_dropped = 'n';
		//Move first packet from Q1 to Q2
		if(!move_pkt_from_q1_to_q2()){
			// if False returned i.e. required tokens not available, then go to sleep and generate tokens
			pthread_mutex_unlock(&m);
			continue;			
		}	
		// check if Q2 was empty before. if yes, then signal queue_not_empty condtion;
		if(My402ListLength(queue2) == 1){ // only one element which is added just now
			pthread_cond_signal(&queue_not_empty);
		}			
		pthread_mutex_unlock(&m);		

	} // end for

	printf("\n Outside for");	

	pthread_mutex_lock(&m);
	pkts_expected = FALSE;	
	pthread_cond_signal(&queue_not_empty);	
	pthread_mutex_unlock(&m);

	
	
	
	printf("\n PACKET THREAD RETURNING TO MAIN!!!");
	return;
} // end routine

