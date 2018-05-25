%{
	#include<iostream>
	#include<map>
	#include<string>
	#include "parser.tab.h"
	#include "node.h"

	using namespace std;
%}

%option noyywrap
%x comment
%x comment2

INT		"int"
TRUE    	"true"
FALSE   	"false"
IF		"if"
ELSE		"else"
WHILE		"while"
VOID		"void"
PRINTF		"printf"
SCANF		"scanf"
RETURN		"return"
AND		"&&"
OR		"||"
NOT		"!"
EQ		"=="
NEQ		"!="
LT		"<"
LTE		"<="
GT		">"
GTE		">="

%%

"/*"		BEGIN(comment);

<comment>[^*\n]*	;	//eat anything that's not a '*'
<comment>"*"+[^*/\n]*	;	//eat up '*'s not followed by '/'s
<comment>\n 		{num_linea++;}
<comment>"*"+"/"	BEGIN(INITIAL);

"//"		BEGIN(comment2);

<comment2>[^\n]	;	/*eat anything that's not a \n */
<comment2>\n 		{num_linea++; BEGIN(INITIAL);}	//end


[ \t\r]+ 	; /*se come los espacios en blanco */

[\n]		{num_linea++;}

{INT}			{yylval.nombre = new string(yytext); return INT;}

{TRUE}			{yylval.nombre = new string(yytext); return TRUE;}
{FALSE}			{yylval.nombre = new string(yytext); return FALSE;}

{IF}			{yylval.nombre = new string(yytext); return IF;}
{ELSE}			{yylval.nombre = new string(yytext); return ELSE;}

{WHILE}			{yylval.nombre = new string(yytext); return WHILE;}

{VOID}			{yylval.nombre = new string(yytext); return VOID;}

{PRINTF}		{yylval.nombre = new string(yytext); return PRINTF;}

{SCANF}		{yylval.nombre = new string(yytext); return SCANF;}

{RETURN}		{yylval.nombre = new string(yytext); return RETURN;}

{AND}			{yylval.nombre = new string(yytext); return AND;}
{OR}			{yylval.nombre = new string(yytext); return OR;}
{NOT}			{yylval.nombre = new string(yytext); return NOT;}

{EQ}			{yylval.nombre = new string(yytext); return EQ;}
{NEQ}			{yylval.nombre = new string(yytext); return NEQ;}
{LT}			{yylval.nombre = new string(yytext); return LT;}
{LTE}			{yylval.nombre = new string(yytext); return LTE;}
{GT}			{yylval.nombre = new string(yytext); return GT;}
{GTE}			{yylval.nombre = new string(yytext); return GTE;}

[a-zA-Z_][a-zA-Z0-9_]* 	{yylval.nombre = new string(yytext); return ID;}
"'"[0-9a-zA-Z]"'"			{yylval.nombre = new string(yytext); return CARACTER;}
"\""[a-zA-Z0-9 %=():?¿!¡@#~$;{}\\\+\-\*\/\[\]_]*"\""	{yylval.nombre = new string(yytext); return STRING;}
[0-9]+	{yylval.nombre = new string(yytext); return NUM;}
[0-9]*\.[0-9]+"f"	{yylval.nombre = new string(yytext); return FLOAT;}
[0-9]+|[0-9]*\.[0-9]+	{yylval.nombre = new string(yytext); return DOUBLE;}

[=(),;{}\+\-\*\/\[\]&]		return *yytext;
.		{cout<<"filtrado caracter ("<<yytext<<")\n";}/* se come los caracteres que no se hayan tratado */

%%
/*
int main() {
	yylex();

	for(map<int, string>::iterator it=tabla.begin(); it!=tabla.end(); ++it) {
		cout << it->first << " -> " << it->second << endl;
	}
	cout<<"terminado"<<endl;
}
*/
