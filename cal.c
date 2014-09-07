/* Copyright (c) 2008-2010 robert wilson
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>

#include <gmp.h>

int main(int argc, char **argv){
 mpz_t year, tmp1, tmp2, tmp3, tmp4;
 unsigned long day_of_week = 0;
 uintmax_t month_min = 1, month_max = 12;
 int_fast8_t month_days = 0;
 const int_fast8_t months_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
  30, 31 };
 time_t t = time(NULL);
 wchar_t weekday[13] = {0}, weekdays[23] = {0}, month_name[LINE_MAX] = {0},
  cal_head[LINE_MAX] = {0};
 struct tm *time_struct = localtime(&t);
 setlocale(LC_ALL, "");
 mpz_init(year);
 switch(argc){
  case 1:
   mpz_set_ui(year, time_struct->tm_year + 1900);
   month_min = month_max = time_struct->tm_mon + 1;
   break;
  case 2:
   mpz_set_str(year, argv[1], 10);
   break;
  default:
   month_min = month_max = strtoumax(argv[1], NULL, 10);
   mpz_set_str(year, argv[2], 10);
 }
 if(mpz_sgn(year) < 1 || !month_min || month_min > 12){
  fputs("http://opengroup.org/onlinepubs/9699919799/utilities/cal.html\n",
   stderr);
  exit(1);
 }
 for(int_fast8_t i = 0; i < 7; ++i){
  time_struct->tm_wday = i;
  wcsftime(weekday, 13, L"%a", time_struct);
  weekday[2] = 0;
  wcsncat(weekdays, weekday, 12);
  wcscat(weekdays, L" ");
 }
 wcscat(weekdays, L"\n");
 for(uintmax_t month = month_min; month <= month_max; ++month){
  time_struct->tm_mon = month - 1;
  wcsftime(month_name, LINE_MAX, L"%OB", time_struct);
  if(month > month_min) putchar('\n');
  for(int_fast8_t i = (20 - swprintf(cal_head, LINE_MAX, L"%ls %s\n", month_name, mpz_get_str(NULL, 10, year))) / 2; i > 0; --i)
   putchar(' ');
  fputws(cal_head, stdout);
  fputws(weekdays, stdout);
  if(!mpz_cmp_ui(year, 1752) && month == 9){
   puts("       1  2 14 15 16");
   puts("17 18 19 20 21 22 23");
   puts("24 25 26 27 28 29 30");
  }else{
   month_days = months_days[month - 1];
   mpz_init(tmp1);
   mpz_init(tmp2);
   mpz_init(tmp3);
   mpz_init(tmp4);
   if(mpz_cmp_ui(year, 1752) > 0 || (!mpz_cmp_ui(year, 1752) && month > 9)){
    /* Gregorian */
    if(month == 2 && mpz_divisible_ui_p(year, 4) && (!mpz_divisible_ui_p(year,
     100) || mpz_divisible_ui_p(year, 400))) month_days = 29;
    mpz_add_ui(tmp4, year, 4800 - (14 - month) / 12);
    mpz_tdiv_q_ui(tmp1, tmp4, 4);
    mpz_tdiv_q_ui(tmp2, tmp4, 100);
    mpz_tdiv_q_ui(tmp3, tmp4, 400);
    mpz_mul_ui(tmp4, tmp4, 365);
    mpz_add(tmp4, tmp4, tmp1);
    mpz_sub(tmp4, tmp4, tmp2);
    mpz_add(tmp4, tmp4, tmp3);
    mpz_add_ui(tmp4, tmp4, (153 * (month + 12 * ((14 - month) / 12) - 3) + 2) /
     5);
    mpz_sub_ui(tmp4, tmp4, 32043);
    day_of_week = mpz_tdiv_ui(tmp4, 7);
   }else{
    /* Julian */
    if(month == 2 && mpz_divisible_ui_p(year, 4))
     month_days = 29;
    mpz_add_ui(tmp2, year, 4800 - (14 - month) / 12);
    mpz_tdiv_q_ui(tmp1, tmp2, 4);
    mpz_mul_ui(tmp2, tmp2, 365);
    mpz_add(tmp2, tmp2, tmp1);
    mpz_add_ui(tmp2, tmp2, (153 * (month + 12 * ((14 - month) / 12) - 3) + 2) /
     5);
    mpz_sub_ui(tmp2, tmp2, 32081);
    day_of_week = mpz_tdiv_ui(tmp2, 7);
   }
   mpz_clear(tmp1);
   mpz_clear(tmp2);
   mpz_clear(tmp3);
   mpz_clear(tmp4);
   for(uint_fast8_t i = 0; i < day_of_week; ++i) fputs("   ", stdout);
   for(int_fast8_t i = 1; i <= month_days; ++i){
    printf("%2u", i);
    ++day_of_week;
    if(i < month_days)
     putchar((day_of_week %= 7) ? ' ' : '\n');
   }
  }
  putchar('\n');
 }
 mpz_clear(year);
 return 0;
}
