/* PARA DEPURAR

	g1,g2,gi -> globales
	l1,l2,li -> locales
	a1,a2,ai -> argumentos
*/

int g1;
int g2;
int g3;

/*
int main(int a1) {
    int l1=6;
    scanf("%d %d %d",&l1, &a1, &g1);
    return 0;
}
*/


/*TODO: aun no tiene if*/

/*TODO: aun no tiene while*/



void f2_(int a1, int a2){
	int l1 = 3;
	/*int l3 = 3*1;
	l1 = 1; //TODO: errores cuando en expresion hay variables locales, globales, argumentos

	g2 = 3*5 + 1;

	int l2 = a1;
	int l4=g2;
	int l5=l1;

	a1 = l1;

	l1 = a2;

	l1 = g1;
	
	
	printf("hola");
	printf("hey %d %d %d",l1,a1,g1); //TODO: no se puede poner seguidos %d%d%d

	f2_(l1,5);
	g3 = prueba(a2,6);
	*/
	scanf("%d %d %d",&a1,&l1,&g1);
}

int prueba(){
	int l8;
	f2_(2,3);
	g1=l8;
	g1 = prueba2_3(4,g1);

	return 2;
}
