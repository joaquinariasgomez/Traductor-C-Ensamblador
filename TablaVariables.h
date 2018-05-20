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
		TablaVariables();

		/*Añade una variable introduciendo su tipo, id y valor.
		Cuando no se quiera iniciar con un valor, se añade null
		por defecto.*/
		void addVariable(string t, string id, string v="null");

		/*elimina una variable introduciendo su id*/
		void eraseVariable(string id);

		/*modifica el valor de una variable introduciendo su id y el nuevo valor*/
		void modifyValue(string id, string v);

		/*modifica el tipo de una variable introduciendo su id y el nuevo tipo*/
		void modifyType(string id, string t);

		/*devuelve el valor de una variable introduciendo su id*/
		string getValue(string id);

		/*devuelve el tipo de una variable introduciendo su id*/
		string getType(string id);

		/*imprime toda la tabla de variables para consultar su estado actual*/
		void printTabla();

		/*devuelve el indice del id que recibe*/
		int getIndex(string id);

		/*reinicia toda la tabla*/
		void clearAll();

		/*da la vuelta a la tabla*/
		void reverseAll();
	private:
		vector<string> tipos;
		vector<string> ids;
		vector<string> valores;
};


#endif
