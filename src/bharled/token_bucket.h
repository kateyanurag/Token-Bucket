

#ifndef __TOKEN_BUCKET_H__
#define __TOKEN_BUCKET_H__

#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <my402list.h>
#include <stdlib.h>

#define BUCKET 10; // bucket capacity
#define LAMBDA 0.5; // pkt arrival rate
#define MU 0.35; // pkt service rate
#define R 1.5; // token arrival rate
#define P 3; // tokens required by a packet
#define NUM 4; // total number of packets 

typedef struct struct_packet{
	long int pkt_num;
	long int tokens_req;
	long double arrival_time;
	long double q1_entry_time;
	long double q2_entry_time;	
	long double service_time;
	long double exit_time;
}packet;



extern long int token_count; // No of tokens in the bucket
extern long int bucket_capacity; // maximum bucket capicity
extern long double pkt_arr_rate; // packet arrival rate : lambda
extern long int tokens_req; // tokens required by a packet : P
extern long double token_arr_rate; // token arrive into bucket at this rate : r
extern long double pkt_service_rate; // packet service rate : mu 
extern long int total_pkts; // total number of packets to arrive

extern pthread_t packet_arrival_thread, signal_thread, token_depositing_thread, server_thread;
extern pthread_mutex_t m;
extern pthread_cond_t queue_not_empty;

extern char *mode; // deterministic or trace driven

extern int signal_caught; // TRUE or FALSE
extern int pkts_expected; // TRUE OR FALSE

extern long double start_time;

extern My402List *queue1;
extern My402List *queue2;


//extern void clean_up();
extern long double get_time();
extern int move_pkt_from_q1_to_q2();
extern void clean_up(void);
extern void print_stats(void);
extern void arrival_thread_routine(void *file_name);
extern void token_depositing_routine(void);
extern void server_thread_routine(void);

extern char all_pkts_dropped; // For special condition
// statistics variables
extern long double avg_pkt_iat; // average packet inter-arrival time
extern long double avg_pkt_serv_time; // average packet service time
extern long int pkts_arrived; // completed + dropped + removed
extern long int pkts_dropped;
extern long int pkts_completed;
extern long double avg_time_in_sys; // sum of time spent by completed packets in system [in seconds]
extern long double sq_avg_time_in_sys; // sum of squares of time spent by completed packets in system [in seconds]
extern long int tokens_arrived;
extern long int tokens_dropped;
#endif
