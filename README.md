# StarCraft3_school_homework
> Public Repository for school homewrok

Files located outside ***final/*** & ***os/final/*** are development files they are only used to save and stage the development proces.


#Important in final folders the files ***sc3.c*** were changed:

> if (main_status == 16) {	//If main_status is 16 this means that is currently locked by other thread.   
> +					i = 0;	//Reset i;    
> 					while(i < k) {	//Loop througth other commnad centers.  
