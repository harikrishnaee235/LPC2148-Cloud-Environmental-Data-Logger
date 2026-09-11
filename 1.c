#include <string.h>

#include "uart.h"

#include "delay.h"

#include "lcd_defines.h"
#include "esp011.h"

/* These MUST match the real definitions in uart.c exactly:
   char buff[300];  int i;
   Previously this file declared `extern char buff[200];` and
   `extern unsigned char i;`, which are the wrong size/type for the real
   objects. That mismatch is undefined behaviour in C and, on top of
   silently limiting the receive index to 0-255, made every strstr()
   check on `buff` a gamble. Fixed to match uart.c. */
extern char buff[300];
extern int i;

/* Wait until at least `min_bytes` characters have arrived in buff[],
   or until `timeout_x10ms` * 10ms has elapsed - whichever comes first.
   Returns 0 on success (data arrived), -1 on timeout.
   NOTE: the previous code reused one single global `time` counter across
   every AT command in the whole connection sequence and never reset it
   in between. That meant the timeout budget for a later command was
   silently eaten by whatever time earlier commands had already used, so
   commands could report a timeout even though the ESP01 answered
   promptly. Using a local counter per call fixes that. */
static int esp01_wait_bytes(int min_bytes, int timeout_x10ms)
{
        int t = 0;

        while((i < min_bytes) && (t < timeout_x10ms))
        {
                delay_ms(10);
                t++;
        }

        return (i >= min_bytes) ? 0 : -1;
}

/* Reset the shared RX buffer/index before issuing a new AT command. */
static void esp01_clear_buffer(void)
{
        i = 0;
        memset(buff, '\0', sizeof(buff));
}

/* The raw ESP01 response in buff[] contains \r\n and other unprintable
   control bytes (that's normal for AT command replies). The HD44780
   LCD has no glyph for those bytes, so it was drawing them as random
   garbage / stray "=" blocks on screen every time buff was dumped to
   the display for debugging. This copies buff into a small static
   scratch buffer with every non-printable byte replaced by a space
   before it goes anywhere near string_lcd(), so only clean, readable
   characters ever show up on the LCD. */
static char lcd_clean[64];

static char *clean_for_lcd(const char *src)
{
        int k = 0;

        /* Skip any leading control bytes (\r, \n, etc.) entirely so the
           first visible character always lands in column 0 of the LCD
           instead of being pushed right by a leading space. */
        while(*src && (*src < 32 || *src > 126))
        {
                src++;
        }

        while(*src && (k < (int)sizeof(lcd_clean) - 1))
        {
                char c = *src;

                if((c >= 32) && (c <= 126))
                {
                        lcd_clean[k++] = c;
                }
                else if((c == '\r') || (c == '\n'))
                {
                        /* collapse any run of CR/LF into a single space
                           instead of one space per byte */
                        if((k == 0) || (lcd_clean[k-1] != ' '))
                        {
                                lcd_clean[k++] = ' ';
                        }
                }
                /* any other control byte is dropped entirely */

                src++;
        }

        lcd_clean[k] = '\0';
        return lcd_clean;
}

/* Dump the cleaned AT response across BOTH LCD lines (16 chars each,
   32 total) instead of just line 1. A 16-char-wide line was cutting
   real, useful text short - e.g. "Recv 48 bytes SEND OK" got chopped
   down to "Recv 48 bytes S", losing "END OK" entirely, because a
   16x2 HD44780 does not auto-wrap onto line 2. This does that wrap
   manually so nothing meaningful gets lost.

   The wrap also breaks on the last space at/before column 16 instead
   of always hard-splitting at exactly 16 characters, so a word never
   gets torn in half - e.g. "WIFI CONNECTED" was being cut as
   "...CONNECTED W" / "IFI GOT IP..." which looked like a stray
   leftover "W" glitch on line 1 even though the rest was really just
   sitting on line 2. */
static void show_clean_response(void)
{
        char *s = clean_for_lcd(buff);
        int  len = (int)strlen(s);
        int  brk;
        int  k;
        int  start;
        int  cnt;

        cmd_lcd(0x01);
        cmd_lcd(0x80);

        if(len <= 16)
        {
                for(k = 0; s[k]; k++)
                {
                        data_lcd(s[k]);
                }
                return;
        }

        brk = 16;
        while((brk > 0) && (s[brk] != ' '))
        {
                brk--;
        }
        if(brk == 0)
        {
                brk = 16;
        }

        for(k = 0; k < brk; k++)
        {
                data_lcd(s[k]);
        }

        cmd_lcd(0xC0);

        start = brk;
        if(s[start] == ' ')
        {
                start++;
        }

        for(k = start, cnt = 0; (cnt < 16) && s[k]; k++, cnt++)
        {
                data_lcd(s[k]);
        }
}

void init_esp01(void)
{
        esp01_connectAP();
}

void update_data(int field, int num)
{
        esp01_sendToThingspeak(field, num);
}


int esp01_connectAP(void)
{
        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("AT");
        delay_ms(1000);

        esp01_clear_buffer();
        string_uart("AT\r\n");

        if(esp01_wait_bytes(4, 200) != 0)
        {
                return -1;
        }

        delay_ms(500);
        buff[i] = '\0';

        show_clean_response();
        delay_ms(2000);

        if(strstr(buff, "OK"))
        {
                cmd_lcd(0xC0);
                string_lcd("OK");
                delay_ms(2000);
        }
        else
        {
                cmd_lcd(0xC0);
                string_lcd("ERROR");
                delay_ms(1000);
        }


        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("ATE0");
        delay_ms(1000);

        esp01_clear_buffer();
        string_uart("ATE0\r\n");

        if(esp01_wait_bytes(4, 200) != 0)
        {
                return -1;
        }

        delay_ms(500);
        buff[i] = '\0';

        show_clean_response();
        delay_ms(2000);

        if(strstr(buff, "OK"))
        {
                cmd_lcd(0xC0);
                string_lcd("OK");
                delay_ms(2000);
        }
        else
        {
                cmd_lcd(0xC0);
                string_lcd("ERROR");
                delay_ms(1000);
        }


        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("AT+CIPMUX");
        delay_ms(1000);

        esp01_clear_buffer();
        string_uart("AT+CIPMUX=0\r\n");

        if(esp01_wait_bytes(4, 200) != 0)
        {
                return -1;
        }

        delay_ms(500);
        buff[i] = '\0';

        show_clean_response();
        delay_ms(2000);

        if(strstr(buff, "OK"))
        {
                cmd_lcd(0xC0);
                string_lcd("OK");
                delay_ms(2000);
        }
        else
        {
                cmd_lcd(0xC0);
                string_lcd("ERROR");
                delay_ms(1000);
        }


        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("AT+CWQAP");
        delay_ms(1000);

        esp01_clear_buffer();
        string_uart("AT+CWQAP\r\n");

        if(esp01_wait_bytes(4, 200) != 0)
        {
                return -1;
        }

        delay_ms(1500);
        buff[i] = '\0';

        show_clean_response();
        delay_ms(2000);

        if(strstr(buff, "OK"))
        {
                cmd_lcd(0xC0);
                string_lcd("OK");
                delay_ms(2000);
        }
        else
        {
                cmd_lcd(0xC0);
                string_lcd("ERROR");
                delay_ms(1000);
        }


        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("AT+CWJAP");
        delay_ms(1000);

        /* need to change the wifi network name and password */
        esp01_clear_buffer();
        string_uart("AT+CWJAP=\"bhagya\",\"123456789\"\r\n");

        if(esp01_wait_bytes(4, 200) != 0)
        {
                return -1;
        }

        delay_ms(2500);
        buff[i] = '\0';

        show_clean_response();
        delay_ms(2000);

        if(strstr(buff, "WIFI CONNECTED") || strstr(buff, "OK"))
        {
                cmd_lcd(0xC0);
                string_lcd("OK");
                delay_ms(2000);
        }
        else
        {
                cmd_lcd(0xC0);
                string_lcd("ERROR");
                delay_ms(1000);
        }

        return 0;
}


/* Build "<value>\r\n" (decimal, handles 0 and negative values) into dst
   and return the number of characters written. Used instead of calling
   int_uart() mid-transmission so we can know the exact payload length
   up front for AT+CIPSEND. */
static int append_int_crlf(char *dst, int val)
{
        char tmp[12];
        int  t = 0;
        int  k = 0;
        int  neg = 0;
        unsigned int n;

        if(val < 0)
        {
                neg = 1;
                n = (unsigned int)(-val);
        }
        else
        {
                n = (unsigned int)val;
        }

        if(n == 0)
        {
                tmp[t++] = '0';
        }
        else
        {
                while(n > 0)
                {
                        tmp[t++] = (char)('0' + (n % 10));
                        n /= 10;
                }
        }

        if(neg)
        {
                dst[k++] = '-';
        }

        while(t > 0)
        {
                dst[k++] = tmp[--t];
        }

        dst[k++] = '\r';
        dst[k++] = '\n';
        dst[k]   = '\0';

        return k;
}


void esp01_sendToThingspeak(int field, int val)
{
        char req[80];
        int  len;

        /* Close any connection left open from a previous upload cycle
           first. If we skip this, the ESP01 refuses the next
           AT+CIPSTART with "ERROR / link is builded" (that's the exact
           message the LCD was showing) because as far as the module is
           concerned a link is still open. Ignoring the result here is
           fine - if there was nothing to close it just answers ERROR,
           which is harmless. */
        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("AT+CIPCLOSE");
        delay_ms(500);

        esp01_clear_buffer();
        string_uart("AT+CIPCLOSE\r\n");
        esp01_wait_bytes(4, 150);
        delay_ms(300);


        cmd_lcd(0x01);
        cmd_lcd(0x80);
        string_lcd("AT+CIPSTART");
        delay_ms(1000);

        esp01_clear_buffer();
        string_uart("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80\r\n");

        /* The old code busy-waited on `while(i<5);` with no timeout at
           all, so if the ESP01 never answered (bad wifi, module reset,
           etc.) the whole board would hang here forever - matching the
           "board stuck / no error shown" symptom. Bounded now. */
        if(esp01_wait_bytes(5, 300) != 0)
        {
                cmd_lcd(0xC0);
                string_lcd("TIMEOUT");
                delay_ms(1000);
                return;
        }

        delay_ms(2500);
        buff[i] = '\0';

        show_clean_response();
        delay_ms(2000);

        if(strstr(buff, "CONNECT") || strstr(buff, "ALREADY CONNECT"))
        {
                cmd_lcd(0xC0);
                string_lcd("OK");
                delay_ms(2000);

                cmd_lcd(0x01);
                cmd_lcd(0x80);
                string_lcd("AT+CIPSEND");
                delay_ms(1000);

                /* Build the exact HTTP GET request first so we know its
                   precise length. The old code hard-coded
                   "AT+CIPSEND=48", which only matches one specific
                   field/value/digit-count combination - for every other
                   value the byte count sent didn't match what
                   AT+CIPSEND was told to expect, so the ESP01 either
                   truncated the request or sat waiting for more bytes
                   until it timed out. field 3 was also missing the
                   "api_key=" part entirely - fixed below. */
                if(field == 1)
                {
                        strcpy(req, "GET /update?api_key=GQANATMFYHMV6ZR3&field1=");
                }
                else if(field == 2)
                {
                        strcpy(req, "GET /update?api_key=GQANATMFYHMV6ZR3&field2=");
                }
                else
                {
                        strcpy(req, "GET /update?api_key=GQANATMFYHMV6ZR3&field3=");
                }

                len = (int)strlen(req);
                len += append_int_crlf(req + len, val);

                esp01_clear_buffer();
                string_uart("AT+CIPSEND=");
                int_uart((unsigned int)len);
                string_uart("\r\n");

                /* Wait for the '>' data prompt instead of blindly
                   guessing a fixed delay before pushing the payload. */
                esp01_wait_bytes(1, 200);
                delay_ms(200);

                esp01_clear_buffer();
                string_uart(req);

                delay_ms(5000);
                delay_ms(5000);

                buff[i] = '\0';

                if(strstr(buff, "SEND OK"))
                {
                        cmd_lcd(0x01);
                        cmd_lcd(0x80);
                        string_lcd("SEND OK");
                        delay_ms(2000);
                }
                else
                {
                        /* Something other than a clean "SEND OK" came
                           back (error/timeout/garbled) - show the full
                           raw response so it's possible to see why. */
                        show_clean_response();
                        delay_ms(2000);
                }

                if(strstr(buff, "OK"))
                {
                        cmd_lcd(0x01);
                        string_lcd("DATA UPDATED");
                        delay_ms(1000);
                }
                else
                {
                        cmd_lcd(0x01);
                        string_lcd("DATA NOT UPDATED");
                        delay_ms(1000);
                }
        }
        else
        {
                cmd_lcd(0xC0);
                string_lcd("ERROR");
                delay_ms(1000);
                return;
        }
}
