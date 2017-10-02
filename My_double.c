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
	char v[double_tot_siz + 5];
};//从0开始，正序位数升高

struct big_bin binary_1 , binary_2 , binary_res;

void big_bin_clear(struct big_bin *a){
	a -> len = 1;
	memset(a -> v , 0 , sizeof(a -> v));
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
void big_bin_com(struct big_bin *a){//求补码
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
	while (res -> v[len - 1] == 0) len--;
	res -> len = len;
}
void big_bin_mul(const struct big_bin *a , const struct big_bin *b , struct big_bin *res){//不带补码的乘法
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

void big_bin_reverse(struct big_bin *a){
	int len = a -> len;
	for (int i = 0 , j = len - 1 ; i < j ; i++ , j--)
		swap(a -> v + i , a -> v + j);
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
void big_bin_rounding(struct big_bin *a){//浮点数的核心
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
	for (int i = double_tot_bit - 1 ; i >= 0 ; i--){
		putchar('0' + get_digit(a , i));
	}
	putchar('\n');
}

void My_double_clear(struct My_double *a){
	memset(a -> digit , 0 , sizeof(a -> digit));
}

char My_double_str[double_tot_siz];
void get_from_rounding(struct My_double *a , struct big_bin *binary , int original_E){
	big_bin_output(binary);
	printf("%d\n",original_E);
	int len = binary -> len;
	int new_E = original_E + len - 1;
	int exp = 0;
	if (new_E > double_bias){
		if (get_s(a)) set_string(a , negative_inf);
		else set_string(a , positive_inf);
	}
	if (new_E >= 1 - double_bias){//normalized
		exp = new_E + double_bias;
		binary -> v[len - 1] = 0;
		len--;
		binary -> len = len;
	}
	else if (new_E == -double_bias){//denormalized
		new_E++;
		exp = 0;
	}
	else{//0
		if (get_s(a)) set_string(a , negative_zero);
		else set_string(a , positive_zero);
		return;
	}
	
	big_bin_rounding(binary);
	if (big_bin_is_zero(binary)){		
		if (get_s(a)) set_string(a , negative_zero);
		else set_string(a , positive_zero);
		return;
	}
	
	printf("new_E = %d exp = %d\n",new_E,exp);
	big_bin_output(binary);
	
	for (int i = 0 , j = double_n ; i < double_k ; i++ , j++)
		set_digit(a , j , (exp >> i) & 1);
		
	for (int i = len - 1 , j = double_n - 1 ; i >= 0 || j >= 0 ; i-- , j--)//设置frac
		if (i < 0){
			set_digit(a , j , 0);
		}
		else{
			set_digit(a , j , binary -> v[i]);
		}
	
	get_string(a);
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
	if (str[0] == '+' || str[1] == '-'){
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
	//for (int i=0;i<M_len;i++) printf("%d",M[i]);putchar('\n');
	res_int . len = len_int;//res_int是big_int，而且是从0开始 && 正序位数升高
	for (int i = 0 ; i < len_int ; i++){
		res_int . v[i] = M[len_int - i - 1];
	}
	res_dec . len = len_dec;//res_dec是tni_gib，而且是从1开始 && 逆序位数升高
	for (int i = len_int ; i < M_len ; i++){
		res_dec . v[i - len_int + 1] = M[i];
	}
	//big_int_output(&res_int);
	//tni_gib_output(&res_dec);
	
	big_bin_clear(&binary_1);
	int len_1 = 0;
	while (!big_int_is_zero(&res_int)){
		//printf("%d\n",len_1);
		//printf("aaaa "),big_int_output(&res_int);
		if (len_1 == double_bias + 1){
			if (get_s(a)) set_string(a , negative_inf);
			else set_string(a , positive_inf);
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
			bin_E -= n + 2 + (len_2 - start - 1);
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
				puts("FUCK");
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
	putchar('\n');
}
//位数与二的幂的对应关系
//1/0 x_1     x_2     x_3     ... x_52
//2^E 2^(E-1) 2^(E-2) 2^(E-3)     2^(E-52)

//===========================================================================
struct My_double a;
char s[70];

int main(){
	read(&a);
	write(&a);
	return 0;
}
