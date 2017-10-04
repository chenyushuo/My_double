#include<bits/stdc++.h>
using namespace std;
double a,b;
char ch;
typedef unsigned long long uint64;
void write(const uint64 &x){
	printf("digit of double : ");
	putchar((x >> 63) & 1 ? '1' : '0');
//	putchar(' ');
	for (int i=62;i>=52;i--)
		putchar((x >> i) & 1 ? '1' : '0');
//	putchar(' ');
	for (int i=51;i>=0;i--)
		putchar((x >> i) & 1 ? '1' : '0');
	putchar('\n');
	//printf("%llu\n",x);
}
void write(const double &a){
	const uint64 *x=(uint64 *)(&a);
	write(*x);
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
void read(double &a){	
	uint64 *x=(uint64 *)(&a);
	char s[70];
	scanf("%s",s);
	set_double(x,s);
}
int main(){
/*	cout<<"FUCK"<<endl;
	double nan=fabs(sqrt(-1)) , inf=1.0/0.0;
	a=-0.0 , b=-0.0;
	printf("%f %f\n",a,b);
	printf(" (a <  b) = %d\n (a <= b) = %d\n (a >  b) = %d\n (a >= b) = %d\n (a != b) = %d\n (a == b) = %d\n" ,
			 (a <  b)      , (a <= b)      , (a >  b)      , (a >= b)      , (a != b)      , (a == b) );
	for(;;){*/
	scanf("%lf%c%lf",&a,&ch,&b);
	double res;
	//read(a),read(b);
	ch='/';
//	printf("%.1200f\n",a),write(a);
//	printf("%.1200f\n",b),write(b);
//	cout<<a<<' '<<b<<endl;
	switch (ch){
		case '+':
			res=a+b;
//			write(res);
			printf("%.1200f\n",res);
			break;
		case '-':
			res=a-b;
//			write(res);
			printf("%.1200f\n",res);
			break;
		case '*':
			res=a*b;
//			write(res);
			printf("%.1200f\n",res);
			break;
		case '/':
			res=a/b;
//			write(res);
			printf("%.1200f\n",res);
			break;
	}
//	}
	return 0;
}
