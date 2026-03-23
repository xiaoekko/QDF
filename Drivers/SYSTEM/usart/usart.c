#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "USART_.h"

#if SYS_SUPPORT_OS
#include "os.h"                              
#endif


#if 1
#if (__ARMCC_VERSION >= 6010050)                   
__asm(".global __use_no_semihosting\n\t");         
__asm(".global __ARM_use_no_argv \n\t");           

#else

#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;

};

#endif

int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}


void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}


FILE __stdout;


int fputc(int ch, FILE *f)
{
    while ((USART1->SR & 0X40) == 0);             

    USART1->DR = (uint8_t)ch;                    
    return ch;
}
#endif
    
#if USART_EN_RX                                   

uint8_t g_usart_rx_buf[USART_REC_LEN];


uint16_t g_usart_rx_sta = 0;
uint8_t g_rx_buffer[RXBUFFERSIZE];                  

#endif


 

 




