/*
	Nombre del Programa tpV2.cpp
	Autores del trabajo: 	dos Santos, Federico
							Fernández, Facundo Agustín
							Fuentes Maya, Albano
	Curso: k1052
	Dia: Martes
	Turno: Noche
	Grupo: 3
	Fecha: 31/11/2020
	Comentario: Usamos para probar las muestras que se dieron en la resolución del parcial 2. Compilamos con DevC++ y funcionó en sistemas 64 bits.
	
*/

#include <iostream>
#include <fstream>
#include <string.h>
#include <string>
#include <iomanip>


using namespace std;

#define CANT_MAX_MESES 7

enum tipo {
    HISOPADOS = 0,
    INFECTADOS,
    RECUPERADOS,
    FALLECIDOS
};


struct tsParDia {
    char nombrePais[21];
    unsigned short mes;
    unsigned short dia;
    unsigned short cantidades[4];
};//para leer PartesDiarios.Dat

typedef char cad20[21];
typedef cad20 tclv;
typedef char cad10[11];

struct tsPais{
    cad20 nombrePais;
    cad10 continente;
    unsigned int cantHabitantes;
};//para leer Paises.Dat

struct tInfo{
	cad20 nombrePais;
    unsigned int cantidadHabitantes;
    unsigned long int matMesesCantidades[CANT_MAX_MESES][4];
    unsigned long int totales[4];
};//Información que contendrá el nodo

typedef struct tNodo* tLista;	//defino el tipo de dato para el puntero empotrado
struct tNodo{
    tInfo Info;
    tLista Sgte;
};//Nodo de la única lista que usaremos en el tp

void Abrir(ifstream&,ifstream&);
void Cerrar(ifstream&,ifstream&);
void leerPais(ifstream&,tsPais&);
void leerParteDiario(ifstream&,tsParDia&);
void procPaises(ifstream&,tLista&);
void procParteDiario(ifstream&,tLista&);
void inicValoresParDia(tLista&);
void insertarParteDiario(tLista&, tsParDia);
bool compare(char[],char[]);
void ListadoPaises(tLista, ofstream&);
void headerListado(string,ofstream&);
void ListadoCasos(tLista&);
void OrdXBur(tLista&,int);
void IntCmb(tLista&, tLista&);
void fabricarListado(tLista &lista,int,string, ofstream&);
void InsertaNodo(tLista &Lista, tInfo valor);
void InsertaEnMedio(tLista &Lista, tInfo valor);
tLista ExisteNodo(tLista &Lista, tLista &pAnt, tclv clv);
void InsertaInicio(tLista &Lista, tInfo Info);

int main(void){
	
    ifstream arParteDiarioBIN, arPaisesBIN;
    tLista lista = NULL;
    ofstream arPaises("ListadoPaises.txt");

    Abrir(arPaisesBIN,arParteDiarioBIN);
    
    procPaises(arPaisesBIN,lista);
    procParteDiario(arParteDiarioBIN,lista);

    ListadoPaises(lista, arPaises);
    ListadoCasos(lista);

    Cerrar(arPaisesBIN,arParteDiarioBIN);

    return 0;
}

void Abrir(ifstream &arPaises, ifstream &arParDia){
    arPaises.open("Paises.Dat", ios::in|ios::binary);
    arParDia.open("ParteDiario.Dat", ios::in|ios::binary);
}

void Cerrar(ifstream &arPaises, ifstream &arParDia){
    arPaises.close();
    arParDia.close();
}

void procPaises(ifstream &arPaises, tLista& lista){
    
    tsPais rPais;
    tInfo rInfo;
    
	//Inicializo los campos de los registros que no leo en este modulo
	for(int cantidad=0;cantidad<4;cantidad++){
		for(int mes=0; mes<CANT_MAX_MESES; mes++){
			rInfo.matMesesCantidades[mes][cantidad]=0;
		}
		rInfo.totales[cantidad]=0;
	}
			
    arPaises.read((char *) &rPais, sizeof rPais);
    
	while(!arPaises.eof()){
        strcpy(rInfo.nombrePais,rPais.nombrePais);
        rInfo.cantidadHabitantes= rPais.cantHabitantes;   
        InsertaNodo(lista, rInfo);
        arPaises.read((char *) &rPais, sizeof rPais);
    }
}

void procParteDiario(ifstream &arParDia, tLista& lista){

    tsParDia registro;
    tLista nodoModificar;
    tLista pAnt = lista;
    
	arParDia.read((char*) &registro, sizeof registro); 
   
    while(!arParDia.eof()){
        
		pAnt=lista;
        
		//Busco la direccion del nodo según la clave nombrePais
        nodoModificar = ExisteNodo(lista, pAnt, registro.nombrePais);
       		        
        //Modifico los campos que no usé en procPaises
		nodoModificar->Info.matMesesCantidades[registro.mes-1][0] +=  registro.cantidades[0];
		nodoModificar->Info.matMesesCantidades[registro.mes-1][1] +=  registro.cantidades[1];
		nodoModificar->Info.matMesesCantidades[registro.mes-1][2] +=  registro.cantidades[2];
		nodoModificar->Info.matMesesCantidades[registro.mes-1][3] +=  registro.cantidades[3];

		arParDia.read((char*) &registro, sizeof registro);
    }
}


void ListadoPaises(tLista lista, ofstream &arPaises){
    
    arPaises<<setw(60)<<left<<"Listado ordenado por paises y meses de casos mensuales"<<endl<<endl;
    while(lista != NULL){
        arPaises<<"================================================================"<<endl<<endl;
        arPaises<<"Pais: "<<lista->Info.nombrePais<<setw(30)<<"Cant. Habit.: "
            <<lista->Info.cantidadHabitantes<<endl<<endl;
        arPaises<<"Totales por casos de Enero a Julio"<<endl<<endl;
        arPaises<< setw(15) << right << "Hisop."
            << setw(15) << right << "Infec."
            << setw(15) << right << "Recup."
            << setw(15) << right << "Fallec." << endl;

        for(int cantidad=0; cantidad<4 ; cantidad++){
            for(int mes=0; mes<CANT_MAX_MESES; mes++){
                lista->Info.totales[cantidad] += lista->Info.matMesesCantidades[mes][cantidad]; 
            }
        }

        arPaises<< setw(15) << right << lista->Info.totales[HISOPADOS]
            << setw(15) << right << lista->Info.totales[INFECTADOS]
            << setw(15) << right << lista->Info.totales[RECUPERADOS]
            << setw(15) << right << lista->Info.totales[FALLECIDOS]<<endl<<endl;;
        arPaises<<"Totales por meses"<<endl<<endl;
        arPaises<< setw(3) << "Mes"
            << setw(15) << "Hisop."
            << setw(15) << "Infec."
            << setw(15) << "Recup."
            << setw(15) << "Fallec." << endl;
        for(int mes=0; mes<CANT_MAX_MESES ; mes++){
            arPaises << setw(3) << mes+1
                << setw(15) << lista->Info.matMesesCantidades[mes][HISOPADOS]
                << setw(15) << lista->Info.matMesesCantidades[mes][INFECTADOS]
                << setw(15) << lista->Info.matMesesCantidades[mes][RECUPERADOS]
                << setw(15) << lista->Info.matMesesCantidades[mes][FALLECIDOS] << endl << endl;  
            
        }
        arPaises<<"================================================================"<<endl<<endl;


        lista = lista->Sgte;
    }

    cout<<"ListadoPaises.txt ha sido generado correctamente!"<<endl<<endl;

}

void headerListado(string nombreListado, ofstream& arListado){
    arListado << setw(75) <<fixed<<"Listado de " << nombreListado << endl;
	arListado << setw( 5) << left << "Nro. "
			   << setw(14) << left << "Nom. "
			   << setw(17) << right<< "Cant.Hab. "
			    << right<< "--------------------   Cantidades de " << nombreListado << " por mes   --------------------"
			   << setw(12) << right<< "Cant." 
			   << setw(20) << right<< "Porcentajes"	<< endl;
	arListado << setw( 5) << left << "Ord. "
			   << setw(14) << left << "Pais"
			   << setw(14) << left << " "
			   << setw(11) << right << "Ene "
			   << setw(11) << right << "Feb "
			   << setw(11) << right << "Mar "
			   << setw(11) << right << "Abr "
			   << setw(11) << right << "May "
			   << setw(11) << right << "Jun "
			   << setw( 11) << right << "Jul "
			   << setw(15) << right << "Tot. " << endl;
}

void ListadoCasos(tLista& lista){
    string nombreListado;
    tLista aux;

    for(int i=0;i<4;i++){
        string nombreArchivo = "Listado";
        if(i == 0){
            nombreListado = "Hisopados";
        }else if (i == 1){
            nombreListado = "Infectados";
        }else if (i == 2){
            nombreListado = "Recuperados";
        }else{
            nombreListado = "Fallecidos";
        }
        
        OrdXBur(lista,i);
        nombreArchivo += nombreListado + ".txt";
        ofstream arListado;
        arListado.open(nombreArchivo.c_str());
        
        fabricarListado(lista,i,nombreListado,arListado);
        cout<<nombreArchivo<<" ha sido generado exitosamente"<<endl<<endl;

        arListado.close();
    }
    
    while(lista!=NULL){
    	aux=lista->Sgte;
    	delete lista;
    	lista=aux;
    }
}

void fabricarListado(tLista &lista, int tipo, string nombreListado, ofstream& arListado){
    double totalPais = 0;
	double totalTipo = 0;
	double habitantesTotal = 0;
	double por100Pais = 0;
	double por100Total = 0;
	tLista pActual=lista;			
	
    headerListado(nombreListado,arListado);
	
	for(int i=0 ; i<10 ; i++){
		
		totalPais = 0;
		arListado	<< setw( 5) << left << i+1
					<< setw(16) << left  << pActual->Info.nombrePais
					<< setw(12) << right << pActual->Info.cantidadHabitantes;
					habitantesTotal += pActual->Info.cantidadHabitantes;
						
					for(int mes=0 ; mes<CANT_MAX_MESES ; mes++){
						arListado << setw(11) << setprecision(0) << right << pActual->Info.matMesesCantidades[mes][tipo];
						totalPais += pActual->Info.matMesesCantidades[mes][tipo];
					}
					
					totalTipo += totalPais;	
					arListado << setw(15) << setprecision(0) << right << totalPais;
					por100Pais = (totalPais * 100) / pActual->Info.cantidadHabitantes;
					arListado << setw( 14) << setprecision(6)<< right << por100Pais << " %" << endl;
		pActual=pActual->Sgte;
			 
	}
	
	arListado << endl << endl;
	arListado << "Cantidad total de " << nombreListado << " hasta la fecha actual: " << setprecision(0) << totalTipo << setprecision(5) << endl;
	por100Total = (totalTipo * 100) / habitantesTotal;
	arListado << "Porcentaje de "	   << nombreListado << ": " << por100Total << " %" << endl;
	
}

//------------Funciones vistas en clase y modificadas para el TP------------------------
//----------------------------------Listas----------------------------------------------

void InsertaNodo(tLista &Lista, tInfo Info) {

  if (Lista == NULL || (strcmp(Info.nombrePais, Lista->Info.nombrePais) < 0) )
    InsertaInicio(Lista, Info);
  else
    InsertaEnMedio(Lista, Info);
} //InsertaNodo

void InsertaInicio(tLista &Lista, tInfo Info) {
  tLista pNodo;
                                           
  pNodo = new tNodo;                                     // 1:
  pNodo->Info = Info;                                   // 2:
  pNodo->Sgte = Lista;                                   // 3:
  Lista = pNodo;                                         // 4:
} //InsertaInicio

void InsertaEnMedio(tLista &Lista, tInfo Info) {
  tLista pAct, 
         pNodo;

  pNodo = new tNodo;                                     // 5:
  pNodo->Info = Info;                                   // 6:
  pAct = Lista;                                          // 7:
  while (pAct->Sgte != NULL && ( strcmp(Info.nombrePais, pAct->Sgte->Info.nombrePais)>0 ))
    pAct = pAct->Sgte;                                   // 8:
  pNodo->Sgte = pAct->Sgte;                              // 9:
  pAct->Sgte = pNodo;                                     //10:
} //InsertaEnMedio

//tclv es un cad20
tLista ExisteNodo(tLista &Lista, tLista &pAnt, tclv clv) {
  tLista pAct;

  pAct = Lista;                                   // 1:
  pAnt = NULL;                                    // 2:
  while ( (pAct != NULL) && (strcmp(clv, pAct->Info.nombrePais) != 0 ) ) {
    pAnt = pAct;                                  // 3:
    pAct = pAct->Sgte;                            // 4:
  }
  
  if ( (pAct != NULL ) &&  ( strcmp(clv, pAct->Info.nombrePais)==0 ) )
    return pAct;                                  // 5:
  else
    return NULL;                                  // 6:
} //ExisteNodo

//-----------------------------Ordenar por burbujeo------------------------------------
void OrdXBur(tLista& lista, int tipoListado){
	bool ordenado;
    tLista i ;
	tLista k = NULL;    
        
    	do{		
    		ordenado=true;
    		for(i=lista ; i->Sgte != k; i=i->Sgte){
    			
    			if(i->Info.totales[tipoListado] < i->Sgte->Info.totales[tipoListado]){
    				IntCmb(i, i->Sgte);
    				ordenado=false;
    			}
    		}
    		k=i;
    		    		
    	}while(!ordenado);
}

void IntCmb(tNodo*& Nodo1, tNodo*& Nodo2){
    
    tInfo infoAux= Nodo1->Info;
    Nodo1->Info= Nodo2->Info;
    Nodo2->Info= infoAux;
    
} 

