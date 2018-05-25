#ifndef TABLAVARIABLES_H
#define TABLAVARIABLES_H

#include <string>
#include <vector>

/* Esta clase sirve para mantener una tabla de variables, guardando
su tipo, su identificador y su valor. Todos estos datos se guardan
con tipo string, por lo que si se quiere hacer calculos con ellos
se deberá utilizar el tipo de esta variable para castear. */

using namespace std;

class TablaVariables{
	public:
		TablaVariables(){}

		/*Añade una variable introduciendo su tipo, id y valor.
		Cuando no se quiera iniciar con un valor, se añade null
		por defecto.*/
		void addVariable(string t, string id, vector<string> v);

		/*devuelve el indice del id que recibe*/
		int getIndex(string id);

		/*devuelve el tipo del indice que recibe*/
		string getType(int i);

		/*devuelve el id del indice que recibe*/
		string getId(int i);

		/*devuelve el vector de dimensiones*/
		vector<string> getDimensions(int i);

		/*reinicia toda la tabla*/
		void clearAll();

	private:
		vector<string> tipos;
		vector<string> ids;
		vector<vector<string> > valores;
};


#endif
