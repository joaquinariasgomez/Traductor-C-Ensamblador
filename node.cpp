#include "node.h"
#include "TablaVariables.h"
#include <typeinfo>
#include <iostream>
#include <string>
#include <cstring>
#include <assert.h>
#include <algorithm>
#include <sstream>

using namespace std;

int num_linea=1;

int yyerror2(const string msj){
	cerr<<msj<<endl;
	return 1;
}

string checkType(string val, string type){
	string res;

	if(type == "bool"){
		if(val == "true"){
			res = "1";
		}else{
			res = "0";
		}
	}else if(type == "float"){
		res = to_string((int)stof(val));
	}else if(type == "double"){
		res = to_string((int)stod(val));
	}else if(type == "char"){
		res = to_string((int)val.at(1)); //ascii value se coge la segunda posicion porque es 'a', a es el caracter
	}else if(type == "int"){
		res = val;
	}

	return res;
}

bool sameDimension(vector<string> v1, vector<string> v2){
	int i=0;
	bool res=true;
	for(vector<string>::iterator it=v1.begin();it!=v1.end();it++){
		if(v1[i] != v2[i]){ res=false; break; }
		i++;	
	}

	return res;
}

bool sameDimensionPuntero(vector<string> v1, vector<string> v2, bool es_dir){
	int i=0;
	bool res=true;

	if(es_dir){
		if(v1.size() != v2.size()+1) res = false;	
	}else{
		if(v1.size() != v2.size()) res = false;
	}

	return res;
}

void checkTypeId(int ind1, TablaVariables& tab1, int ind2, TablaVariables& tab2, bool es_dir=false){
	if(tab1.getType(ind1) == tab2.getType(ind2)){
		if(tab1.getType(ind1) == "inta"){
			if(!sameDimension(tab1.getDimensions(ind1), tab2.getDimensions(ind2))){
				yyerror2(to_string(num_linea)+") Error al asignar un array de distinta dimension");
			}
		}else if(tab1.getType(ind1) == "intp"){
			if(!sameDimensionPuntero(tab1.getDimensions(ind1), tab2.getDimensions(ind2), es_dir)){
				yyerror2(to_string(num_linea)+") Error al asignar un puntero de distinta dimension");
			}
		}else if(es_dir){
			if(!sameDimensionPuntero(tab1.getDimensions(ind1), tab2.getDimensions(ind2), es_dir)){
				yyerror2(to_string(num_linea)+") Error al asignar un puntero de distinta dimension");
			}
		}else{
			//esta permitido
		}
	}else{
		if(es_dir){
			if(!sameDimensionPuntero(tab1.getDimensions(ind1), tab2.getDimensions(ind2), es_dir)){
				yyerror2(to_string(num_linea)+") Error al asignar un puntero de distinta dimension");
			}
		}else{
			yyerror2(to_string(num_linea)+") Error al asignar una variable a un tipo distinto");
		}
	}
}

void Tree::NodoAsignacion::asignar(TablaVariables& globales, TablaVariables& locales, TablaVariables& args){
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(n2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		int aux = locales.getIndex(n1_->id_);
		int indice;
		if(aux==-1){
			aux = args.getIndex(n1_->id_);
			if(aux==-1){
				aux = globales.getIndex(n1_->id_);
				assert(aux != -1);//es global
				f << "\tmovl $" << nodoConstante->val_ << ", " << n1_->id_ << endl;

			}else{//es argumento
				indice=8 + aux*4;
				f << "\tmovl $" << nodoConstante->val_ << ", " << indice << "(%ebp)" << endl;
			}

		}else{ //es local
			indice=(1+aux)*-4;
			f << "\tmovl $" << nodoConstante->val_ << ", " << indice << "(%ebp)" << endl;
		}

	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(n2_)){


		int aux = locales.getIndex(n1_->id_);
		int indice;
		if(aux==-1){
			aux = args.getIndex(n1_->id_);
			if(aux==-1){
				aux = globales.getIndex(n1_->id_);
				assert(aux != -1); //es global
				int aux2 = locales.getIndex(nodoId->id_);
				int indice2;
				if(aux2==-1){
					aux2 = args.getIndex(nodoId->id_);
					if(aux2==-1){
						aux2 = globales.getIndex(nodoId->id_);
						assert(aux2 != -1);//es global
						checkTypeId(aux, globales, aux2, globales, nodoId->es_direccion);

						f << "\tmovl " << nodoId->id_ << ", " << n1_->id_ << endl;;

					}else{//es argumento

						checkTypeId(aux, globales, aux2, args, nodoId->es_direccion);

						indice2=8 + aux2*4;
						f << "\tmovl " << indice2 << "(%ebp), " << n1_->id_ << endl;
					}

				}else{//es local

					checkTypeId(aux, globales, aux2, locales, nodoId->es_direccion);

					indice2=(1+aux2)*-4;
					f << "\tmovl " << indice2 << "(%ebp), " << n1_->id_ << endl;
				}

			}else{//es argumento
				indice=8 + aux*4;
				int aux2 = locales.getIndex(nodoId->id_);
				int indice2;
				if(aux2==-1){
					aux2 = args.getIndex(nodoId->id_);
					if(aux2==-1){
						aux2 = globales.getIndex(nodoId->id_);
						assert(aux2 != -1);//es global
						checkTypeId(aux, args, aux2, globales, nodoId->es_direccion);

						f << "\tmovl " << nodoId->id_ << ", " << indice << "(%ebp)" << endl;;

					}else{//es argumento

						checkTypeId(aux, args, aux2, args, nodoId->es_direccion);

						indice2=8 + aux2*4;
						f << "\tmovl " << indice2 << "(%ebp), " << indice << "(%ebp)" << endl;
					}

				}else{//es local

					checkTypeId(aux, args, aux2, locales, nodoId->es_direccion);

					indice2=(1+aux2)*-4;
					f << "\tmovl " << indice2 << "(%ebp), " << indice << "(%ebp)" << endl;
				}
			}

		}else{//es local
			indice=(1+aux)*-4;
			int aux2 = locales.getIndex(nodoId->id_);
			int indice2;
			if(aux2==-1){
				aux2 = args.getIndex(nodoId->id_);
				if(aux2==-1){
					aux2 = globales.getIndex(nodoId->id_);
					assert(aux2 != -1);//es global
					checkTypeId(aux, locales, aux2, globales, nodoId->es_direccion);

					f << "\tmovl " << nodoId->id_ << ", " << indice << "(%ebp)" << endl;;

				}else{//es argumento

					checkTypeId(aux, locales, aux2, args, nodoId->es_direccion);

					indice2=8 + aux2*4;
					f << "\tmovl " << indice2 << "(%ebp), " << indice << "(%ebp)" << endl;
				}

			}else{//es local

				checkTypeId(aux, locales, aux2, locales, nodoId->es_direccion);

				indice2=(1+aux2)*-4;
				f << "\tmovl " << indice2 << "(%ebp), " << indice << "(%ebp)" << endl;
			}
		}

	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(n2_)) {

        int aux = locales.getIndex(n1_->id_);
		int indice;
		if(aux==-1){
			aux = args.getIndex(n1_->id_);
			if(aux==-1){
				aux = globales.getIndex(n1_->id_);
				assert(aux != -1);//es global
				if(globales.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede asignar una expresion a un array o puntero");
				}
				f << "\tmovl %eax, " << n1_->id_ << endl;

			}else{//es argumento
				if(args.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede asignar una expresion a un array o puntero");
				}
				indice=8 + aux*4;
				f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
			}

		}else{ //es local
			if(locales.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede asignar una expresion a un array o puntero");
			}

			indice=(1+aux)*-4;
			f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
		}

	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(n2_)) {
		int aux = locales.getIndex(n1_->id_);
		int indice;
		if(aux==-1){
			aux = args.getIndex(n1_->id_);
			if(aux==-1){
				aux = globales.getIndex(n1_->id_);
				assert(aux != -1);//es global
				if(globales.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede asignar una expresion a un array o puntero");
				}
				f << "\tmovl %eax, " << n1_->id_ << endl;

			}else{//es argumento
				if(args.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede asignar una expresion a un array o puntero");
				}
				indice=8 + aux*4;
				f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
			}

		}else{ //es local
			if(locales.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede asignar una expresion a un array o puntero");
			}
			indice=(1+aux)*-4;
			f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
		}

	}
	else {
		int aux = locales.getIndex(n1_->id_);
		int indice;
		if(aux==-1){
			aux = args.getIndex(n1_->id_);
			if(aux==-1){
				aux = globales.getIndex(n1_->id_);
				assert(aux != -1);//es global
				f << "\tmovl %eax, " << n1_->id_ << endl;

			}else{//es argumento
				indice=8 + aux*4;
				f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
			}

		}else{ //es local
			indice=(1+aux)*-4;
			f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
		}
	}

}

void Tree::NodoDefinicion::declarar(TablaVariables& tab, bool esLocal, vector<string> dim){
	if(n1_->tipo_ == "inta"){
		tab.addVariable(n1_->tipo_, n2_->id_, dim);
	}else if(n1_->tipo_ == "intp"){
		tab.addVariable(n1_->tipo_, n2_->id_, dim);
	}else{
		tab.addVariable(n1_->tipo_, n2_->id_,dim);
	}

	if(esLocal){
		f << "\tsubl $4, %esp" << endl;
	} else {
		f.close();

		ofstream salida;
		ifstream fichero;

		fichero.open("ejer.s");
		salida.open("ejer2.s", ios::app);

		salida << ".comm "<< n2_->id_ << ",4,4\n";

		char cadena[80];

		int i=1;
		if(!fichero.fail()){
			fichero.getline(cadena,80,'\n');
		}
		while(!fichero.eof()){
			salida << cadena << endl;
			i++;
			fichero.getline(cadena,80,'\n');
		}

		fichero.close();
		salida.close();
		remove("ejer.s");
		rename("ejer2.s", "ejer.s");
		f.open("ejer.s", ios_base::app);
	}
}

void Tree::NodoDefinicion::instanciar(TablaVariables& globales, TablaVariables& locales, TablaVariables& args, vector<string> dim){
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(n3_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		if(n1_->tipo_ == "inta"){
			yyerror2(to_string(num_linea)+") Error no se puede asignar un literal a un array");
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else if(n1_->tipo_ == "intp"){
			yyerror2(to_string(num_linea)+") Error no se puede asignar un literal a un puntero");
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else{
			locales.addVariable(n1_->tipo_, n2_->id_,dim);
		}

		f << "\tsubl $4, %esp" << endl;

		int aux = locales.getIndex(n2_->id_);
		int indice;

		assert(aux!=-1);

		indice=(1+aux)*-4;
		f << "\tmovl $" << nodoConstante->val_ << ", " << indice << "(%ebp)" << endl;

	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(n3_)){
		
		if(n1_->tipo_ == "inta"){
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else if(n1_->tipo_ == "intp"){
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else{
			locales.addVariable(n1_->tipo_, n2_->id_,dim);
		}

		f << "\tsubl $4, %esp" << endl;

		int aux = locales.getIndex(n2_->id_);
		int indice;
		assert(aux!=-1);
		indice=(1+aux)*-4;

		int aux2 = locales.getIndex(nodoId->id_);
		int indice2;
		if(aux2==-1){
			aux2 = args.getIndex(nodoId->id_);
			if(aux2==-1){
				aux2 = globales.getIndex(nodoId->id_);
				assert(aux2 != -1);//es global
				checkTypeId(aux,locales,aux2,globales, nodoId->es_direccion);

				f << "\tmovl " << nodoId->id_ << ", " << indice << "(%ebp)" << endl;

			}else{//es argumento

				checkTypeId(aux,locales,aux2,args, nodoId->es_direccion);

				indice2=8 + aux2*4;
				f << "\tmovl " << indice2 << "(%ebp), " << indice << "(%ebp)" << endl;
			}

		}else{//es local

			checkTypeId(aux,locales,aux2,locales, nodoId->es_direccion);

			indice2=(1+aux2)*-4;
			f << "\tmovl " << indice2 << "(%ebp), " << indice << "(%ebp)" << endl;
		}

	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(n3_)) {
        
		if(n1_->tipo_ == "inta"){
			yyerror2(to_string(num_linea)+") Error no se puede asignar un expresion a un array");
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else if(n1_->tipo_ == "intp"){
			yyerror2(to_string(num_linea)+") Error no se puede asignar un expresion a un puntero");
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else{
			locales.addVariable(n1_->tipo_, n2_->id_,dim);
		}

        f << "\tsubl $4, %esp" << endl;

        int aux = locales.getIndex(n2_->id_);
		int indice;

		assert(aux!=-1);

		indice=(1+aux)*-4;
		f << "\tmovl %eax, " << indice << "(%ebp)" << endl;

	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(n3_)) {
        
		if(n1_->tipo_ == "inta"){
			yyerror2(to_string(num_linea)+") Error no se puede asignar un expresion a un array");
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else if(n1_->tipo_ == "intp"){
			yyerror2(to_string(num_linea)+") Error no se puede asignar un expresion a un puntero");
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else{
			locales.addVariable(n1_->tipo_, n2_->id_,dim);
		}

        f << "\tsubl $4, %esp" << endl;

        int aux = locales.getIndex(n2_->id_);
		int indice;

		assert(aux!=-1);

		indice=(1+aux)*-4;
		f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
	}
	else {
		
		if(n1_->tipo_ == "inta"){
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else if(n1_->tipo_ == "intp"){
			locales.addVariable(n1_->tipo_, n2_->id_, dim);
		}else{
			locales.addVariable(n1_->tipo_, n2_->id_,dim);
		}

        f << "\tsubl $4, %esp" << endl;

        int aux = locales.getIndex(n2_->id_);
		int indice;

		assert(aux!=-1);

		indice=(1+aux)*-4;
		f << "\tmovl %eax, " << indice << "(%ebp)" << endl;
	}
}

void Tree::NodoExpresion::operar(TablaVariables& variables, TablaVariables& variables_locales, TablaVariables& variables_args){
	std::string operando1;
	std::string operando2;

	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(n1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		operando1=nodoConstante->val_;
		//Salvar eax por si se sobreescribe, dado que este almacenará los resultados temporales
		f << "\tmovl %eax, %ebx" << endl;
		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(n1_)){
		f << "\tmovl %eax, %ebx" << endl;
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(n1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(n1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}
	else if(NodoCall* nodoCall=dynamic_cast<NodoCall*>(n1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}

	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(n2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		switch(operador_) {
			case '+': f << "\taddl $" << nodoConstante->val_ << ", %eax" << endl; break;
			case '-': f << "\tsubl $" << nodoConstante->val_ << ", %eax" << endl; break;
			case '*': f << "\timull $" << nodoConstante->val_ << ", %eax" << endl; break;
			case '/': f << "\tmovl $" << nodoConstante->val_ << ", %ecx" << endl;
					  f << "\tcdq" << endl;
					  f << "\tdivl %ecx" << endl; break;
		}
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(n2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
				}

				switch(operador_) {
					case '+': f << "\taddl " << nodoId->id_ << ", %eax" << endl; break;
					case '-': f << "\tsubl " << nodoId->id_ << ", %eax" << endl; break;
					case '*': f << "\timull " << nodoId->id_ << ", %eax" << endl; break;
					case '/': f << "\tmovl " << nodoId->id_ << ", %ecx" << endl;
							  f << "\tcdq" << endl;
							  f << "\tdivl %ecx" << endl; break;
				}
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
				}

				int indice=8+aux*4;
				switch(operador_) {
					case '+': f << "\taddl " << indice << "(%ebp), %eax" << endl; break;
					case '-': f << "\tsubl " << indice << "(%ebp), %eax" << endl; break;
					case '*': f << "\timull " << indice << "(%ebp), %eax" << endl; break;
					case '/': f << "\tmovl " << indice << "(%ebp), %ecx" << endl;
							  f << "\tcdq" << endl;
							  f << "\tdivl %ecx" << endl; break;
				}
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
			}

			int indice=(1+aux)*-4;
			switch(operador_) {
				case '+': f << "\taddl " << indice << "(%ebp), %eax" << endl; break;
				case '-': f << "\tsubl " << indice << "(%ebp), %eax" << endl; break;
				case '*': f << "\timull " << indice << "(%ebp), %eax" << endl; break;
				case '/': f << "\tmovl " << indice << "(%ebp), %ecx" << endl;
						  f << "\tcdq" << endl;
						  f << "\tdivl %ecx" << endl; break;
			}
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(n2_)) {
		switch(operador_) {
			case '+': f << "\taddl %ebx, %eax" << endl; break;
			case '-': f << "\tsubl %eax, %ebx" << endl;
					  f << "\tmovl %ebx, %eax" << endl; break;
			case '*': f << "\timull %ebx, %eax" << endl; break;
			case '/': f << "\tmovl %ebx, %ecx" << endl;
					  f << "\tcdq" << endl;
					  f << "\tdivl %ecx" << endl; break;
		}
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(n2_)) {
		switch(operador_) {
			case '+': f << "\taddl %ebx, %eax" << endl; break;
			case '-': f << "\tsubl %eax, %ebx" << endl;
					  f << "\tmovl %ebx, %eax" << endl; break;
			case '*': f << "\timull %ebx, %eax" << endl; break;
			case '/': f << "\tmovl %ebx, %ecx" << endl;
					  f << "\tcdq" << endl;
					  f << "\tdivl %ecx" << endl; break;
		}
	}else if(NodoCall* nodoCall=dynamic_cast<NodoCall*>(n2_)) {
		switch(operador_) {
			case '+': f << "\taddl %ebx, %eax" << endl; break;
			case '-': f << "\tsubl %eax, %ebx" << endl;
					  f << "\tmovl %ebx, %eax" << endl; break;
			case '*': f << "\timull %ebx, %eax" << endl; break;
			case '/': f << "\tmovl %ebx, %ecx" << endl;
					  f << "\tcdq" << endl;
					  f << "\tdivl %ecx" << endl; break;
		}
	}

	if(operador_=='/') {
		//Move all temporary results to %eax
		f << "\tmovl %ecx, %eax" << endl;
	}
}

void Tree::NodoMenosUnario::operar(TablaVariables& variables, TablaVariables& variables_locales, TablaVariables& variables_args) {
    if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(operando_)){

    	nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $0, %ebx" << endl;
		f << "\tsubl $" << nodoConstante->val_ << ", %ebx" << endl;
		f << "\tmovl %ebx, %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(operando_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
				}

				f << "\tmovl $0, %ebx" << endl;
				f << "\tsubl " << nodoId->id_ << ", %ebx" << endl;
				f << "\tmovl %ebx, %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
				}

				int indice=8+aux*4;
				f << "\tmovl $0, %ebx" << endl;
				f << "\tsubl " << indice << "(%ebp), %ebx" << endl;
				f << "\tmovl %ebx, %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede operar con arrays o punteros");
			}

			int indice=(1+aux)*-4;
			f << "\tmovl $0, %ebx" << endl;
			f << "\tsubl " << indice << "(%ebp), %ebx" << endl;
			f << "\tmovl %ebx, %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(operando_)) {
		f << "\tmovl $0, %ebx" << endl;
		f << "\tsubl %eax, %ebx" << endl;
		f << "\tmovl %ebx, %eax" << endl;
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(operando_)) {
		f << "\tmovl $0, %ebx" << endl;
		f << "\tsubl %eax, %ebx" << endl;
		f << "\tmovl %ebx, %eax" << endl;
	}
}

Tree::NodoFuncion::NodoFuncion(std::string s, TablaVariables& locales): tipo_(s) {
	locales.clearAll();

	f << ".text" << endl;
	f << ".globl " << tipo_ << endl;
	f << ".type " << tipo_ << ", @function" << endl;
	f << s << ":" << endl;
	f << "\tpushl %ebp" << endl;
	f << "\tmovl %esp, %ebp" << endl;

}

Tree::NodoCall::NodoCall(std::string s, vector<string>& pushs){
	std::reverse(pushs.begin(),pushs.end());
	for(vector<string>::iterator it=pushs.begin(); it!=pushs.end(); it++){
		f << *it << endl;
	}
	int n = pushs.size()*4;
	pushs.clear();

	f << "\tmovl %eax, %ebx" << endl;	//Save temporary data
	f << "\tcall " << s << endl;
	f << "\taddl $" << n << ", %esp" << endl;
}

Tree::NodoCallPrintf::NodoCallPrintf(vector<string>& pushs, int cont){
	std::reverse(pushs.begin(),pushs.end());
	for(vector<string>::iterator it=pushs.begin(); it!=pushs.end(); it++){
		f << *it << endl;
	}
	int n = pushs.size()*4 + 4;
	pushs.clear();

	f << "\tpushl $.LC" << cont-1 << endl;

	f << "\tcall printf" << endl;
	f << "\taddl $" << n << ", %esp" << endl;
}

Tree::NodoCallScanf::NodoCallScanf(vector<string>& pushs, int cont){
	std::reverse(pushs.begin(),pushs.end());
	for(vector<string>::iterator it=pushs.begin(); it!=pushs.end(); it++){
		f << *it << endl;
	}
	int n = pushs.size()*4 + 4;
	pushs.clear();

	f << "\tpushl $.LC" << cont-1 << endl;

	f << "\tcall scanf" << endl;
	f << "\taddl $" << n << ", %esp" << endl;
}

Tree::NodoPush::NodoPush(Node* n, vector<string>& v, TablaVariables& globales, TablaVariables& locales, TablaVariables& args){
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(n)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		std::string res = "\tpushl $"+nodoConstante->val_;
		v.push_back(res);
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(n)){
		int aux = locales.getIndex(nodoId->id_);
		int indice;
		if(aux==-1){
			aux = args.getIndex(nodoId->id_);
			if(aux==-1){
				aux = globales.getIndex(nodoId->id_);
				assert(aux != -1);
				v.push_back("\tpushl "+nodoId->id_);

			}else{
				indice=8 + aux*4;
				v.push_back("\tpushl "+std::to_string(indice)+"(%ebp)");
			}

		}else{
			indice=(1+aux)*-4;
			v.push_back("\tpushl "+std::to_string(indice)+"(%ebp)");
		}
	}else if(NodoCall* nodoCall = dynamic_cast<NodoCall*>(n)){
		std::string res = "\tpushl %eax";
		v.push_back(res);
	}else if(NodoExpresion* nodoExpresion = dynamic_cast<NodoExpresion*>(n)){
		std::string res = "\tpushl %eax";
		v.push_back(res);
	}
}

Tree::NodoPushScanf::NodoPushScanf(std::string s, vector<string>& v, TablaVariables& globales, TablaVariables& locales, TablaVariables& args){
	int aux = locales.getIndex(s);
	int indice;
	if(aux==-1){
		aux = args.getIndex(s);
		if(aux==-1){
			aux = globales.getIndex(s);
			assert(aux != -1);
			v.push_back("\tpushl $"+s);

		}else{
			indice=8 + aux*4;
			v.push_back("\tleal "+std::to_string(indice)+"(%ebp), %eax\n\tpushl %eax");
		}

	}else{
		indice=(1+aux)*-4;
		v.push_back("\tleal "+std::to_string(indice)+"(%ebp), %eax\n\tpushl %eax");
	}
}

Tree::NodoFinalFuncion::NodoFinalFuncion(){
	f << "\tleave\n\tret" << endl;
}

Tree::NodoReturn::NodoReturn(Node* expresion, const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args): expresion_(expresion), variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args){
	//Analizar la expresion
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(expresion_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(expresion_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede devolver un array o puntero");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede devolver un array o puntero");
			}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede devolver un array o puntero");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(expresion_)) {
        //No hacer nada, el resultado está en %eax
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(expresion_)) {
        //No hacer nada, el resultado está en %eax
	}

	f << "\tleave\n\tret" << endl;
}

Tree::NodoString::NodoString(string s, int cont){
	f.close();

	ofstream salida;
	ifstream fichero;

	fichero.open("ejer.s");
	salida.open("ejer2.s", ios::app);

	salida << ".LC" << cont << ":\n";
	salida << "\t.string " << s << "\n";
	salida << "\t.section .rodata\n";

	char cadena[80];

	int i=1;
	if(!fichero.fail()){
		fichero.getline(cadena,80,'\n');
	}
	while(!fichero.eof()){
		salida << cadena << endl;
		i++;
		fichero.getline(cadena,80,'\n');
	}

	fichero.close();
	salida.close();
	remove("ejer.s");
	rename("ejer2.s", "ejer.s");
	f.open("ejer.s", ios_base::app);
}

Tree::NodoWhile::NodoWhile(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, Node* lineas, int opcion, stack<int>& etiquetas):variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), expresion_(expresion), lineas_(lineas) {
	int contador_etiqueta=etiquetas.top();
	switch(opcion) {
		case 1: f << "startW" << contador_etiqueta << ":" << endl; break;
		case 2:	//Mover el contenido de la expresion a %eax
		if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(expresion_)){

			nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

			f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
		}else if(NodoId* nodoId = dynamic_cast<NodoId*>(expresion_)){
			//Analizar a qué tabla de variables pertenece nodoId
			int aux=variables_locales_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_args_.getIndex(nodoId->id_);
				if(aux==-1) {
					aux=variables_.getIndex(nodoId->id_);
					assert(aux!=-1);	//Debe ser una variable global en este punto
					//ES UNA VARIABLE GLOBAL
					if(variables_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero como condicion");
					}
					f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
				}
				else {
					//ES UN ARGUMENTO
					if(variables_args_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero como condicion");
					}
					int indice=8+aux*4;
					f << "\tmovl " << indice << "(%ebp), %eax" << endl;
				}
			}
			else {
				//ES UNA VARIABLE LOCAL
				if(variables_locales_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero como condicion");
				}
				int indice=(1+aux)*-4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(expresion_)) {
			//El resultado debe estar en %eax, luego no se hace nada
		}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(expresion_)) {
			//No hacer nada por ahora
		}
		f << "\tcmp $0, %eax" << endl;
		f << "\tjne finalW" << contador_etiqueta << endl; break;
		case 3: f << "\tjmp startW" << contador_etiqueta << endl;
		 		f << "finalW" << contador_etiqueta << ":" << endl;
				etiquetas.pop(); break;
	}
}

Tree::NodoIf::NodoIf(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, Node* lineas_true, Node* lineas_false, int opcion, stack<int>& etiquetas):variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), expresion_(expresion), lineas_true_(lineas_true), lineas_false_(lineas_false) {
	int contador_etiqueta=etiquetas.top();
	switch(opcion) {
		case 1:	//Mover el contenido de la expresion a %eax
		if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(expresion_)){

			nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

			f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
		}else if(NodoId* nodoId = dynamic_cast<NodoId*>(expresion_)){
			//Analizar a qué tabla de variables pertenece nodoId
			int aux=variables_locales_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_args_.getIndex(nodoId->id_);
				if(aux==-1) {
					aux=variables_.getIndex(nodoId->id_);
					assert(aux!=-1);	//Debe ser una variable global en este punto
					//ES UNA VARIABLE GLOBAL
					if(variables_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero como condicion");
					}
					f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
				}
				else {
					//ES UN ARGUMENTO
					if(variables_args_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero como condicion");
					}
					int indice=8+aux*4;
					f << "\tmovl " << indice << "(%ebp), %eax" << endl;
				}
			}
			else {
				//ES UNA VARIABLE LOCAL
				if(variables_locales_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero como condicion");
				}
				int indice=(1+aux)*-4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(expresion_)) {
			//El resultado debe estar en %eax, luego no se hace nada
		}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(expresion_)) {
			//No hacer nada por ahora
		}
		f << "\tcmp $0, %eax" << endl;
		f << "\tjne false" << contador_etiqueta << endl; break;
		case 2: f << "\tjmp finalIF" << contador_etiqueta << endl;
				f << "false" << contador_etiqueta << ":" << endl; break;
		case 3:	f << "finalIF" << contador_etiqueta << ":" << endl;
				etiquetas.pop(); break;
		case 4: f << "false" << contador_etiqueta << ":" << endl;
				etiquetas.pop(); break;
	}
}

Tree::NodoAND::NodoAND(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, int opcion, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), expresion_(expresion) {
	int contador_etiqueta=etiquetas.top();
	switch(opcion) {
		case 1:	//Mover el contenido de la expresion a %eax
		if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(expresion_)){

			nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

			f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
		}else if(NodoId* nodoId = dynamic_cast<NodoId*>(expresion_)){
			//Analizar a qué tabla de variables pertenece nodoId
			int aux=variables_locales_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_args_.getIndex(nodoId->id_);
				if(aux==-1) {
					aux=variables_.getIndex(nodoId->id_);
					assert(aux!=-1);	//Debe ser una variable global en este punto
					//ES UNA VARIABLE GLOBAL
					if(variables_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
					}
					f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
				}
				else {
					//ES UN ARGUMENTO
					if(variables_args_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
					}
					int indice=8+aux*4;
					f << "\tmovl " << indice << "(%ebp), %eax" << endl;
				}
			}
			else {
				//ES UNA VARIABLE LOCAL
				if(variables_locales_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
				}
				int indice=(1+aux)*-4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(expresion_)) {
			//El resultado debe estar en %eax, luego no se hace nada
		}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(expresion_)) {
			//No hacer nada por ahora
		}
		f << "\tcmp $0, %eax" << endl;
		f << "\tje finalAND" << contador_etiqueta << endl; break;
		case 2: f << "finalAND" << contador_etiqueta << ":" << endl;
				etiquetas.pop(); break;
	}
}

Tree::NodoOR::NodoOR(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, int opcion, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), expresion_(expresion) {
	int contador_etiqueta=etiquetas.top();
	switch(opcion) {
		case 1:	//Mover el contenido de la expresion a %eax
		if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(expresion_)){

			nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

			f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
		}else if(NodoId* nodoId = dynamic_cast<NodoId*>(expresion_)){
			//Analizar a qué tabla de variables pertenece nodoId
			int aux=variables_locales_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_args_.getIndex(nodoId->id_);
				if(aux==-1) {
					aux=variables_.getIndex(nodoId->id_);
					assert(aux!=-1);	//Debe ser una variable global en este punto
					//ES UNA VARIABLE GLOBAL
					if(variables_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
					}
					f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
				}
				else {
					//ES UN ARGUMENTO
					if(variables_args_.getType(aux) != "int"){
						yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
					}
					int indice=8+aux*4;
					f << "\tmovl " << indice << "(%ebp), %eax" << endl;
				}
			}
			else {
				//ES UNA VARIABLE LOCAL
				if(variables_locales_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
				}
				int indice=(1+aux)*-4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(expresion_)) {
			//El resultado debe estar en %eax, luego no se hace nada
		}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(expresion_)) {
			//No hacer nada por ahora
		}
		f << "\tcmp $0, %eax" << endl;
		f << "\tjne finalOR" << contador_etiqueta << endl; break;
		case 2: f << "finalOR" << contador_etiqueta << ":" << endl;
				etiquetas.pop(); break;
	}
}

Tree::NodoNOT::NodoNOT(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), expresion_(expresion) {
	int contador_etiqueta=etiquetas.top();
	//Mover el contenido de la expresion a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(expresion_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(expresion_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion logica");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(expresion_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(expresion_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp $0, %eax" << endl;
	f << "\tje finalNOT" << contador_etiqueta << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalNOT" << contador_etiqueta << ":" << endl;
	etiquetas.pop();
}

Tree::NodoEQ::NodoEQ(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), exp1_(exp1), exp2_(exp2) {
	int num_etiqueta=etiquetas.top();
	//Mover el contenido de exp1 a %ebx
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %ebx" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp1_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %ebx" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp1_)) {
		//No hacer nada por ahora
	}
	//Mover el contenido de exp2 a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp2_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp2_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp %ebx, %eax" << endl;
	f << "\tje trueEQ" << num_etiqueta << endl;
	f << "\tmovl $0, %eax" << endl;
	f << "\tjmp finalEQ" << num_etiqueta << endl;
	f << "trueEQ" << num_etiqueta << ":" << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalEQ" << num_etiqueta << ":" << endl;
	etiquetas.pop();
}

Tree::NodoNEQ::NodoNEQ(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), exp1_(exp1), exp2_(exp2) {
	int num_etiqueta=etiquetas.top();
	//Mover el contenido de exp1 a %ebx
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %ebx" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp1_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %ebx" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp1_)) {
		//No hacer nada por ahora
	}
	//Mover el contenido de exp2 a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp2_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp2_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp %ebx, %eax" << endl;
	f << "\tjne trueNEQ" << num_etiqueta << endl;
	f << "\tmovl $0, %eax" << endl;
	f << "\tjmp finalNEQ" << num_etiqueta << endl;
	f << "trueNEQ" << num_etiqueta << ":" << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalNEQ" << num_etiqueta << ":" << endl;
	etiquetas.pop();
}

Tree::NodoGTE::NodoGTE(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), exp1_(exp1), exp2_(exp2) {
	int num_etiqueta=etiquetas.top();
	//Mover el contenido de exp1 a %ebx
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %ebx" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp1_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %ebx" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp1_)) {
		//No hacer nada por ahora
	}
	//Mover el contenido de exp2 a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp2_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp2_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp %ebx, %eax" << endl;
	f << "\tjge trueGTE" << num_etiqueta << endl;
	f << "\tmovl $0, %eax" << endl;
	f << "\tjmp finalGTE" << num_etiqueta << endl;
	f << "trueGTE" << num_etiqueta << ":" << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalGTE" << num_etiqueta << ":" << endl;
	etiquetas.pop();
}

Tree::NodoGT::NodoGT(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), exp1_(exp1), exp2_(exp2) {
	int num_etiqueta=etiquetas.top();
	//Mover el contenido de exp1 a %ebx
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %ebx" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp1_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %ebx" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp1_)) {
		//No hacer nada por ahora
	}
	//Mover el contenido de exp2 a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp2_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp2_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp %ebx, %eax" << endl;
	f << "\tjg trueGT" << num_etiqueta << endl;
	f << "\tmovl $0, %eax" << endl;
	f << "\tjmp finalGT" << num_etiqueta << endl;
	f << "trueGT" << num_etiqueta << ":" << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalGT" << num_etiqueta << ":" << endl;
	etiquetas.pop();
}

Tree::NodoLTE::NodoLTE(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), exp1_(exp1), exp2_(exp2) {
	int num_etiqueta=etiquetas.top();
	//Mover el contenido de exp1 a %ebx
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %ebx" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp1_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %ebx" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp1_)) {
		//No hacer nada por ahora
	}
	//Mover el contenido de exp2 a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp2_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp2_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp %ebx, %eax" << endl;
	f << "\tjle trueLTE" << num_etiqueta << endl;
	f << "\tmovl $0, %eax" << endl;
	f << "\tjmp finalLTE" << num_etiqueta << endl;
	f << "trueLTE" << num_etiqueta << ":" << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalLTE" << num_etiqueta << ":" << endl;
	etiquetas.pop();
}

Tree::NodoLT::NodoLT(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas): variables_(variables), variables_locales_(variables_locales), variables_args_(variables_args), exp1_(exp1), exp2_(exp2) {
	int num_etiqueta=etiquetas.top();
	//Mover el contenido de exp1 a %ebx
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp1_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %ebx" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp1_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %ebx" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %ebx" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp1_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp1_)) {
		//No hacer nada por ahora
	}
	//Mover el contenido de exp2 a %eax
	if(NodoConstante* nodoConstante = dynamic_cast<NodoConstante*>(exp2_)){

		nodoConstante->val_ = checkType(nodoConstante->val_, nodoConstante->type_);

		f << "\tmovl $" << nodoConstante->val_ << ", %eax" << endl;
	}else if(NodoId* nodoId = dynamic_cast<NodoId*>(exp2_)){
		//Analizar a qué tabla de variables pertenece nodoId
		int aux=variables_locales_.getIndex(nodoId->id_);
		if(aux==-1) {
			aux=variables_args_.getIndex(nodoId->id_);
			if(aux==-1) {
				aux=variables_.getIndex(nodoId->id_);
				assert(aux!=-1);	//Debe ser una variable global en este punto
				//ES UNA VARIABLE GLOBAL
				if(variables_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				f << "\tmovl " << nodoId->id_ << ", %eax" << endl;
			}
			else {
				//ES UN ARGUMENTO
				if(variables_args_.getType(aux) != "int"){
					yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
				}
				int indice=8+aux*4;
				f << "\tmovl " << indice << "(%ebp), %eax" << endl;
			}
		}
		else {
			//ES UNA VARIABLE LOCAL
			if(variables_locales_.getType(aux) != "int"){
				yyerror2(to_string(num_linea)+") Error no se puede usar un array o puntero en operacion de comparacion");
			}
			int indice=(1+aux)*-4;
			f << "\tmovl " << indice << "(%ebp), %eax" << endl;
		}
	}else if(NodoExpresion* nodoExp=dynamic_cast<NodoExpresion*>(exp2_)) {
		//El resultado debe estar en %eax, luego no se hace nada
	}else if(NodoMenosUnario* nodoMenosUnario=dynamic_cast<NodoMenosUnario*>(exp2_)) {
		//No hacer nada por ahora
	}
	f << "\tcmp %ebx, %eax" << endl;
	f << "\tjl trueLT" << num_etiqueta << endl;
	f << "\tmovl $0, %eax" << endl;
	f << "\tjmp finalLT" << num_etiqueta << endl;
	f << "trueLT" << num_etiqueta << ":" << endl;
	f << "\tmovl $1, %eax" << endl;
	f << "finalLT" << num_etiqueta << ":" << endl;
	etiquetas.pop();
}
