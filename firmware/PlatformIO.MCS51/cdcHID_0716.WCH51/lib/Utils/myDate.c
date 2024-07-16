/*         
 * myDate.c
 *
 *         01234567890                  
 *    Date Jul 11 2024  
 * 
 * */
#include "myDate.h"

#ifdef _TITLE_STAMP_
unsigned char _month, _day, _year;
__code const char *months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

void getMyDate(void)
{
  char temp [] = __DATE__;
  unsigned char i;

  _year = atoi(temp + 9);
  *(temp + 6) = 0;
  _day = atoi(temp + 4);
  *(temp + 3) = 0;
  for (i = 0; i < 12; i++) {    
    if (!strcmp(temp, months[i])) {
      _month = i + 1;
      return;
      }
    }
}
#endif