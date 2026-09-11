#include<lpc21xx.h>
#include"uart.h"
#include"delay.h"
	  char buff[300];
	  int i=0;
int main()
{
init_uart();
while(1)
{
//string_uart("AT\r\n");
						esp01_connectAP_AT();

delay_ms(1000);
}
}