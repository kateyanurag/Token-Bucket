
#include <stdio.h>
#include <token_bucket.h>
#include <math.h>
// Statistics variables


extern void print_stats(){
	// Average Inter arrival Time
	if(pkts_arrived != 0){
		fprintf(stdout, "\n Average Packet Inter-Arrival Time : (%Lf)/(%ld) = %.6Lf sec   ", avg_pkt_iat, pkts_arrived, avg_pkt_iat/(pkts_arrived*1000));
	}else{
		fprintf(stdout, "\n Average Packet Inter-Arrival Time : 0.00 sec , < no packets arrived >");	
	}
	
	// Average packet service time
	if(pkts_completed != 0){
		fprintf(stdout, "\n Average Packet Service Time : (%Lf)/(%ld) = %.6Lf sec   ",avg_pkt_serv_time, pkts_completed, avg_pkt_serv_time/(pkts_completed*1000));
	}else{
		fprintf(stdout, "\n Average Packet Service Time : 0.00 sec , < no packets processed by the server >");		
	}
	fprintf(stdout,"\n");
	
	// Average packets in Q1 and Q2 and S
	fprintf(stdout, "\n Average Packets in Q1 : To do "); 
	fprintf(stdout, "\n Average Packets in Q2 :  To do");
	fprintf(stdout, "\n Average Packets in S :  To do");	
	
	fprintf(stdout,"\n");
	// Average time spend by a packet in system
	if(pkts_completed != 0){
		fprintf(stdout, "\n Average time a Packet spends in system : (%Lf)/(%ld) = %.6Lf sec   ",avg_time_in_sys, pkts_completed, avg_time_in_sys/(pkts_completed));
	}else{
		fprintf(stdout, "\n Average time a Packet spends in system : 0.00 sec , < no packets processed by the server >");		
	}
	
	// standard deviation for time spent in system
	if(pkts_completed != 0){
		avg_time_in_sys = avg_time_in_sys/pkts_completed;
		avg_time_in_sys = avg_time_in_sys * avg_time_in_sys;
		sq_avg_time_in_sys = sq_avg_time_in_sys/pkts_completed;
		long double std_dev = sqrt((sq_avg_time_in_sys - avg_time_in_sys));
		fprintf(stdout, "\n SD for time spent in system : sqrt((%Lf) - (%Lf)) = %.6Lf sec   ",sq_avg_time_in_sys, avg_time_in_sys, std_dev);
	}else{
		fprintf(stdout, "\n SD for time spent in system : 0.00  , < no packets processed by the server >");
	}
	
	fprintf(stdout,"\n");
	// token drop prabability
	if(tokens_arrived != 0){
		fprintf(stdout, "\n Token drop probability :  (%ld)/(%ld) = %.6Lf", tokens_dropped, tokens_arrived, (long double)tokens_dropped/tokens_arrived);	
	}else{
		fprintf(stdout, "\n Token drop probability : 0.0 , < no tokens arrived > ");		
	}

	// packets drop prabability
	if(pkts_arrived != 0){
		fprintf(stdout, "\n Packet drop probability :  (%ld)/(%ld) = %.6Lf", pkts_dropped, pkts_arrived, (long double)pkts_dropped/pkts_arrived);	
	}else{
		fprintf(stdout, "\n Packet drop probability : 0.0 , < no packets arrived > ");		
	}	
}
