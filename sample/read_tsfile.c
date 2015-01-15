#include<stdio.h>
void extract_pkt_details(char *pkt_details){
	int i;
	char one[10], two[10], three[10];
	int chk = 1;
	int x = 0;
	one[0] = '\0';
	two[0] = '\0';
	three[0] = '\0';
	for(i=0;i<strlen(pkt_details);i++){
		if(pkt_details[i]>=48 || pkt_details[i]<=57){
			if(chk == 1){
									
			}else if(chk == 2){
									
			}else if(chk == 3){
				
			}
		}else if(pkt_details[i] == ' ' || pkt_details[i] == '\t'){
			if(chk == 1 && strlen(one)!=0){
				ckk = 2; x = 0;			
			}else if(chk == 2 && strlen(two)!=0){
				ckk = 3; x = 0;			
			}else if(chk == 3 && strlen(three)!=0){
				ckk = 2; x = 0;			
			}
		}else{
			fprintf(stderr, "\n Invalid ts file. Invalid characters");		
		}	
	}
	
}
void main(){
	
}
