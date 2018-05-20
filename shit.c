int fact(int n)
{
//if (n <= 1)
    //return 1;
//else
return fact(n-1);
}
int numero;
int main()
{
printf(“Dame un entero : “);
scanf(“%d”, &numero);
printf(“El factorial = %d\n”, fact(numero));
return 0;
}
