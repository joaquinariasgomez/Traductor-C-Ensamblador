int g1;

int g2[1][4];
int g3[3];
int g4[1][2][3];

void main(int a1) {
/*	Esto se lo traga
	int a;
	int b;
	int c;
	int d;
	int e;
	
	a = true;
	b = 2;
	c = 3.2;
	d = 2.3f;
	e = '0';

	int y;
	y = 3.4/2.4;
	y = func(2.3,4);

	int f = true;
	int g = 2;
	int h = 3.2;
	int i = 2.3f;
	int j = '0';
	int k = 3.4/2.5;

	if(2.3 > 1.1f){}else{}

	int l1[3];
	int l2[5][1][4][2];
	int l3[4] = l2;
	int l4[2] = 2*2;
	int l5[3] = l1;
	int l6 = l1*2;
	int l7 = 3*l1;
	
	int l1[3];
	int * l2;
	int * l3;
	int l4=2;
	while(l1 && l2){} //debe petar
	if(l1>3){}else{} //debe petar
	l2 = l1; //debe petar
	l2 = l4; //debe petar
	l3 = 2*3; //debe petar
	l3 = l2; //funciona
	l3 = l2+4; //debe petar
	int ** l6 = l2; //debe petar
	
	
*/	
	
	int l1;
	int l3 = &l1; //debe petar
	int * l2 = &l1; //funciona
	int * l7;
	int l4[3] = l7; //debe petar
	int l5[4] = &l1; //debe petar
	int ** l6 = &l7; //funciona
	int ** l8 = &l1; //debe petar
	int * l10;
	int ** l11;
	l11 = &l10; //funciona
	l10 = &l11; //debe petar
	

}

int func(){
	return 2.3;
}
