const UINT8 DEFAULT_COMPANY_NAME[][32]={                                  
                                        "SUN IMPEX                       ",
};

const UINT8 DEFAULT_DEALER_NAME[][16]={  
                                        "   *SUN IMPEX*  ",
};

const UINT8 DEFAULT_BRAND_NAME[][16]={  
                                        " *SUN IMPEX DPU*",
};

const UINT8 DEFAULT_DEVICE_NO[][7]={  "SUN9999",
    
};

const UINT16 DEFAULT_LOGIN_PASSWORD[]={1234};

const UINT16 DEFAULT_USER_LOGIN_PASSWORD[]={1111};

const UINT32 DEFAULT_DEALER_PASSWORD[]={1977};

const UINT32 DEFAULT_MASTER_PASSWORD[]={6031977};;


    const UINT8 DEFAULT_DEALER_MULTIPLY_BY[]={36};
    const UINT8 DEFAULT_MASTER_MULTIPLY_BY[]={39};

const UINT8 DEFAULT_TARE_COMMAND[]={'t'};
       
BYTE M_COMPANY[32];
BYTE M_DEALER[16];
BYTE M_BRAND[16];  

BYTE M_DEVICE_NO[7]="SUN0001";   
BYTE M_MFG_DATE[6]="210525";
UINT16 M_BATCH_NO=125; 

UINT16 M_LOG_PASSWORD;
UINT16 M_USER_LOG_PASSWORD;

UINT32 M_DEALER_PASSWORD;
UINT32 M_MASTER_PASSWORD;
 
UINT8 M_DEALER_MULTIPLY_BY;
UINT8 M_MASTER_MULTIPLY_BY;

UINT8 M_TARE_COMMAND;


void Select_Dealer_Default_Values(UINT8 loop_cnt)
{
    UINT8 *def_addr;   
 
    def_addr=(char *)&DEFAULT_COMPANY_NAME[loop_cnt][0];
    strncpy(M_COMPANY,def_addr,32);

    def_addr=(char *)&DEFAULT_DEALER_NAME[loop_cnt][0];
    strncpy(M_DEALER,def_addr,16);

    def_addr=(char *)&DEFAULT_BRAND_NAME[loop_cnt][0];
    strncpy(M_BRAND,def_addr,16);

    def_addr=(char *)&DEFAULT_DEVICE_NO[loop_cnt][0];
    strncpy(M_DEVICE_NO,def_addr,7);

    M_LOG_PASSWORD=DEFAULT_LOGIN_PASSWORD[loop_cnt];       
    M_USER_LOG_PASSWORD=DEFAULT_USER_LOGIN_PASSWORD[loop_cnt];
    
    M_TARE_COMMAND=DEFAULT_TARE_COMMAND[loop_cnt];   

    M_DEALER_PASSWORD=DEFAULT_DEALER_PASSWORD[loop_cnt];
    M_MASTER_PASSWORD=DEFAULT_MASTER_PASSWORD[loop_cnt];        

    M_DEALER_MULTIPLY_BY=DEFAULT_DEALER_MULTIPLY_BY[loop_cnt];
    M_MASTER_MULTIPLY_BY=DEFAULT_MASTER_MULTIPLY_BY[loop_cnt];
    
}
