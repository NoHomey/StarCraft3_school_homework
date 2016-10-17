# StarCraft3_school_homework
Public Repository for a school homewrok for the `Operation system practice` course in ***ELSYS (TUES) - Technological shcool "Electronic systems"***. The idea of the homework was to do something practicle to exercise threads usage and whats better than implementing simple console based StarCraft3 game :)

Under ***ls*** directory is located simple `unix ls command` implementation which is again a school homework for the same course in ***ELSYS (TUES) - Technological shcool "Electronic systems"***.

Files located outside ***final/*** & ***os/final/*** are development files they are only used to save and stage the development proces.


#Important in final folders the files ***sc3.c*** were changed:

> if (main_status == 16) {	//If main_status is 16 this means that is currently locked by other thread.   
> +					i = 0;	//Reset i;    
> 					while(i < k) {	//Loop througth other commnad centers.

#Ivo
