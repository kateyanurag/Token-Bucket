#include <stdio.h>
#include <token_bucket.h>
#include <string.h>
#include <stdlib.h>

int pkts_expected = TRUE;
long int pkts_arrived = 0;
long int pkts_dropped = 0;
long double avg_pkt_iat = 0.0; // average packet inter-arrival time


// This is a special variable which is used to kill Token and Server threads when all the incoming packets are dropped due to insufficient tokens
char all_pkts_dropped = 'x'; // initialize with random value

packet *get_packet(int i, long int tokens, long double serv_time){
	packet *p = NULL;		
	p = (packet *)malloc(sizeof(packet));
	if(p == NULL){
		printf("\n packet cannot be created !");
		exit(0);
	}
	p->pkt_num = i;
	if(strcmp(mode, "deterministic") == 0){
		p->tokens_req = tokens_req;
		p->service_time = 1000000.0/pkt_service_rate; // convert to micro sec
	}else{
		p->tokens_req = tokens;
		p->service_time = 1000.0 * serv_time; // convert to micro sec
	}
	return p;
}
int validate(char *pkt_details){
	int flag = TRUE, i=0;
	for(i=0;i<strlen(pkt_details);i++){
		if(!(pkt_details[i]>=48 && pkt_details[i]<=57)){
			if(pkt_details[i]=='\n') continue;
			flag = FALSE;
			break;
		}	
	}
	return flag;
}
void extract_pkt_details(char *pkt_details, long double *arr_time, long int *tokens, long double *serv_time){
	char ch;
   	char one[10]={'\0'}, two[10]={'\0'}, three[10]={'\0'};
  	int x=0, y=0, z=0;
   	int check = 1;
	int i = 0;
	// check for leading or trailing white space characters	
	if(pkt_details[0] == ' ' || pkt_details[0] == '\t' || pkt_details[strlen(pkt_details)-1] == ' ' || pkt_details[strlen(pkt_details)-1] == '\t' ){
		fprintf(stderr, "\n ERROR : Line in tsfile contains leading/trailing white space character(s) ... program exiting ");
		exit(1);
	}
	while((ch = pkt_details[i])!='\0'){
		if(ch == '\n'){
			i++;
			continue;		
		}
		if(ch>=48 && ch<=57){
			if(check == 1){
				one[x] = ch; x++;				
			}
			if(check == 2){
				two[y] = ch; y++;			
			}		
			if(check == 3){
				three[z] = ch; z++;			
			}
		}
		else if(ch == ' ' || ch == '\t'){
			
			if(check == 1 && strlen(one)!= 0){
				check = 2;						
			}	
			if(check == 2 && strlen(two)!= 0){
				check = 3;						
			}
			if(check == 3 && strlen(three)!= 0){
				fprintf(stderr, "\n ERROR : Line in tsfile contains trailing white space character(s) ... program exiting ");						
			}			
		}
		else{
			fprintf(stderr, "\n ERROR : Line in tsfile contains invalid character(s) ... program exiting ");
			exit(1);
		}
			
		i++;	
	}
	if(strlen(one) == 0 || strlen(two) == 0 || strlen(three) == 0){
		fprintf(stderr, "\n ERROR : Invalid TSFILE. Missing one of the fields, either inter-arrival time or token required or service time ... program exiting");
		exit(1);	
	}
	one[x]='\0'; two[y]='\0'; three[z]='\0';
	*arr_time = atol(one); *arr_time = (*arr_time); 
	*tokens = atol(two);
	*serv_time = atol(three); *serv_time = (*serv_time);
}
void arrival_thread_routine(void *file_name){

	
	packet *p = NULL;
	int i = 1;
	long double curr_pkt_time, prev_pkt_time = 0.0;
	long double arr_time=0.0, serv_time=0.0;  long int tokens=0; // fields in trace file
	long double iat; // inter-arrival time
	FILE *tsfile = NULL;
	void *check;
	int max_len = 50; 
	int file_is_open = FALSE; 
	char pkt_details[max_len]; // contains line read from tsfile in a trace drivem mode
	
	fflush(stdout);
	if(strcmp(mode, "deterministic") == 0){
		if(1/pkt_arr_rate > 10.0){
			pkt_arr_rate = 0.1;		
		}if(1/pkt_service_rate > 10.0){
			pkt_service_rate = 0.1;		
		}	
	}
	// if trace driven mode
	if(strcmp(mode, "tracedriven") == 0){
		fprintf(stdout, "\n ************* trace driven mode  **************");
		tsfile = fopen((char *)file_name, "r");
		if(tsfile == NULL){
			fprintf(stderr,"\n *** ERROR : Trace Driven file cannot be opened. Either invalid or corrupt or access denied ..... program exiting\n");		
			exit(1);		
		}
		file_is_open = TRUE;
		check = fgets(pkt_details,max_len,tsfile);
		if(check == NULL){
			fprintf(stderr,"\n *** ERROR : Unable to read from Trace file ..... program exiting");
			fclose(tsfile);			
			exit(2);		
		}
		if(validate(pkt_details)){
			total_pkts = atoi(pkt_details);			
		}else{
			fprintf(stderr,"\n *** ERROR : Trace Driven file is in incorrect format ..... program exiting\n");
			exit(1);		
		}
			
	}else{
		fprintf(stdout, "\n ************** deterministic mode  ***************");
	}
	for(i=1;i<=total_pkts;i++){
		
			
			
		fflush(stdout);
		// if file_open is TRUE i.e running in trace_driven mode
		if(file_is_open){
		// get packet details from trace file		
			check = fgets(pkt_details,max_len,tsfile);
			extract_pkt_details(pkt_details, &arr_time, &tokens, &serv_time); // extract the info from the record read from file
		}		
				
		// sleep for pkt inter arrival time
		if(strcmp(mode, "tracedriven") == 0){
			usleep(arr_time*1000);			
		}else{	
			usleep(1000000/pkt_arr_rate);
		} 
		

		// after waking up check if cntrl+c is pressed 
		pthread_mutex_lock(&m);
			if(signal_caught){
			 pthread_mutex_unlock(&m);
			 break;		
		}	
		pthread_mutex_unlock(&m);
		
		// create a packet
		p = get_packet(i, tokens, serv_time);
		curr_pkt_time = get_time();
		p->arrival_time = curr_pkt_time;
		iat = curr_pkt_time-prev_pkt_time;
		avg_pkt_iat += iat;
		pkts_arrived ++;
		// check if tokens required exceed the bucket capacity
		if(p->tokens_req > bucket_capacity){
			printf("\n %012.3Lfms : p%ld arrives, needs %ld tokens, inter arrival time = %.03Lfms", curr_pkt_time, p->pkt_num, p->tokens_req, curr_pkt_time-prev_pkt_time); 
			printf("\n    \t << ** p%ld DROPPED, too many tokens required ** >>", p->pkt_num);
			pkts_dropped ++;
			free(p);
			prev_pkt_time = curr_pkt_time;
			continue;
		}
		
		printf("\n %012.3Lfms : p%ld arrives, needs %ld tokens, inter arrival time = %.03Lfms", curr_pkt_time, p->pkt_num, p->tokens_req, iat);
		// increment avg_pkt_iat		
		pthread_mutex_lock(&m);	
		if(signal_caught){
			printf("\n    \t << ** p%ld DROPPED, <cntrl+c> pressed  by the user ** >>", p->pkt_num);
			free(p);
			pkts_expected = FALSE;
			pthread_mutex_unlock(&m);
			break;
		}// end if
		prev_pkt_time = curr_pkt_time;

		// enque packet in Q1
		printf("\n %012.3Lfms : p%ld enters queue1",get_time(),p->pkt_num);
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
	
	pthread_mutex_lock(&m);
	pkts_expected = FALSE;
	if(all_pkts_dropped != 'n' ){
		all_pkts_dropped = 'y';			
	}	
	pthread_cond_signal(&queue_not_empty);	
	pthread_mutex_unlock(&m);

	return;
} // end routine

