/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ysarac <yunusemresarac@yaani.com>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/11/03 14:15:55 by ysarac            #+#    #+#             */
/*   Updated: 2023/11/04 19:07:14 by ysarac           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stdarg.h>
#include "stm32f1xx_hal.h"

int	ft_putstr(char *str, int i); // string (char *)
int	put_digit(long long int number, char *base, int bsize);// number
int	ft_formats(va_list args, const char formats);   // chechk function
int	put_digit_pointer(void *point, char *base, int bsize);// pointer
int	ft_printf_lcd(const char *format, ...); // ft_printf_lcd
void lcd_init (void);   // initialize lcd
void lcd_send_cmd (char cmd);  // send command to the lcd
int lcd_send_data (char data);  // send data to the lcd
void lcd_send_string (char *str);  // send string to the lcd
int lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);
void lcd_clear (void);
#endif
