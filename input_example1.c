int fact(int n) {
    if(n<=1) {
        n=1;
    }
    else {
        return n*fact(n);
    }
    return fact(n);
}

int numero;

int main() {
    scanf("%d", &numero);
    printf("El factorial = %d\n", fact(numero));
    return 0;
}
