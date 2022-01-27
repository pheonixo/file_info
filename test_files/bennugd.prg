/*
    CREATE PLAYER - Example


	 
	
*/





include "crs/coldevrestserver.prg";



private
   int status;
   string APP_ID = "2618fc21-7d59-4eb3-b400-00c31154f81a";
   string APP_PASS= "MICLAVE";
   string USER = "laurita";
   string USER_PASS = "78127172mm";   
   string EMAIL = "laura@JKO.COM";
   
   
 
	
	
begin
	Set_mode(800,600,32);
	set_fps(60,0);	
	
	
	status= CRS_PLAYER_CREATE(APP_ID, APP_PASS, USER,USER_PASS, EMAIL);
	
    if (status == CRS_MSG_OK 	     )	
	  write(0,5,50,0,"PLAYER CREADo OK");
    end
	
	
   if (status <= 0)  //error ?	
	
    if (status == CRS_MSG_ERROR_EXISTS 	     )	
	  write(0,5,50,0,"Player ya existe");
	  
	else
	  write(0,5,50,0,"Player ERROR");
   	
	end
	
   end	//fin error  
     
	
 
	
	loop
		frame;
	end
end


 
	
