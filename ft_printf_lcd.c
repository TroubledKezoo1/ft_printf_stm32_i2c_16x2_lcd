#include <ft_printf_lcd.h>
#include <unistd.h>
extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly
#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

int	ft_putstr(char *str, int i)
{
	int	result;

	result = 0;
	if (!str)
		str = "(null)";
	while (str[i])
	{
		result = lcd_send_data(*str);
		if (result < 0)
			return (-1);
		i += result;
	}
	return (i);
}

int	put_digit(long long int number, char *base, int bsize)
{
	int	counter;
	int	tmp;

	counter = 0;
	if (number < 0 && bsize == 10)
	{
		if (lcd_send_data('-')< 0)
			return (-1);
		number *= -1;
		counter++;
	}
	if (number >= bsize)
	{
		tmp = put_digit(number / bsize, base, bsize);
		if (tmp < 0)
			return (-1);
		counter += tmp;
	}
	if (lcd_send_data(base[number % bsize]) < 0)
		return (-1);
	return (counter + 1);
}

int	put_digit_pointer(void *point, char *base, int bsize)
{
	int					counter;
	int					tmp;
	unsigned long int	number;

	number = (unsigned long int)point;
	counter = 0;
	if (number < 0 && bsize == 10)
	{
		if (lcd_send_data('-')< 0)
			return (-1);
		number *= -1;
		counter++;
	}
	if (number >= (unsigned long int)bsize)
	{
		tmp = put_digit_pointer((void *)(number / bsize), base, bsize);
		if (tmp < 0)
			return (-1);
		counter += tmp;
	}
	if (lcd_send_data(base[number % bsize]) < 0)
		return (-1);
	return (counter + 1);
}

int	ft_formats(va_list args, const char formats)
{
	if (formats == 'c')
		return (lcd_send_data((char){va_arg(args, int)}));
	else if (formats == 's')
		return (ft_putstr(va_arg(args, char *), 0));
	else if (formats == 'p')
	{
		if (ft_putstr("0x", 0) < 0)
			return (-1);
		return (2 + put_digit_pointer(va_arg(args, void *), "0123456789abcdef",
				16));
	}
	else if (formats == 'd' || formats == 'i')
		return (put_digit  (va_arg(args, int), "0123456789", 10));
	else if (formats == 'u')
		return (put_digit  (va_arg(args, unsigned int), "0123456789", 10));
	else if (formats == 'x')
		return (put_digit  (va_arg(args, unsigned int), "0123456789abcdef", 16));
	else if (formats == 'X')
		return (put_digit  (va_arg(args, unsigned int), "0123456789ABCDEF", 16));
	else if (formats == 'n')
		return (lcd_put_cur(va_arg(args, unsigned int), 0));
	else if (formats == '%')
		return (lcd_send_data('%'));
	return (0);
}

int	ft_printf_lcd(const char *format, ...)
{
	va_list	args;
	int		len;
	int		result;

	len = 0;
	lcd_init();
	va_start(args, format);
	while (*format)
	{
		if (*format == '%')
			result = ft_formats(args, *(++format));
		else
			result = lcd_send_data(*format);
		if (result < 0)
			return (-1);
		len += result;
		format++;
	}
	va_end(args);
	return (len);
}
void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

int lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	if (HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100) == HAL_ERROR)
		return (-1);
	return (1);
}

void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

int lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
    return (1);
}


void lcd_init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(5);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(1);  // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(10);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(10);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(1);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(1);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(1);
	HAL_Delay(1);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(1);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}

