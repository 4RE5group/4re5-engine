#include "engine.h"


/*
	This function takes a char array and convert it to double.
	Ex: '-123.456     hello world!' => -123.456 + increment variable pointed by 8
*/
double	parseDouble(const char *str, int *to_increment)
{
	int sign = 1;
	int	j = 0;
	if (str[j] == '-') // handle negative numbers
	{
		sign = -1;
		j++;
	}
	// check if next is a number otherwise error
	if (str[j] < '0' || str[j] > '9') {
		__builtin_printf("error: invalid str in parseDouble\n");
		return (0.0);
	}
	// process int part
	double	num = 0.0f;
	while (str[j] >= '0' && str[j] <= '9')
		num = 10 * num + (str[j++] - '0');
	if (str[j] == '.')
	{
		// process decimal part
		j++;
		float		negative_exp_of_ten = 0.1;
		while (str[j] >= '0' && str[j] <= '9' && negative_exp_of_ten >= 0.0001)
		{
			num = num + (str[j++] - '0') * negative_exp_of_ten;
			negative_exp_of_ten *= 0.1;
		}
	}
	// increment pointed var
	*to_increment += j;
	return (num * sign);
}


/*
	This function takes a char array and convert it to integer.
	Simple atoi.
*/
int		parseInt(const char *str, int *to_increment)
{
	int sign = 1;
	int	j = 0;
	if (str[j] == '-') // handle negative numbers
	{
		sign = -1;
		j++;
	}
	// check if next is a number otherwise error
	if (str[j] < '0' || str[j] > '9') {
		__builtin_printf("error: invalid str in parseInt\n");
		return (0);
	}
	// process int part
	long	num = 0;
	while (str[j] >= '0' && str[j] <= '9' && num < 2147483648)
		num = 10 * num + (str[j++] - '0');
	
	// increment pointed var
	*to_increment += j;
	return (num * sign);
}