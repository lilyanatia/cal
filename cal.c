// Copyright (C) 2008-2021 Lily Anatia Wilson <hotaru@thinkindifferent.net>
// This file is part of cal.
//
// cal is non-violent software: you can use, redistribute,
// and/or modify it under the terms of the CNPLv6 as found
// in the LICENSE file in the source code root directory or
// at <https://git.pixie.town/thufie/CNPL>.
//
// cal comes with ABSOLUTELY NO WARRANTY, to the extent
// permitted by applicable law.  See the CNPL for details.

#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <sys/param.h>

#include <gmp.h>

int main(int argc, char **argv){
 mpz_t year, tmp1, tmp2, tmp3, tmp4;
 unsigned long day_of_week = 0;
 uintmax_t month_min = 1, month_max = 12;
 int_fast8_t month_days = 0;
 const int_fast8_t months_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,
  30, 31 };
 time_t t = time(NULL);
 char weekday[13] = {0}, weekdays[92] = {0}, month_name[13] = {0};
 wchar_t w_weekday[3] = {0}, w_month_name[13] = {0};
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
  strftime(weekday, 13, "%a", time_struct);
  mbstowcs(w_weekday, weekday, 2);
  wcstombs(weekday, w_weekday, 13);
  strcat(weekdays, weekday);
  strcat(weekdays, " ");
 }
 strcat(weekdays, "\n");
 for(uintmax_t month = month_min; month <= month_max; ++month){
  time_struct->tm_mon = month - 1;
  strftime(month_name, 13, "%B", time_struct);
  mbstowcs(w_month_name, month_name, 13);
  if(month > month_min) putchar('\n');
  for(int_fast8_t i = MAX(0, 20 - wcslen(w_month_name) - strlen(mpz_get_str(NULL, 10, year))) / 2; i > 0; --i)
   putchar(' ');
  fputs(month_name, stdout);
  putchar(' ');
  printf("%s\n", mpz_get_str(NULL, 10, year));
  fputs(weekdays, stdout);
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
     fputs((day_of_week %= 7) ? " " : "\n", stdout);
   }
  }
  putchar('\n');
 }
 mpz_clear(year);
 return 0;
}
