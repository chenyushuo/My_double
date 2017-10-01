#include<stdio.h>
#include<string.h>
//这个double是使用IEEE754标准

typedef long long int64;
typedef unsigned long long uint64;

//===========================================================================
//常数定义

const char positive_inf[]  = "0111111111110000000000000000000000000000000000000000000000000000";
const char negative_inf[]  = "1111111111110000000000000000000000000000000000000000000000000000";
const char positive_nan[]  = "0111111111111000000000000000000000000000000000000000000000000000";
const char negative_nan[]  = "1111111111111000000000000000000000000000000000000000000000000000";
const char positive_zero[] = "0000000000000000000000000000000000000000000000000000000000000000";
const char negative_zero[] = "1000000000000000000000000000000000000000000000000000000000000000";

const char test_double[] = "0000000000001000000000000000000000000000000000000000000000000000";
/*

0.000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000000000000000
00000000000000000000000000000000000000000000000000000000000000000000011125369292
53600691545116358666202032109607990231165915276663708443602217406959097927141579
50625551028203366986551790550257621708077673005442800619268885941056538899676600
11652398050737212918180359607825234712518671041876254033253083290794743602455899
84295819824250317954385059152437399890443876874974725790225802525457699928291235
40932255676896790249605799054288302599621667605717619507439784980479564444580149
63207555317331566968317387932565146858810236628158907428321754360614143188210224
23405703806955738531400844926622055012080723710809283583075270077142542358376450
95158066138944836485368656166704349449158753391942346304638698898642932982747054
56845477030682337843511993391576453404923086054623126983642578125000000000000000

*/

#define double_tot_bit   (64)
#define double_k         (11)
#define double_bias      ((1 << (double_k - 1)) - 1)
#define double_n         (52)
#define double_tot_siz   (double_bias + double_n)

#define min(a,b) ((a)<(b) ? (a) : (b))
#define max(a,b) ((a)>(b) ? (a) : (b))

//===========================================================================
//以下是double所用的10进制高精度

char is_init_output = 0;

struct big_int{
	int len;
	char v[double_tot_siz];	
};//从0开始 && 正序位数升高

struct tni_gib{
	int len;
	char v[double_tot_siz];
};//从1开始 && 逆序位数升高

struct big_int power2[double_tot_siz] , res_int;
struct tni_gib power5[double_tot_siz] , res_dec;

#define big_int_carry \
if (a -> v[i] >= 10){ \
	a -> v[i + 1] += a -> v[i] / 10; \
	a -> v[i] %= 10; \
}

void big_int_add(struct big_int *a , struct big_int *b){
	int len = max(a -> len , b -> len);
	for (int i = 0 ; i < len ; i++){
		a -> v[i] += b -> v[i];
		big_int_carry;
	}
	if (a -> v[len]) len++;
	a -> len = len;
}
void big_int_shl(struct big_int *a){
	int len = a -> len;
	for (int i = 0 ; i < len ; i++)
		a -> v[i] <<= 1;
	for (int i = 0 ; i < len ; i++)
		big_int_carry;
	if (a -> v[len]) len++;
	a -> len = len;
}
int big_int_shr(struct big_int *a){
	int rest = 0;
	int len = a -> len;
	for (int i = len - 1 ; i >= 0 ; i--){
		a -> v[i] += rest * 10;
		rest = a -> v[i] & 1;
		a -> v[i] >>= 1;
	}
	if (a -> v[len - 1] == 0) len--;
	a -> len = len;
	return rest;
}

#define tni_gib_carry \
if (a -> v[i] >= 10){ \
	a -> v[i - 1] += a -> v[i] / 10; \
	a -> v[i] %= 10; \
}

void tni_gib_add(struct tni_gib *a , struct tni_gib *b){
	int len = max(a -> len , b -> len);
	for (int i = len ; i ; i--)
		a -> v[i] += b -> v[i];
	a -> len = len;
}
void tni_gib_mul(struct tni_gib *a){
	int len = a -> len;
	for (int i = len ; i ; i--)
		a -> v[i] *= 5;
	for (int i = len ; i ; i--)
		tni_gib_carry;
	len++;
	for (int i = len ; i ; i--)
		a -> v[i] = a -> v[i - 1];
	a -> v[0] = 0;
	a -> len = len;
}
int tni_gib_shl(struct tni_gib *a){
	int len = a -> len;
	for (int i = len ; i ; i--)
		a -> v[i] <<= 1;
	for (int i = len ; i ; i--)
		tni_gib_carry;
	if (a -> v[len] == 0) len--;
	int res = a -> v[0];
	a -> v[0] = 0;
	return res;
}

void init_output(){
	is_init_output = 1;
	//power2
	power2[0] . len = 1;
	memset(power2[0] . v , 0 , sizeof(power2[0] . v));
	power2[0] . v[0] = 1;
	for (int i = 1 ; i < double_tot_siz ; i++){
		power2[i] = power2[i - 1];
		big_int_shl( &power2[i] );
	}
	
	//power5
	power5[1] . len = 1;
	memset(power5[0] . v , 0 , sizeof(power5[0] . v));
	power5[1] . v[1] = 5;
	for (int i = 2 ; i < double_tot_siz ; i++){
		power5[i] = power5[i - 1];
		tni_gib_mul( &power5[i] );
	}
}
void clear_output(){
	if (is_init_output == 0)
		init_output();
	res_int . len = 1;
	memset(res_int . v , 0 , sizeof(res_int . v));
	res_dec . len = 1;
	memset(res_dec . v , 0 , sizeof(res_dec . v));
}
void main_output(){
	for (int i = res_int.len - 1 ; i >= 0 ; i--){
		putchar('0' + res_int . v[i]);
	}
	putchar('.');
	for (int i = 1 ; i <= res_dec.len ; i++){
		putchar('0' + res_dec . v[i]);
	}
}

void big_int_output(struct big_int *a){
	int len = a -> len;
	for (int i = len - 1 ; i >= 0 ; i--)
		putchar('0' + a -> v[i]);
	putchar('\n');
}
void tni_gib_output(struct tni_gib *a){
	int len = a -> len;
	for (int i = 1 ; i <= len ; i++)
		putchar('0' + a -> v[i]);
	putchar('\n');
}

//===========================================================================
//以下是double所用的2进制高精度

//===========================================================================
//以下是double

struct My_double{
	char digit[double_tot_bit >> 3];
};

void set_digit(struct My_double *a , int x , int v){
	a -> digit[x >> 3] &= (0xff ^ (1 << (x & 7)));
	a -> digit[x >> 3] |= (v << (x & 7));
}
void set_s(struct My_double *a , int s){
	set_digit(a , double_tot_bit - 1 , s);
}
void set_exp(struct My_double *a , int exp){
	for (int i = double_tot_bit - 2 ; i >= double_n ; i--){
		set_digit(a , i , (exp >> (i - double_n)) & 1);
	}
}
void set_frac(struct My_double *a , int64 frac){
	for (int i = double_n - 1 ; i >= 0 ; i--){
		set_digit(a , i , (frac >> i) & 1);
	}
}

int get_digit(const struct My_double *a , int x){
	return (a -> digit[x >> 3] >> (x & 7)) & 1;
}
int get_s(const struct My_double *a){
	return get_digit(a , double_tot_bit - 1);
}
int get_exp(const struct My_double *a){
	int res = 0;
	for (int i = double_tot_bit - 2 ; i >= double_n ; i--){
		res <<= 1;
		res |= get_digit(a , i);
	}
	return res;
}
int64 get_frac(const struct My_double *a){
	int64 res = 0;
	for (int i = double_n - 1 ; i >= 0 ; i--){
		res <<= 1;
		res |= get_digit(a , i);
	}
	return res;
}

void set_string(struct My_double *a , const char *str){
	for (int i = double_tot_bit - 1 ; i >= 0 ; i--){
		set_digit(a , i , str[double_tot_bit - i - 1] - '0');
	}
}
void get_string(const struct My_double *a){
	for (int i = double_tot_bit - 1 ; i >= 0 ; i--){
		putchar('0' + get_digit(a , i));
	}
}

void read(struct My_double *a){
	
}
void write(const struct My_double *a){
	int s = get_s(a);
	int exp = get_exp(a);
	int64 frac = get_frac(a);
	if (s) putchar('-');
	if (exp == (1 << double_k) - 1){
		if (frac) putchar('n'),putchar('a'),putchar('n');
		else putchar('i'),putchar('n'),putchar('f');
	}
	else{
		clear_output();
		int E;
		if (exp == 0){
			E = 1 - double_bias;
		}
		else{
			E = exp - double_bias;
			if (E >= 0) big_int_add(&res_int , &power2[E]);
			else tni_gib_add(&res_dec , &power5[-E]);
		}
		for (int i = 1 ; i <= double_n ; i++)
			if (get_digit(a , double_n - i)){
				if (E - i >= 0) big_int_add(&res_int , &power2[E - i]);
				else tni_gib_add(&res_dec , &power5[i - E]);
			}
		main_output();
	}

}
//位数与二的幂的对应关系
//1/0 x_1     x_2     x_3     ... x_52
//2^E 2^(E-1) 2^(E-2) 2^(E-3)     2^(E-52)

//===========================================================================
struct My_double a;
char s[70];

int main(){
	//scanf("%s",s);
	set_string(&a,test_double);
	get_string(&a);
	putchar('\n');
	write(&a);
	return 0;
}
