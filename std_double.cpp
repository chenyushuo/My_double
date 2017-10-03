#include<bits/stdc++.h>
using namespace std;
double a,b;
char ch;
typedef unsigned long long uint64;
void write(const uint64 &x){
	putchar((x >> 63) & 1 ? '1' : '0');
	putchar(' ');
	for (int i=62;i>=52;i--)
		putchar((x >> i) & 1 ? '1' : '0');
	putchar(' ');
	for (int i=51;i>=0;i--)
		putchar((x >> i) & 1 ? '1' : '0');
	putchar('\n');
	//printf("%llu\n",x);
}
void set_digit(uint64 *x,int p,int v){
	assert(v==0||v==1);
	//if (v) cout<<p<<endl;
	*x &= ((~0uLL)^(1LL<<p));
	*x |= ((1LL*v)<<p);
}
void set_double(uint64 *x,const char *s){
	for (int i = 0;i < 64;i++)
		set_digit(x,i,s[64-i-1] - '0');
}
int main(){
	//scanf("%lf%c%lf",&a,&ch,&b);
	double res;
	uint64 *x=(uint64 *)(&a);
	char s[70];
	scanf("%s",s);
	set_double(x,s);
	x=(uint64 *)(&b);
	scanf("%s",s);
	set_double(x,s);
	ch='*';
	printf("%.1200f\n",a);
	printf("%.1200f\n",b);
	//scanf("%lf",&res);
	//write(*x);
	//printf("%.1200f\n",res);
	//printf("%e\n",res);
	switch (ch){
		case '+':
			printf("%f\n",a+b);
			res=a+b;
			write(*x);
			printf("%f\n",res);
			break;
		case '-':
			printf("%f\n",a-b);
			res=a-b;
			write(*x);
			printf("%f\n",res);
			break;
		case '*':
			printf("%f\n",a*b);
			res=a*b;
			x=(uint64 *)(&res);
			write(*x);
			printf("%.1200f\n",res);
			break;
		case '/':
			printf("%f\n",a/b);
			res=a/b;
			write(*x);
			printf("%f\n",res);
			break;
	}
	return 0;
}
