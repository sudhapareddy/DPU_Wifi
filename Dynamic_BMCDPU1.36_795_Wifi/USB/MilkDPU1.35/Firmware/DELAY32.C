void delay(void)
{   
	char DCount1;    

	DCount1=0x03;
	while(DCount1>0)
	{
		DCount1--;
	}  
	return;
}

void delay_mis_old(char t)
{   	
	t=t>>3;
	while(t>0)
	  t--;
	  
	return;
}
void delay_ms_old(char t)
{     	
	while(t>0)
	{            	
		delay_mis_old(245);
		delay_mis_old(245);
		delay_mis_old(245);
		delay_mis_old(245);
		t--;
	}	
	
	return;
}


