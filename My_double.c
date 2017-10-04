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

#define double_tot_bit   (64)
#define double_k         (11)
#define double_bias      ((1 << (double_k - 1)) - 1)
#define double_n         (52)
#define double_tot_siz   (double_bias + double_n)

//小函数
#define min(a,b) ((a)<(b) ? (a) : (b))
#define max(a,b) ((a)>(b) ? (a) : (b))

void swap(char *a , char *b){
	char t = *a;
	*a = *b;
	*b = t;
}

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

void clear_res(){
	res_int . len = 1;
	memset(res_int . v , 0 , sizeof(res_int . v));
	res_dec . len = 1;
	memset(res_dec . v , 0 , sizeof(res_dec . v));
}

char big_int_is_zero(const struct big_int *a){
	return a -> len == 1 && a -> v[0] == 0;
}
char tni_gib_is_zero(const struct tni_gib *a){
	return a -> len == 1 && a -> v[1] == 0;
}

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
	if (len > 1 && a -> v[len - 1] == 0) len--;
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
	for (int i = len ; i ; i--)
		tni_gib_carry;
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
	if (len > 1 && a -> v[len] == 0) len--;
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
		big_int_shl(&power2[i]);
	}
	
	//power5
	power5[1] . len = 1;
	memset(power5[0] . v , 0 , sizeof(power5[0] . v));
	power5[1] . v[1] = 5;
	for (int i = 2 ; i < double_tot_siz ; i++){
		power5[i] = power5[i - 1];
		tni_gib_mul(&power5[i]);
	}
}
void clear_output(){
	if (is_init_output == 0)
		init_output();
	clear_res();
}
void main_output(){
	for (int i = res_int.len - 1 ; i >= 0 ; i--){
		putchar('0' + res_int . v[i]);
	}
	putchar('.');
	for (int i = 1 ; i <= res_dec.len ; i++){
		putchar('0' + res_dec . v[i]);
	}
	for (int i = res_dec.len+1;i<=1200;i++) putchar('0');
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
struct big_bin{
	int len;
	char v[double_tot_siz << 1];
};//从0开始，正序位数升高

struct big_bin binary_1 , binary_2 , binary_res;

void big_bin_clear(struct big_bin *a){
	a -> len = 1;
	memset(a -> v , 0 , sizeof(a -> v));
}
void big_bin_reverse(struct big_bin *a){
	int len = a -> len;
	for (int i = 0 , j = len - 1 ; i < j ; i++ , j--)
		swap(a -> v + i , a -> v + j);
}

char big_bin_from_x_is_zero(struct big_bin *a , int x){//判断 x 位后面是否全为0
	char flag = 1;
	for (int i = x ; i >= 0 ; i--)
		if (a -> v[i])
			flag = 0;
	return flag;
}
char big_bin_is_zero(struct big_bin *a){
	return a -> len == 1 && a -> v[0] == 0;
}

void big_bin_input(struct big_bin *a , const char *str){
	int len = 0;
	big_bin_clear(a);
	for (int i = 0 ; str[i] ; i++)
		a -> v[len++] = str[i] - '0';
	big_bin_reverse(a);
	while (a -> v[len - 1] == 0) len--;
	a -> len = len;
}
void big_bin_output(const struct big_bin *a){
	int len = a -> len;
	for (int i = len - 1 ; i >= 0 ; i--)
		putchar('0' + a -> v[i]);
	putchar('\n');
}

void big_bin_inc(struct big_bin *a , int x){//单独的一位+1
	int len = max(a -> len , x + 1);
	a -> v[x] ++;
	for (int i = x ; i < len ; i++){
		if (a -> v[i] == 2){
			a -> v[i] = 0;
			a -> v[i + 1] ++;
		}
		else break;
	}
	if (a -> v[len]) len++;
	a -> len = len;
	//for (int i=len-1;i >= 0;i--) printf("%d",a->v[i]);putchar('\n');
}
void big_bin_complement(struct big_bin *a){//求补码
	int len = a -> len;
	for (int i = 0 ; i < len ; i++)
		a -> v[i] ^= 1;
	big_bin_inc(a , 0);
	if (a -> len == len + 1){
		a -> v[len] = 0;
		a -> len = len;
	}
}
void big_bin_add(const struct big_bin *a , const struct big_bin *b , struct big_bin *res){//带补码的加法
	int len_a = a -> len;
	int len_b = b -> len;
	int len = max(len_a , len_b);
	*res = *a;
	for (int i = 0 ; i < len_b ; i++)
		if (b -> v[i]){
			big_bin_inc(res , i);
		}
	if (res -> v[len])
		res -> v[len] = 0;
	//while (res -> v[len - 1] == 0) len--;
	res -> len = len;
}
void big_bin_mul(const struct big_bin *a , const struct big_bin *b , struct big_bin *res){//不带补码的乘法
	//big_bin_output(a);
	//big_bin_output(b);
	int len_a = a -> len;
	int len_b = b -> len;
	int len = len_a + len_b - 1;
	big_bin_clear(res);
	for (int i = 0 ; i < len_a ; i++)
		if (a -> v[i]){
			for (int j = 0 ; j < len_b ; j++)
				if (b -> v[j])
					big_bin_inc(res , i + j);
		}
	if (res -> v[len]) len++;
	res -> len = len;
}

void big_bin_shl(struct big_bin *a){
	int len = a -> len;
	for (int i = len - 1 ; i ; i--)
		a -> v[i] = a -> v[i - 1];
	a -> v[0] = 0;
	a -> len = ++len;
}
void big_bin_shr(struct big_bin *a){
	int len = a -> len;
	for (int i = 0 ; i < len - 1 ; i++)
		a -> v[i] = a -> v[i + 1];
}
void swap_big_bin(struct big_bin **a , struct big_bin **b){
	struct big_bin *t;
	t = *a;
	*a = *b;
	*b = t;
}
void big_bin_div(struct big_bin *a , struct big_bin *b , struct big_bin *res){
	static struct big_bin aa;
	static struct big_bin bb;
	static struct big_bin t;
	static struct big_bin *tmp;
	tmp = &t;
	big_bin_clear(tmp);
	
	aa = *a , bb = *b;
	a = &aa , b = &bb;
	int len_a = a -> len;
	int len_b = b -> len;
	int len_res = 1;//最开始尾部对齐的时候肯定有一位
	
	while (len_a > len_b){
		big_bin_shl(b);
		len_b++;
		len_res++;
	}
	b -> len = len_b;
	
	//printf("a : "),big_bin_output(a);
	//printf("b : "),big_bin_output(b);
	
	big_bin_complement(b);
	//printf("start ! : "),big_bin_output(b);
	for (int i = len_res - 1 ; i ; i--){
		big_bin_add(a , b , tmp);
		//printf("tmp : "),big_bin_output(tmp);
		if (tmp -> v[len_a - 1])
			res -> v[i] = 0;
		else{
			res -> v[i] = 1;
			swap_big_bin(&a , &tmp);
			//printf("a : "),big_bin_output(a);
		}
		big_bin_shr(b);	
		//printf("b : "),	big_bin_output(b);
	}
	//0位应该表示是否有余数
	res -> v[0] = big_bin_from_x_is_zero(a , len_a - 1) ^ 1;
	res -> len = len_res;
	
	//printf("res -> len = %d\n",len_res);
}

void big_bin_rounding(struct big_bin *a){//浮点数的核心
	//printf("big_bin_rounding : ");
	//big_bin_output(a);
	int len = a -> len;
	if (len <= double_n) return;
	int key_point = len - double_n - 1;
	if (a -> v[key_point]){//舍去的位是1
		if (big_bin_from_x_is_zero(a , key_point - 1)){//全为0
			if (a -> v[key_point + 1]){
				big_bin_inc(a , key_point + 1);
			}
		}
		else{//不全为0
			big_bin_inc(a , key_point + 1);
		}
	}
	for (int i = key_point ; i >= 0 ; i--)
		a -> v[i] = 0;
}

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
	printf("digit of double : ");
	for (int i = double_tot_bit - 1 ; i >= 0 ; i--){
		putchar('0' + get_digit(a , i));
	}
	putchar('\n');
}

//double 的初始化
void My_double_clear(struct My_double *a){
	memset(a -> digit , 0 , sizeof(a -> digit));
}
void overflow(struct My_double *a){
	//printf("overflow!\n");
	if (get_s(a)) set_string(a , negative_inf);
	else set_string(a , positive_inf);
}
void invalid_number(struct My_double *a){
	if (get_s(a)) set_string(a , negative_nan);
	else set_string(a , positive_nan);
}
void set_zero(struct My_double *a){
	if (get_s(a)) set_string(a , negative_zero);
	else set_string(a , positive_zero);
}
void underflow(struct My_double *a){
	//printf("underflow!\n");
	set_zero(a);
}

//这个过程必须保证binary的首位为1
//所以首先就要去掉首位的0
//如果那个数是normalized的话就要删去首位的1
//如果那个数是denormalized的话就不需要删去首位的1
//如果那个数过大就设为inf，也就是overflow
//如果那个数过小就设为0，也是underflow 这个可能在denormalized的数中发生
void get_from_rounding(struct My_double *a , struct big_bin *binary , int original_E){
	//printf("start rounding\n");
	//big_bin_output(binary);	
	
	int len = binary -> len;
	int new_E = original_E + len - 1;
	int exp = 0;
	
	while (len > 1 && binary -> v[len - 1] == 0){
		new_E--;
		len--;
	}
	binary -> len = len;
	
	//printf("new_E = %d len = %d\n",new_E,len);
	
	if (big_bin_is_zero(binary)){//0
		set_zero(a);
		return;
	}
	else if (new_E > double_bias){//overflow
		overflow(a);
		return;
	}
	if (new_E >= 1 - double_bias){//normalized
		exp = new_E + double_bias;
		binary -> v[--len] = 0;
		binary -> len = len;
	}
	else if (new_E >= -double_bias - double_n){//denormalized (maybe underflow)
		for (int i = new_E + 1 ; i < 1 - double_bias ; i++)
			binary -> v[len++] = 0;
		binary -> len = len;
		new_E = 1 - double_bias;
		exp = 0;
	}
	
	big_bin_rounding(binary);
	
	//printf("complete rounding\n");
	//big_bin_output(binary);
	
	if (exp == 0 && big_bin_from_x_is_zero(binary , binary -> len - 1)){//underflow
		underflow(a);
		return;
	}
	
	for (int i = 0 , j = double_n ; i < double_k ; i++ , j++)//设置exp
		set_digit(a , j , (exp >> i) & 1);
		
	for (int i = len - 1 , j = double_n - 1 ; i >= 0 || j >= 0 ; i-- , j--)//设置frac
		if (i < 0)
			set_digit(a , j , 0);
		else
			set_digit(a , j , binary -> v[i]);
	
}

void read(struct My_double *a){
	static char str[double_tot_siz];
	scanf("%s",str);
	My_double_clear(a);
	int str_len = 0;
	for (int i = 0 ; str[i] ; i++)
		str_len++;
	
	static char M[double_tot_siz];
	memset(M , 0 , sizeof(M));
	int start_M = -1;
	int end_M = -1;
	if (str[0] == '+' || str[0] == '-'){
		start_M = 1;
		if (str[0] == '-')
			set_s(a , 1);
	}
	else start_M = 0;
	for (int i = start_M ; i < str_len ; i++)
		if (!('0' <= str[i + 1] && str[i + 1] <= '9') && str[i + 1] != '.'){
			end_M = i;
			break;
		}
		
	int point = -1;
	for (int i = start_M ; i <= end_M ; i++)
		if (str[i] == '.'){
			point = i - start_M;
			break;
		}
	
	int M_len = 0;
	for (int i = start_M ; i <= end_M ; i++){
		if (str[i] == '.') continue;
		M[M_len++] = str[i] - '0';
	}
	if (point == -1)
		point = M_len;
	//如此的话，M[0,point)是整数，剩下的为小数
	
	int start_dec_E = -1;
	char is_negative_dec_E = 0;
	for (int i = 0 ; i < str_len ; i++)
		if (str[i] == 'E' || str[i] == 'e'){
			if (str[i + 1] == '+' || str[i + 1] == '-'){
				start_dec_E = i + 2;
				if (str[i + 1] == '-')
					is_negative_dec_E = 1;
			}
			else start_dec_E = i + 1;
			break;
		}
	
	int dec_E = 0;
	if (start_dec_E != -1){
		for (int i = start_dec_E ; i < str_len ; i++)
			dec_E = dec_E * 10 + str[i] - '0';
		if (is_negative_dec_E)
			dec_E = -dec_E;
	}
	
	point += dec_E;
	M_len = max(M_len , point);
	int len_int = point;
	int len_dec = M_len - len_int;
	clear_res();
	//for (int i=0;i<M_len;i++) printf("%d ",M[i]);putchar('\n');
	for (int i = 0 ; i < len_int ; i++){
		res_int . v[i] = M[len_int - i - 1];
	}
	res_int . len = max(len_int , 1);//res_int是big_int，而且是从0开始 && 正序位数升高
	for (int i = len_int ; i < M_len ; i++){
		res_dec . v[i - len_int + 1] = M[i];
	}
	res_dec . len = max(len_dec , 1);//res_dec是tni_gib，而且是从1开始 && 逆序位数升高
	
	//一定要删去前导零，不然后面在取位数的时候会把前导零算进去，然后就会出bug
	len_int = max(len_int , 1);
	len_dec = max(len_dec , 1);
	while (len_int > 1 && res_int . v[len_int - 1] == 0) len_int--;
	res_int . len = len_int;
	while (len_dec > 1 && res_dec . v[len_dec] == 0) len_dec--;
	res_dec . len = len_dec;
	
	//printf("res_int = %d res_dec = %d\n",len_int,len_dec);
	//big_int_output(&res_int);
	//tni_gib_output(&res_dec);
	
	big_bin_clear(&binary_1);
	int len_1 = 0;
	while (!big_int_is_zero(&res_int)){
		//printf("%d\n",len_1);
		//printf("aaaa "),big_int_output(&res_int);
		if (len_1 == double_bias + 1){
			overflow(a);
			return;
		}
		binary_1 . v[len_1++] = big_int_shr(&res_int);
		//printf("digit = %d\n",binary_1 . v[len_1 - 1]);
	}
	len_1 = max(len_1 , 1);
	binary_1 . len = len_1;
	//big_bin_output(&binary_1);
	
	big_bin_clear(&binary_2);
	int len_2 = 0;
	while (!tni_gib_is_zero(&res_dec)){
		if (len_2 == double_bias + 1){
			binary_2 . v[len_2++] = 1;
			break;
		}
		binary_2 . v[len_2++] = tni_gib_shl(&res_dec);
	}
	len_2 = max(len_2 , 1);
	binary_2 . len = len_2;
	big_bin_reverse(&binary_2);
	//big_bin_output(&binary_2);
	
	//binary_res最开始所要表达的数形式为 0.xxxxxx * 2^len_1 次方
	//而后变成xxxx * 2 ^ ? 次方
	big_bin_clear(&binary_res);
	int len_res = 0;
	int n = double_n + 1;
	int bin_E = len_1;
	if (len_1 >= n){//整数位数足够多
		if (len_1 > n){//整数位数太多
			if (big_bin_from_x_is_zero(&binary_1 , len_1 - n - 2)){//xxxx1000 xxxxxx的情况
				binary_res . v[len_res++] = big_bin_is_zero(&binary_2) ^ 1;
			}
			else{//xxxx1xxx xxxx的情况
				binary_res . v[len_res++] = 1;
			}
			binary_res . v[len_res++] = binary_1 . v[len_1 - n - 1];
		}
		else{//整数位数刚好
			binary_res . v[len_res++] = big_bin_from_x_is_zero(&binary_2 , len_2 - 2) ^ 1;
			binary_res . v[len_res++] = binary_2 . v[len_2 - 1];
		}
		for (int i = len_1 - n; i < len_1 ; i++)
			binary_res . v[len_res++] = binary_1 . v[i];
		bin_E -= n + 2;
	}
	else{//整数位数不够
		if (big_bin_is_zero(&binary_1)){//这个是0.xxxxx 可能是denormalized的数
			if (big_bin_is_zero(&binary_2)){
				if (get_s(a)) set_string(a , negative_zero);
				else set_string(a , positive_zero);
				return;
			}
			int start = len_2 - 1;
			for (int i = len_2 - 1 ; i >= 0 ; i--)
				if (binary_2 . v[i]){
					start = i;
					break;
				}
			bin_E = 0 - (n + 2 + (len_2 - start - 1));
			if (start + 1 <= n){//总位数不够 或 刚好
				binary_res . v[len_res++] = 0;
				binary_res . v[len_res++] = 0;
				for (int i = 0 ; i < n - (start + 1) ; i++)
					binary_res . v[len_res++] = 0;
				for (int i = 0 ; i <= start ; i++)
					binary_res . v[len_res++] = binary_2 . v[i];
			}
			else{//总位数足够
				binary_res . v[len_res++] = big_bin_from_x_is_zero(&binary_2 , start - n - 1) ^ 1;
				binary_res . v[len_res++] = binary_2 . v[start - n];
				for (int i = start - n + 1; i <= start  ; i++)
					binary_res . v[len_res++] = binary_2 . v[i];
			}
		}
		else{
			if (len_1 + len_2 <= n){//总位数不够 或 刚好
				binary_res . v[len_res++] = 0;
				binary_res . v[len_res++] = 0;
				for (int i = 0 ; i < n - len_1 - len_2 ; i++)
					binary_res . v[len_res++] = 0;
				for (int i = 0 ; i < len_2 ; i++)
					binary_res . v[len_res++] = binary_2 . v[i];
			}
			else{//总位数足够
				binary_res . v[len_res++] = big_bin_from_x_is_zero(&binary_2 , len_2 - (n - len_1) - 2) ^ 1;
				binary_res . v[len_res++] = binary_2 . v[len_2 - (n - len_1) - 1];
				for (int i = len_2 - (n - len_1) ; i < len_2 ; i++)
					binary_res . v[len_res++] = binary_2 . v[i];
			}
			for (int i = 0 ; i < len_1 ; i++)
				binary_res . v[len_res++] = binary_1 . v[i];
			bin_E -= n + 2;
		}
	}
	binary_res . len = len_res;
	get_from_rounding(a , &binary_res , bin_E);
}
void write(const struct My_double *a){
	int s = get_s(a);
	int exp = get_exp(a);
	int64 frac = get_frac(a);
	if (s) putchar('-');
	if (exp == (1 << double_k) - 1){
		if (frac) printf("nan");
		else printf("inf");
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
	putchar('\n');
}
//位数与二的幂的对应关系
//1/0 x_1     x_2     x_3     ... x_52
//2^E 2^(E-1) 2^(E-2) 2^(E-3)     2^(E-52)

//将My_double转化为binary，而且binary要位移delta位
//并且会在开头加两个0（因为要用补码来代替，不能让两个正数加的爆到负数）
void from_My_double_to_big_bin(const struct My_double *a , struct big_bin *binary , const int delta){
	big_bin_clear(binary);
	int len_bin = 0;
	for (int i = 0 ; i < delta ; i++)
		binary -> v[len_bin++] = 0;
	for (int i = 0 ; i < double_n ; i++)
		binary -> v[len_bin++] = get_digit(a , i);
	int exp = get_exp(a);
	if (exp)//normalized
		binary -> v[len_bin++] = 1;
	else
		binary -> v[len_bin++] = 0;
	binary -> v[len_bin++] = 0;
	binary -> v[len_bin++] = 0;
	binary -> len = len_bin;
	
	//printf("from_My_double_to_big_bin : ");
	//big_bin_output(binary);
}

void get_exp_and_E(const struct My_double *a , int *exp , int *E){
	*exp = get_exp(a);
	if (*exp)
		*E = *exp - double_bias;
	else
		*E = 1 - double_bias;
}

char is_nan(const struct My_double *a){
	if (get_exp(a) == (1 << double_k) - 1 && get_frac(a) != 0)
		return 1;
	return 0;
}
char is_inf(const struct My_double *a){
	if (get_exp(a) == (1 << double_k) - 1 && get_frac(a) == 0)
		return 1;
	return 0;
}
char is_zero(const struct My_double *a){
	if (get_exp(a) == 0 && get_frac(a) == 0)
		return 1;
	return 0;
}

//|a|>|b| => 1  |a|=|b| => 0  |a|<|b| => -1
char abs_compare(const struct My_double *a , const struct My_double *b){
	for (int i = double_tot_bit - 2 ; i >= 0 ; i--){
		int digit_a = get_digit(a , i);
		int digit_b = get_digit(b , i);
		if (digit_a != digit_b)
			return digit_a > digit_b ? 1 : -1;
	}	
	return 0;
}

void plus(const struct My_double *a , const struct My_double *b , struct My_double *c){
	My_double_clear(c);
	//这里有一些特殊情况
	if (is_nan(a) || is_nan(b)){
		invalid_number(c);
		return;
	}
	if (is_inf(a) || is_inf(b)){
		if (is_inf(a) && is_inf(b) && get_s(a) != get_s(b)){
			set_s(c , 1);
			invalid_number(c);
		}
		else{
			if (is_inf(a))
				set_s(c , get_s(a));
			else
				set_s(c , get_s(b));
			overflow(c);
		}
		return;
	}
	//一般情况
	int exp_a , E_a;
	int exp_b , E_b;
	get_exp_and_E(a , &exp_a , &E_a);
	get_exp_and_E(b , &exp_b , &E_b);
	int E_c = min(E_a , E_b);
	//printf("E_a = %d E_b = %d E_c = %d\n" , E_a , E_b , E_c);
	from_My_double_to_big_bin(a , &binary_1 , E_a - E_c);
	from_My_double_to_big_bin(b , &binary_2 , E_b - E_c);
	
	//这一段是将两个二进制数的位数调为一样，这样方便用补码来进行加减法
	int len_1 = binary_1 . len;
	int len_2 = binary_2 . len;
	len_1 = max(len_1 , len_2);
	len_2 = max(len_1 , len_2);
	binary_1 . len = len_1;
	binary_2 . len = len_2;
	
	if (get_s(a)) big_bin_complement(&binary_1);
	if (get_s(b)) big_bin_complement(&binary_2);
	//printf("binary_1 : "),big_bin_output(&binary_1);
	//printf("binary_2 : "),big_bin_output(&binary_2);
	big_bin_add(&binary_1 , &binary_2 , &binary_res);
	//printf("binary_res : "),big_bin_output(&binary_res);
	int len_res = binary_res . len;
	if (binary_res . v[len_res - 1]){
		set_s(c , 1);
		big_bin_complement(&binary_res);
	}
	else{
		set_s(c , 0);
	}
	get_from_rounding(c , &binary_res , E_c - double_n);
}
void minus(const struct My_double *a , struct My_double *b , struct My_double *c){
	int s = get_s(b);
	set_s(b , s ^ 1);
	plus(a , b , c);	
	set_s(b , s);
}
void mul(const struct My_double *a , const struct My_double *b , struct My_double *c){
	My_double_clear(c);
	set_s(c , get_s(a) ^ get_s(b));
	//这里有一些特殊情况
	if (is_nan(a) || is_nan(b)){
		invalid_number(c);
		return;
	}
	if (is_inf(a) || is_inf(b)){
		if (is_zero(a) || is_zero(b))
			invalid_number(c);
		else
			overflow(c);
		return;
	}
	//一般情况
	int exp_a , E_a;
	int exp_b , E_b;
	get_exp_and_E(a , &exp_a , &E_a);
	get_exp_and_E(b , &exp_b , &E_b);
	int E_c = E_a + E_b;
	from_My_double_to_big_bin(a , &binary_1 , 0);
	from_My_double_to_big_bin(b , &binary_2 , 0);
	big_bin_mul(&binary_1 , &binary_2 , &binary_res);
	//printf("res : "),big_bin_output(&binary_res);
	//big_bin_output(&binary_res);
	//printf("E_a = %d E_b = %d\n" , E_a , E_b);
	get_from_rounding(c , &binary_res , E_c - 2 * double_n);
	//get_string(a);
	//get_string(b);
	//get_string(c);
}
void div(const struct My_double *a , const struct My_double *b , struct My_double *c){
	My_double_clear(c);
	set_s(c , get_s(a) ^ get_s(b));
	//这里有一些特殊情况
	if (is_nan(a) || is_nan(b)){
		invalid_number(c);
		return;
	}
	if (is_inf(a)){
		if (is_inf(b)){
			set_s(c , 1);
			invalid_number(c);
		}
		else
			overflow(c);
		return;
	}
	else if (is_inf(b)){
		set_zero(c);
		return;
	}
	if (is_zero(a)){
		if (is_zero(b)){
			set_s(c , 1);
			invalid_number(c);
		}
		else
			set_zero(c);
		return;
	}
	else if (is_zero(b)){
		overflow(c);
		return;
	}
	//一般情况
	int exp_a , E_a;
	int exp_b , E_b;
	get_exp_and_E(a , &exp_a , &E_a);
	get_exp_and_E(b , &exp_b , &E_b);
	int E_c = (E_a - double_n * 2 - 1) - E_b;
	from_My_double_to_big_bin(a , &binary_1 , double_n * 2 + 1);
	from_My_double_to_big_bin(b , &binary_2 , 0 );
	//printf("binary_1 : "),big_bin_output(&binary_1);
	//printf("binary_2 : "),big_bin_output(&binary_2);
	big_bin_div(&binary_1 , &binary_2 , &binary_res);
	//printf("binary_res : "),big_bin_output(&binary_res);
	//printf("E_a = %d E_b = %d E_c = %d\n",E_a,E_b,E_c);
	get_from_rounding(c , &binary_res , E_c);
}
//===========================================================================
struct My_double a , b , c;
char s[70];

int main(){
	/*read(&a);
	write(&a);*/
	read(&a);
	read(&b);
//	write(&a),get_string(&a);
//	write(&b),get_string(&b);
	div(&a , &b , &c);
//	get_string(&c);
	write(&c);
	/*scanf("%s",s);
	set_string(&a , s);
	scanf("%s",s);
	set_string(&b , s);
	mul(&a , &b , &c);
	write(&c);*/
	return 0;
}
