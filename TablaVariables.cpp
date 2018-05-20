#include "TablaVariables.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

TablaVariables::TablaVariables() {}

void TablaVariables::addVariable(string t, string id, string v){
	tipos.push_back(t);
	ids.push_back(id);
	valores.push_back(v);
}

void TablaVariables::eraseVariable(string id){
	int pos=0;
	bool encontrado=false;

	while(pos<ids.size() && !encontrado){
		if(ids[pos] == id) encontrado = true;
		else pos++;
	}

	if(encontrado) { //si no existe la variable no se hace nada
		tipos.erase(tipos.begin() + pos);
		ids.erase(ids.begin() + pos);
		valores.erase(valores.begin() + pos);
	}
}

void TablaVariables::modifyValue(string id, string v){
	int pos=0;
	bool encontrado=false;

	while(pos<ids.size() && !encontrado){
		if(ids[pos] == id) encontrado = true;
		else pos++;
	}

	if(encontrado) {
		valores[pos] = v;
	}
}

void TablaVariables::modifyType(string id, string t){
	int pos=0;
	bool encontrado=false;

	while(pos<ids.size() && !encontrado){
		if(ids[pos] == id) encontrado = true;
		else pos++;
	}

	if(encontrado) {
		tipos[pos] = t;
	}
}

/* si no existe esa variable, devuelve 9999 e imprime un error en consola*/
string TablaVariables::getValue(string id){
	int pos=0;
	bool encontrado=false;

	while(pos<ids.size() && !encontrado){
		if(ids[pos] == id) encontrado = true;
		else pos++;
	}

	if(encontrado) {
		return valores[pos];
	}else{
		cout<<"Error: se ha intentado obtener el valor de una variable que no existe"<<endl;
		return "9999";
	}
}

/* si no existe esa variable, devuelve 9999 e imprime un error en consola*/
string TablaVariables::getType(string id){
	int pos=0;
	bool encontrado=false;

	while(pos<ids.size() && !encontrado){
		if(ids[pos] == id) encontrado = true;
		else pos++;
	}

	if(encontrado) {
		return tipos[pos];
	}else{
		cout<<"Error: se ha intentado obtener el valor de una variable que no existe"<<endl;
		return "9999";
	}
}

void TablaVariables::printTabla(){
	cout<<"Tipo\tID\t\tValor"<<endl;
	cout<<"------------------------------"<<endl;
	for(int i=0;i<tipos.size();i++){
		cout<<tipos[i]<<"\t"<<ids[i]<<"\t\t"<<valores[i]<<endl;
	}
	cout<<endl;
}

int TablaVariables::getIndex(string id) {
	int res=0;
	bool encontrado=false;
	for(vector<string>::iterator it = ids.begin();it!=ids.end();it++){
		if(*it == id){ encontrado=true; break; }
		res++;
	}

	if(encontrado) return res;
	else return -1;
}

void TablaVariables::clearAll(){
	tipos.clear();
	ids.clear();
	valores.clear();
}

void TablaVariables::reverseAll(){
	std::reverse(tipos.begin(), tipos.end());
	std::reverse(ids.begin(), ids.end());
	std::reverse(valores.begin(), valores.end());
}
