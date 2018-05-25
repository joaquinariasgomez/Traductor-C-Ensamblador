%{

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <stack>
#include "TablaVariables.h"
#include "node.h"

using namespace std;

int yyerror(const char* msj);
int yylex(void);

bool estoy_en_void=false;
bool return_en_int=false;

TablaVariables variables;
TablaVariables variables_locales;
TablaVariables variables_args;

vector<string> array_dimensions;

vector<string> call_pushs;

stack<int> etiquetas_while;
stack<int> etiquetas_if;
stack<int> etiquetas_and;
stack<int> etiquetas_or;
stack<int> etiquetas_not;
stack<int> etiquetas_eq;
stack<int> etiquetas_neq;
stack<int> etiquetas_gte;
stack<int> etiquetas_gt;
stack<int> etiquetas_lte;
stack<int> etiquetas_lt;

int cont_strings = 0;
int cont_args_printf = 0;
int contador_etiqueta_while = 0;
int contador_etiqueta_if = 0;
int contador_etiqueta_and = 0;
int contador_etiqueta_or = 0;
int contador_etiqueta_not = 0;
int contador_etiqueta_eq = 0;
int contador_etiqueta_neq = 0;
int contador_etiqueta_gte = 0;
int contador_etiqueta_gt = 0;
int contador_etiqueta_lte = 0;
int contador_etiqueta_lt = 0;

Tree::NodoRaiz raiz;

ofstream f;

void TrueIfNoReferences(string str);
void TrueIfAnyReferences(string str);

%}

%error-verbose

%code requires
{
#include "node.h"
}

%union {
 int valor;
 std::string * nombre;
 char caracter;
 Tree::Node* nodo;
 Tree::NodoTipo* nodoTipo;
}

%token <nombre> INT FLOAT DOUBLE IF ELSE AND OR NOT EQ NEQ LT LTE GT GTE NUM ID CARACTER PRINTF STRING TRUE FALSE WHILE VOID RETURN SCANF

%type <nombre> sentencia_cond terminacion_while 

%type <caracter> opAritBajaPrioridad opAritAltaPrioridad

%type <nodo> sentencia_asig sentencia_def ambito argumento_scanf argumento_printf referencia_variables funcion lineas sentencia_def_local sentencia_return sentencia sentencia_scanf sentencia_print sentencia_llamada_funcion operando exp1 exp2 exp3 exp4 exp5 exp6 expresion argumentos args array

%start inicio

%%

inicio: inicio ambito
	| /*epsilon*/
;

ambito: sentencia_def ';'
    | funcion
;

funcion: INT ID '(' argumentos_def ')' { Tree::NodoFuncion* n = new Tree::NodoFuncion(*$2, variables_locales);} '{' lineas '}' {variables_args.clearAll(); if(!return_en_int) {yyerror((to_string(num_linea)+") Se esperaba return en funcion int").c_str());} else return_en_int=false;}
    | VOID ID '(' argumentos_def ')' { estoy_en_void=true; Tree::NodoFuncion* n = new Tree::NodoFuncion(*$2, variables_locales);} '{' lineas '}' {variables_args.clearAll(); Tree::NodoFinalFuncion* n = new Tree::NodoFinalFuncion(); estoy_en_void=false;}
;

argumentos_def: argumentos_def ',' INT ID {variables_args.addVariable("int", *$4, array_dimensions);}
	| INT ID {variables_args.addVariable("int", *$2,array_dimensions);}
	| /*epsilon*/
;

lineas: lineas sentencia
    | /*epsilon*/
;

sentencia_return: RETURN expresion ';' {Tree::NodoReturn* n = new Tree::NodoReturn($2, variables, variables_locales, variables_args);}
;

sentencia: sentencia_asig ';'
	| sentencia_def_local ';'
	| sentencia_cond
	| sentencia_print ';'
	| sentencia_scanf ';'
	| sentencia_llamada_funcion ';'
    | sentencia_return      {return_en_int=true; if(estoy_en_void) {yyerror((to_string(num_linea)+") No se esperaba sentencia return en funcion void").c_str());}}
;

sentencia_llamada_funcion: ID '(' argumentos ')' {Tree::NodoCall* n = new Tree::NodoCall(*$1, call_pushs); $$=n;}
;

argumentos: argumentos ',' args
	| args
;

args: expresion { Tree::NodoPush* n = new Tree::NodoPush($1, call_pushs, variables, variables_locales, variables_args); $$=n;}
	| /*epsilon*/
;

sentencia_asig: ID '=' expresion {Tree::NodoAsignacion* n = new Tree::NodoAsignacion(new Tree::NodoId(*$1), $3); n->asignar(variables, variables_locales, variables_args); $$ = n;}
	| ID '=' '&' ID {Tree::NodoAsignacion* n = new Tree::NodoAsignacion(new Tree::NodoId(*$1), new Tree::NodoId(*$4, true)); n->asignar(variables, variables_locales, variables_args); $$ = n;}
;

sentencia_def: INT ID {Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("int"), new Tree::NodoId(*$2)); n->declarar(variables,false,array_dimensions); $$ = n;}
	| INT ID array { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("inta"), new Tree::NodoId(*$2)); n->declarar(variables,false,array_dimensions); $$ = n; array_dimensions.clear();}
	| INT puntero ID { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("intp"), new Tree::NodoId(*$3)); n->declarar(variables,false,array_dimensions); $$ = n; array_dimensions.clear();}
;

array: array '[' NUM ']' { array_dimensions.push_back(*$3);}
	| '[' NUM ']' { array_dimensions.push_back(*$2); }
;

puntero: puntero '*' { array_dimensions.push_back("1"); }
	| '*' { array_dimensions.push_back("1"); }
;

sentencia_def_local: INT ID '=' expresion {Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("int"), new Tree::NodoId(*$2), $4); n->instanciar(variables, variables_locales, variables_args,array_dimensions); $$ = n;}
	| INT ID '=' '&' ID {Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("int"), new Tree::NodoId(*$2), new Tree::NodoId(*$5,true)); n->instanciar(variables, variables_locales, variables_args,array_dimensions); $$ = n;}
	| INT ID {Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("int"), new Tree::NodoId(*$2)); n->declarar(variables_locales,true,array_dimensions); $$ = n;}
	| INT ID array { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("inta"), new Tree::NodoId(*$2)); n->declarar(variables_locales,true,array_dimensions); $$ = n; array_dimensions.clear();}
	| INT ID array '=' expresion { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("inta"), new Tree::NodoId(*$2),$5); n->instanciar(variables, variables_locales, variables_args,array_dimensions); $$ = n; array_dimensions.clear();}
	| INT ID array '=' '&' ID { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("inta"), new Tree::NodoId(*$2),new Tree::NodoId(*$6)); n->instanciar(variables, variables_locales, variables_args,array_dimensions); $$ = n; array_dimensions.clear();}
	| INT puntero ID { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("intp"), new Tree::NodoId(*$3)); n->declarar(variables_locales,true,array_dimensions); $$ = n; array_dimensions.clear();}
	| INT puntero ID '=' expresion { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("intp"), new Tree::NodoId(*$3),$5); n->instanciar(variables, variables_locales, variables_args,array_dimensions); $$ = n; array_dimensions.clear();}
	| INT puntero ID '=' '&' ID { Tree::NodoDefinicion* n = new Tree::NodoDefinicion(new Tree::NodoTipo("intp"), new Tree::NodoId(*$3), new Tree::NodoId(*$6,true)); n->instanciar(variables, variables_locales, variables_args,array_dimensions); $$ = n; array_dimensions.clear();}
;

sentencia_cond: IF '(' expresion ')' {++contador_etiqueta_if; etiquetas_if.push(contador_etiqueta_if); Tree::NodoIf* n=new Tree::NodoIf(variables, variables_locales, variables_args, $3, NULL, NULL, 1, etiquetas_if);} terminacion_if
    | WHILE '(' {++contador_etiqueta_while; etiquetas_while.push(contador_etiqueta_while); Tree::NodoWhile* n=new Tree::NodoWhile(variables, variables_locales, variables_args, NULL, NULL, 1, etiquetas_while);} expresion ')' {Tree::NodoWhile* n=new Tree::NodoWhile(variables, variables_locales, variables_args, $4, NULL, 2, etiquetas_while);} terminacion_while
;

terminacion_if: '{' lineas '}' {Tree::NodoIf* n=new Tree::NodoIf(variables, variables_locales, variables_args, NULL, $2, NULL, 2, etiquetas_if);} ELSE '{' lineas '}' {Tree::NodoIf* n=new Tree::NodoIf(variables, variables_locales, variables_args, NULL, NULL, $7, 3, etiquetas_if);}
    | '{' lineas '}'           {Tree::NodoIf* n=new Tree::NodoIf(variables, variables_locales, variables_args, NULL, $2, NULL, 4, etiquetas_if);}
    | sentencia_asig ';'       {Tree::NodoIf* n=new Tree::NodoIf(variables, variables_locales, variables_args, NULL, $1, NULL, 4, etiquetas_if);}
    | sentencia_def_local ';'  {Tree::NodoIf* n=new Tree::NodoIf(variables, variables_locales, variables_args, NULL, $1, NULL, 4, etiquetas_if);}
;

terminacion_while: '{' lineas '}' {Tree::NodoWhile* n=new Tree::NodoWhile(variables, variables_locales, variables_args, NULL, $2, 3, etiquetas_while);}
    | sentencia_asig ';'          {Tree::NodoWhile* n=new Tree::NodoWhile(variables, variables_locales, variables_args, NULL, $1, 3, etiquetas_while);}
    | sentencia_def_local ';'     {Tree::NodoWhile* n=new Tree::NodoWhile(variables, variables_locales, variables_args, NULL, $1, 3, etiquetas_while);}
;

sentencia_scanf: SCANF '(' STRING ',' argumento_scanf ')' {TrueIfAnyReferences(*$3); cont_args_printf=0; Tree::NodoString* n = new Tree::NodoString(*$3,cont_strings); cont_strings++; Tree::NodoCallScanf* n2 = new Tree::NodoCallScanf(call_pushs,cont_strings);} 
;

argumento_scanf: argumento_scanf ',' '&' ID {Tree::NodoPushScanf* n = new Tree::NodoPushScanf(*$4, call_pushs, variables, variables_locales, variables_args); cont_args_printf++; $$=n;}
	| '&' ID {Tree::NodoPushScanf* n = new Tree::NodoPushScanf(*$2, call_pushs, variables, variables_locales, variables_args); cont_args_printf++; $$=n;}
;

sentencia_print: PRINTF '(' argumento_printf ')' {Tree::NodoCallPrintf* n = new Tree::NodoCallPrintf(call_pushs, cont_strings); $$=n;}
;

argumento_printf: STRING {TrueIfNoReferences(*$1); Tree::NodoString* n = new Tree::NodoString(*$1,cont_strings); cont_strings++; $$=n;}
	| STRING ',' referencia_variables {TrueIfAnyReferences(*$1); cont_args_printf=0; Tree::NodoString* n = new Tree::NodoString(*$1,cont_strings); cont_strings++; $$=n;}
;

referencia_variables: referencia_variables ',' expresion {Tree::NodoPush* n = new Tree::NodoPush($3, call_pushs, variables, variables_locales, variables_args); cont_args_printf++; $$=n;}
	| expresion {Tree::NodoPush* n = new Tree::NodoPush($1, call_pushs, variables, variables_locales, variables_args); cont_args_printf++; $$=n;}
;

expresion: expresion {++contador_etiqueta_or; etiquetas_or.push(contador_etiqueta_or); Tree::NodoOR* n=new Tree::NodoOR(variables, variables_locales, variables_args, $1, 1, etiquetas_and);} OR exp1 {Tree::NodoOR* n=new Tree::NodoOR(variables, variables_locales, variables_args, $4, 2, etiquetas_or);}
    | exp1              {$$=$1;}
;

exp1: exp1 {++contador_etiqueta_and; etiquetas_and.push(contador_etiqueta_and); Tree::NodoAND* n=new Tree::NodoAND(variables, variables_locales, variables_args, $1, 1, etiquetas_and);} AND exp2 {Tree::NodoAND* n=new Tree::NodoAND(variables, variables_locales, variables_args, $4, 2, etiquetas_and);}
    | exp2              {$$=$1;}
;

exp2: NOT exp2          {++contador_etiqueta_not; etiquetas_not.push(contador_etiqueta_not); Tree::NodoNOT* n=new Tree::NodoNOT(variables, variables_locales, variables_args, $2, etiquetas_not); $$=n;}
    | exp3              {$$=$1;}
;

exp3: exp3 EQ exp4      {++contador_etiqueta_eq; etiquetas_eq.push(contador_etiqueta_eq); Tree::NodoEQ* n=new Tree::NodoEQ(variables, variables_locales, variables_args, $1, $3, etiquetas_eq); $$=n;}
    | exp3 NEQ exp4     {++contador_etiqueta_neq; etiquetas_neq.push(contador_etiqueta_neq); Tree::NodoNEQ* n=new Tree::NodoNEQ(variables, variables_locales, variables_args, $1, $3, etiquetas_neq); $$=n;}
    | exp3 GTE exp4     {++contador_etiqueta_gte; etiquetas_gte.push(contador_etiqueta_gte); Tree::NodoGTE* n=new Tree::NodoGTE(variables, variables_locales, variables_args, $1, $3, etiquetas_gte); $$=n;}
    | exp3 GT exp4      {++contador_etiqueta_gt; etiquetas_gt.push(contador_etiqueta_gt); Tree::NodoGT* n=new Tree::NodoGT(variables, variables_locales, variables_args, $1, $3, etiquetas_gt); $$=n;}
    | exp3 LTE exp4     {++contador_etiqueta_lte; etiquetas_lte.push(contador_etiqueta_lte); Tree::NodoLTE* n=new Tree::NodoLTE(variables, variables_locales, variables_args, $1, $3, etiquetas_lte); $$=n;}
    | exp3 LT exp4      {++contador_etiqueta_lt; etiquetas_lt.push(contador_etiqueta_lt); Tree::NodoLT* n=new Tree::NodoLT(variables, variables_locales, variables_args, $1, $3, etiquetas_lt); $$=n;}
    | exp4              {$$=$1;}
;

exp4: exp4 opAritBajaPrioridad exp5 {Tree::NodoExpresion* n = new Tree::NodoExpresion($1, $2, $3); n->operar(variables, variables_locales, variables_args); $$=n;}
    | exp5              {$$=$1;}
;

exp5: exp5 opAritAltaPrioridad exp6 {Tree::NodoExpresion* n = new Tree::NodoExpresion($1, $2, $3); n->operar(variables, variables_locales, variables_args); $$=n;}
    | exp6              {$$=$1;}
;

exp6: '-' exp6          {Tree::NodoMenosUnario* n=new Tree::NodoMenosUnario($2); n->operar(variables, variables_locales, variables_args); $$=n;}
    | '(' expresion ')' {$$=$2;}
    | operando          {$$=$1;}
    | sentencia_llamada_funcion {$$=$1;}
    | TRUE              {$$=new Tree::NodoConstante(*$1,"bool");}
    | FALSE             {$$=new Tree::NodoConstante(*$1,"bool");}
;

operando: NUM {$$=new Tree::NodoConstante(*$1,"int");}
	| FLOAT {$$=new Tree::NodoConstante(*$1,"float");}	
	| DOUBLE {$$=new Tree::NodoConstante(*$1,"double");}
	| CARACTER {$$=new Tree::NodoConstante(*$1,"char");}
	| ID  {$$=new Tree::NodoId(*$1);}
;

opAritBajaPrioridad: '+' {$$ = '+';}
                   | '-' {$$ = '-';}
;

opAritAltaPrioridad: '*' {$$ = '*';}
                   | '/' {$$ = '/';}
;

%%

void TrueIfNoReferences(string str) {
	bool res = true;
	for(int i=0;i<str.length();i++){
		if(str.at(i) == '%') res = false;
	}

	if(!res) yyerror((to_string(num_linea)+") Hay referencias cuando no deberia.").c_str());
}

void TrueIfAnyReferences(string str){
  bool res = false;
  for(int i=0;i<str.length();i++){
		if(str.at(i) == '%') {
            res = true;
        }
	}

  if(!res) yyerror((to_string(num_linea)+") No hay referencias, cuando debería.").c_str());
	else {
    //Print str without ""
    int cont=1;
    int puntero=0;
    bool isVariable=false;
    int numberOfVariables=0;

    //Run first time to secure that number of variables matches number of printf_arguments
    while(cont < str.length()-1) {
      if(str[cont]!='%' && !isVariable) {
        ++cont;
      }
      else {
        //wait till space or to end to increment numberOfVariables
        isVariable=true;
        if(str[cont]==' ' || cont==str.length()-2) {
          isVariable=false;
          ++numberOfVariables;
        }
        else {
          ++cont;
        }
      }
    }

    if(numberOfVariables!=cont_args_printf) {
      yyerror((to_string(num_linea)+") El numero de referencias no concuerda con el numero de variables").c_str());
      return;
    }

  }
}

int yyerror(const char* msj){
	cerr<<msj<<endl;
	return 1;
}

int main(){
    f.open("ejer.s");
	yyparse();
    f.close();

	return EXIT_SUCCESS;
}
