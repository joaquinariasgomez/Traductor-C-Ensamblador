#include "TablaVariables.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

void TablaVariables::addVariable(string t, string id, vector<string> v){
	tipos.push_back(t);
	ids.push_back(id);
	valores.push_back(v);
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

string TablaVariables::getType(int i){
	return tipos[i];
}

string TablaVariables::getId(int i){
	return ids[i];
}

vector<string> TablaVariables::getDimensions(int i){
	return valores[i];
}

void TablaVariables::clearAll(){
	tipos.clear();
	ids.clear();
	valores.clear();
	//for(vector<string>::iterator it = valores.begin();it!=valores.end();it++){
	//	*it.clear();
	//}
}