#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stack>
#include "TablaVariables.h"

using namespace std;
extern ofstream f;
extern int num_linea;

namespace Tree{
	class Node{
	public:
		Node(void){}
		virtual void dummy(){}
	};

	class NodoRaiz : public Node{
		std::vector<Node*> pns_;
	public:
		NodoRaiz() {}
		void addNodo(Node* n){pns_.push_back(n);}
	};

	class NodoId : public Node{
	public:
		string id_;
		bool es_direccion;  

		NodoId(string s, bool b=false): id_(s), es_direccion(b) {}
	};

	class NodoConstante : public Node{
	public:
		std::string val_;
		std::string type_;

		NodoConstante(std::string s, std::string t): val_(s), type_(t) {}
	};

	class NodoTipo : public Node{
	public:
		std::string tipo_;

		NodoTipo(std::string s): tipo_(s) {}
	};

	class NodoExpresion : public Node{
	public:
		Node* n1_;
		char operador_;
		Node* n2_;
		std::string resultado_;

		NodoExpresion(Node* n1, char op, Node* n2): n1_(n1), operador_(op), n2_(n2) {}
		void operar(TablaVariables& variables, TablaVariables& variables_locales, TablaVariables& variables_args);
	};

	class NodoMenosUnario: public Node{
        public:
			Node* operando_;
            std::string resultado_;

            NodoMenosUnario(Node* operando): operando_(operando) {}
            void operar(TablaVariables& variables, TablaVariables& variables_locales, TablaVariables& variables_args);
    };

	class NodoAsignacion : public Node{
	public:
		NodoId* n1_;
		Node* n2_;

		NodoAsignacion(NodoId* n1, Node* n2): n1_(n1), n2_(n2) {}

		void asignar(TablaVariables& tab1, TablaVariables& tab2, TablaVariables& tab3);
	};

	class NodoDefinicion : public Node{
	public:
		NodoTipo* n1_;
		NodoId* n2_;
		Node* n3_;

		NodoDefinicion(NodoTipo* n1, NodoId* n2, Node* n3=NULL): n1_(n1), n2_(n2), n3_(n3) {}

		void declarar(TablaVariables& tab, bool b, vector<string> v);
		void instanciar(TablaVariables& tab1, TablaVariables& tab2, TablaVariables& tab3, vector<string> v);
	};

	class NodoFuncion : public Node{
	public:
		std::string tipo_;

		NodoFuncion(std::string s, TablaVariables& locales);

	};

	class NodoCall : public Node{
	public:
		NodoCall(std::string s, vector<string>& v);
	};

	class NodoPush : public Node{
	public:
		NodoPush(Node* n, vector<string>& v, TablaVariables& tab1, TablaVariables& tab2, TablaVariables& tab3);
	};

	class NodoPushScanf : public Node{
	public:
		NodoPushScanf(string s, vector<string>& v, TablaVariables& tab1, TablaVariables& tab2, TablaVariables& tab3);
	};

	class NodoFinalFuncion : public Node{
	public:
		NodoFinalFuncion();
	};

	class NodoReturn : public Node{
	public:
		Node* expresion_;
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;

		NodoReturn(Node* expresion, const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args);
	};

	class NodoWhile : public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* expresion_;
		Node* lineas_;

		NodoWhile(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, Node* lineas, int opcion, stack<int>& etiquetas);
	};

	class NodoIf : public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* expresion_;
		Node* lineas_true_;
		Node* lineas_false_;

		NodoIf(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, Node* lineas_true, Node* lineas_false, int opcion, stack<int>& etiquetas);
	};

	class NodoAND: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* expresion_;

		NodoAND(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, int opcion, stack<int>& etiquetas);
	};

	class NodoOR: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* expresion_;

		NodoOR(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, int opcion, stack<int>& etiquetas);
	};

	class NodoNOT: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* expresion_;

		NodoNOT(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* expresion, stack<int>& etiquetas);
	};

	class NodoEQ: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* exp1_;
		Node* exp2_;

		NodoEQ(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas);
	};

	class NodoNEQ: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* exp1_;
		Node* exp2_;

		NodoNEQ(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas);
	};

	class NodoGTE: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* exp1_;
		Node* exp2_;

		NodoGTE(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas);
	};

	class NodoGT: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* exp1_;
		Node* exp2_;

		NodoGT(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas);
	};

	class NodoLTE: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* exp1_;
		Node* exp2_;

		NodoLTE(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas);
	};

	class NodoLT: public Node{
	public:
		TablaVariables variables_;
		TablaVariables variables_locales_;
		TablaVariables variables_args_;
		Node* exp1_;
		Node* exp2_;

		NodoLT(const TablaVariables& variables, const TablaVariables& variables_locales, const TablaVariables& variables_args, Node* exp1, Node* exp2, stack<int>& etiquetas);
	};

	class NodoCallPrintf : public Node{
	public:
		NodoCallPrintf(vector<string>& v, int cont);
	};

	class NodoCallScanf : public Node{
	public:
		NodoCallScanf(vector<string>& v, int cont);
	};

	class NodoString : public Node{
	public:
		NodoString(string s, int cont);
	};

};

#endif
