#include <token_bucket.h>
#include <stdio.h>
#include <thread_modules.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
 
// extern variables set to default values
long int token_count = 0;
long int bucket_capacity = BUCKET;
long double pkt_arr_rate = LAMBDA;
long int tokens_req = P;
long double token_arr_rate = R;
long double pkt_service_rate = MU;
long int total_pkts = NUM;
int signal_caught = FALSE;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
My402List *queue1, *queue2;
long double start_time;
pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;

pthread_t packet_arrival_thread, signal_thread, token_depositing_thread, server_thread;

char *mode = "deterministic"; // deterministic or trace driven
//

typedef struct warmup2_signal{
		sigset_t set;
		int sig;
	}signals;

//


sigset_t set;
struct sigaction act;
int *sig_th_status;
int sig;
int result = -1;
char *file_name = "empty";

extern long double get_time(){
 	struct timeval t;
	long double sec, usec, time;
	gettimeofday(&t,NULL);
	sec = (long double)t.tv_sec;
	usec = (long double)t.tv_usec;
	time = (sec*1000000)+usec;
	return (time/1000)-start_time; // return time in miliseconds
}
long double get_start_time(){
 	struct timeval t;
	long double sec, usec, time;
	gettimeofday(&t,NULL);
	sec = (long double)t.tv_sec;
	usec = (long double)t.tv_usec;
	time = (sec*1000000)+usec;
	return (time/1000); // return time in miliseconds
}

void handler_fun()
{
	int result;
	//printf("\n ^^^^^^^^ Press cntrl+c to break !!!");
	result = sigwait(&set,&sig);
	//printf("\n ############ After sig wait");	
	if(result == 0){
		//printf("\n $$$$$$$$$$ <cntrl+c> caught for SIGNAL : %d!!!",sig);	
		pthread_mutex_lock(&m);
		signal_caught = TRUE;
		pthread_mutex_unlock(&m);
	}else{
		printf("\n Error occurred after pressing <cntrl+c>");	
	}
	return;
} 

void init(){
	
	queue1 = (My402List *)malloc(sizeof(My402List));
	if(!queue1){
		printf("\n Error in creating queue1"); exit(1);	
	}
	queue2 = (My402List *)malloc(sizeof(My402List));
	if(!queue2){
		printf("\n Error in creating queue1"); exit(1);	
	}
	My402ListInit(queue1);
	My402ListInit(queue2);
}

int is_valid(char *option){
	char *valid_options[] = {"-lambda", "-mu", "-r", "-B", "-P", "-n", "-t"};
	int flag = FALSE;
	int i;
	for(i=0;i<7;i++){
		if(strcmp(valid_options[i], option) == 0){
			flag = TRUE;
			break;	
		}
	}
	return flag;
}

int parse_args(char *option, char *value){
	char *usage = "warmup [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]";
	int flag = TRUE;
	if(is_valid(option)){
		 if(strcmp(option,"-B") == 0)
			bucket_capacity = atoi(value);
		 else if(strcmp(option,"-P") == 0) 
			tokens_req = atoi(value);
		 else if(strcmp(option,"-n") == 0)
			total_pkts = atoi(value);
		 else if(strcmp(option,"-lambda") == 0)
			pkt_arr_rate = atof(value);	
		 else if(strcmp(option,"-mu") == 0)
			pkt_service_rate = atof(value);
		 else if(strcmp(option,"-r") == 0)
			token_arr_rate = atof(value);	
		 else if(strcmp(option,"-t") == 0){
			file_name = value;
		 	mode = "tracedriven";
		 }
			
	}else{
		fprintf(stdout,"\n ERROR : Invalid option '%s'.\n Correct Usage : %s\n", option, usage);
		flag = FALSE;
	}
	return flag;
}

int main(int argc, char *argv[]){
	
	int rv = -1;
	void **value_ptr = NULL;
        int flag;
	char *usage = "warmup [-lambda lambda] [-mu mu] [-r r] [-B B] [-P P] [-n num] [-t tsfile]";
	if(argc > 15 || ((argc-1)%2 != 0)){
		fprintf(stderr, "\n ERROR : Incorrect usage of command 'warmup2'.\n Correct Usage : %s\n", usage);	
		exit(1);
	}
	int i = 0;
	for(i=1;i<argc;i+=2){
		flag = parse_args(argv[i], argv[i+1]);
		if(!flag)
			exit(1);	
	}
	
	fprintf(stdout, "\n EMULATION PARAMETERS : ");
	if(strcmp(file_name,"empty") == 0){
		fprintf(stdout, "\n Number of packets to arrive : %ld", total_pkts);
	}
	fprintf(stdout, "\n r : %Lf", token_arr_rate);
	fprintf(stdout, "\n B : %ld", bucket_capacity);
	if(strcmp(file_name,"empty") == 0){
		fprintf(stdout, "\n lambda : %Lf", pkt_arr_rate);
		fprintf(stdout, "\n mu : %Lf", pkt_service_rate);
		fprintf(stdout, "\n P : %ld", tokens_req);
	}else{
		fprintf(stdout, "\n tsfile : %s", file_name);
	}
	
	fprintf(stdout, "\n 000.000ms : EMULATION BEGINS");
	start_time = get_start_time();
	//printf("\n Start time :%Lf", start_time);

	
	sigemptyset(&set);	
	sigaddset(&set, SIGINT);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
	pthread_create(&signal_thread, 0, (void *)&handler_fun, NULL);	

	init();

	pthread_create(&packet_arrival_thread, 0, (void *)&arrival_thread_routine, file_name);
	pthread_create(&token_depositing_thread, 0, (void *)&token_depositing_routine, NULL);
	pthread_create(&server_thread, 0, (void *)&server_thread_routine, NULL);


	// join all the child threads
	pthread_join (signal_thread, value_ptr);
	pthread_join (packet_arrival_thread, value_ptr);
	pthread_join (token_depositing_thread, value_ptr);
	pthread_join (server_thread, value_ptr);

	
	
	clean_up();
	printf("\n\n %Lfms : EMULATION ENDS",get_time());	
	fprintf(stdout, "\n\n\n********** STATISTICS ************");
	print_stats();
	fprintf(stdout,"\n***************************************\n\n");
	
	rv = 0;
	return rv;	
}
