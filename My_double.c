#include<stdio.h>
#include<string.h>

typedef long long int64;
typedef unsigned long long uint64;

const char positive_inf[]  = "0111111111110000000000000000000000000000000000000000000000000000";
const char negative_inf[]  = "1111111111110000000000000000000000000000000000000000000000000000";
const char positive_nan[]  = "0111111111111000000000000000000000000000000000000000000000000000";
const char negative_nan[]  = "1111111111111000000000000000000000000000000000000000000000000000";
const char positive_zero[] = "0000000000000000000000000000000000000000000000000000000000000000";
const char negative_zero[] = "1000000000000000000000000000000000000000000000000000000000000000";

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
	power5[0] . len = 1;
	memset(power5[0] . v , 0 , sizeof(power5[0] . v));
	power5[0] . v[1] = 1;
	for (int i = 1 ; i < double_tot_siz ; i++){
		power5[i] = power5[i - 1];
		tni_gib_mul( &power5[i] );
	}
}
void clear_output(){
	if (is_init_output == 0)
		init_output();
	memset(res_int . v , 0 , sizeof(res_int . v));
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

//===========================================================================
//以下是double所用的2进制高精度

//===========================================================================
//以下是double

struct My_double{
	char digit[double_tot_bit >> 3];
};

int get_digit(const struct My_double *a , int x){
	return (a -> digit[x >> 3] >> (x & 7)) & 1;
}
int get_s(const struct My_double *a , int s){
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

void read(struct My_double *a){
	
	//printf("%d\n",x -> digit[0]);
}
void write(const struct My_double *a){

}


int main(){
	
	return 0;
}
