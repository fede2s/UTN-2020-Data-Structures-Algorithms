/****************************************************************
*																*
*				Calculos de casos de CoronaV-19					*
*																*
*	Integrantes:	dos Santos, Federico Augusto				*
*					Fernandez, Facundo Agustin					*
*					Calle Gutierrez, Jordan Ariel				*
*					Fuentes Maya, Albano Agustin				*
*					Navarro, Julian								*
*					Pesce, Gonzalo Ignacio						*
*					Villanueva, Emanuel							*
*																*
*	Fecha: 06/10/2020											*
*																*
*	Objetivo: 	Se podra generar 4 diferentes archivos con el 	*
*				ranking de los 10 paises, de forma descendente	*
*				con respecto a 4 claves: Hisopados, Infectados,	*
*				Recuperados y Fallecidos.						*
*																*
****************************************************************/




#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <string>
#include <cstring>
using namespace std;

#define CANT_MAX_PAISES 200
#define CANT_MAX_MESES 7


typedef char cad20[21];

enum estado {
    HISOPADOS = 0,
    INFECTADOS,
    RECUPERADOS,
    FALLECIDOS,
    ERROR
};

struct {
    unsigned long int cantidades[4];
} typedef acumuladosMensuales;

struct {
    unsigned long int cantidades[4];
} typedef acumuladosTotales;

struct {
    cad20 nombrePais;
    unsigned int cantHabitantes;
} typedef tsPais;

struct {
    cad20 nombrePais;
    unsigned short int mes;
    unsigned short int dia;
    unsigned int cantidades[4];
} typedef tsParDia;

struct {
    cad20 nombrePais;
    unsigned int cantHabitantes;
    acumuladosMensuales estadisticasMensuales[CANT_MAX_MESES];
    acumuladosTotales totales;
} typedef tsCalc;

void enumToStr ( estado enumerado, cad20 nombreArchivo ){
	
	if (enumerado == 0)
		strcpy(nombreArchivo, "Hisopados");
	else if ( enumerado == 1)
		strcpy(nombreArchivo, "Infectados");
	else if ( enumerado == 2)
		strcpy(nombreArchivo, "Recuperados");
	else if ( enumerado == 3)
		strcpy(nombreArchivo, "Fallecidos");
}

estado strToEnum ( cad20 str ){
	
	if (strcmp(str,"0") == 0)
		return HISOPADOS;
	else if (strcmp(str,"1") == 0)
			return INFECTADOS;
		else if (strcmp(str,"2") == 0)
				return RECUPERADOS;
			else if (strcmp(str,"3") == 0)
					return FALLECIDOS;
				else return ERROR;
}

void Abrir (ifstream &arPaises, ifstream &arPartesDiarios){
	
	arPaises.open("Paises.txt");
	arPartesDiarios.open("ParteDiario.txt");
} 

void Cerrar (ifstream &arPaises, ifstream &arPartesDiarios){
	
	arPaises.close();
	arPartesDiarios.close();
} 

void calcularTotales (tsCalc tvrPais[], int& cardPaises){
	
	for(int i=0 ; i < cardPaises ; i++){
		
		for(int j=0 ; j < CANT_MAX_MESES ; j++){
			
			for(int k=0 ; k<4 ; k++){
				
				tvrPais[i].totales.cantidades[k] += tvrPais[i].estadisticasMensuales[j].cantidades[k];
			}
		}
	}
}

void IntCmb (tsCalc &listaPaises1, tsCalc &listaPaises2){
	
	tsCalc aux = listaPaises1;
	listaPaises1 = listaPaises2;
	listaPaises2 = aux;
}

void OrdxBur (tsCalc tvrPais[], int& cardPaises){

	bool ordenado;
	int j = 0;
	
	do {
		j++;
		ordenado = true;
		for (int i = 0; i < (cardPaises - j); i++){

			if( strcmp(tvrPais[i].nombrePais, tvrPais[i+1].nombrePais) > 0 ){
				
				IntCmb(tvrPais[i],tvrPais[i+1]);
				ordenado = false;
			}
		}
	}while(!ordenado);
}

void OrdxBur (tsCalc tvrPais[], estado tipoListado, int& cardPaises){

	bool ordenado;
	int j = 0;
	
	do {
		j++;
		ordenado = true;
		for (int i = 0; i < (cardPaises - j); i++){

			if(tvrPais[i].totales.cantidades[tipoListado] < tvrPais[i+1].totales.cantidades[tipoListado]){
				IntCmb(tvrPais[i],tvrPais[i+1]);
				ordenado = false;
			}
		}
	}while(!ordenado);
}

bool LeerPaises (ifstream &arPaises, tsPais &regPais){
	
		arPaises.get(regPais.nombrePais, 20);
		arPaises.ignore(22);
		arPaises >> regPais.cantHabitantes;
		arPaises.ignore();
		return arPaises.good();
}

void inicRegPais (tsPais& regPais){
	
		strcpy(regPais.nombrePais, "*");
     	regPais.cantHabitantes = 0;
}

void inicRegCalc (tsCalc& regCalc){
	
		strcpy(regCalc.nombrePais, "*");
     	regCalc.cantHabitantes = 0;
        
		for(int i=0 ; i<CANT_MAX_MESES ; i++){
			
			regCalc.estadisticasMensuales[i].cantidades[HISOPADOS] = 0;
			regCalc.estadisticasMensuales[i].cantidades[INFECTADOS] = 0;
			regCalc.estadisticasMensuales[i].cantidades[RECUPERADOS] = 0;
			regCalc.estadisticasMensuales[i].cantidades[FALLECIDOS] = 0;
		}
		
		regCalc.totales.cantidades[HISOPADOS] = 0;
		regCalc.totales.cantidades[INFECTADOS] = 0;
		regCalc.totales.cantidades[RECUPERADOS] = 0;
		regCalc.totales.cantidades[FALLECIDOS] = 0;
}

void inicTablaPaises( tsCalc tvrPais[], tsCalc regCalc ){

	for( int i = 0 ; i < CANT_MAX_PAISES ; i++){
		
        tvrPais[i] = regCalc;
	}
}

void ProcPaises ( ifstream &arPaises, tsCalc tvrPais[], int& cardPaises ){
	
	tsPais regPais;
	tsCalc regCalc;
	
	inicRegPais( regPais );
	inicRegCalc( regCalc );
	inicTablaPaises( tvrPais, regCalc );
	
	while( LeerPaises( arPaises, regPais ) && cardPaises < CANT_MAX_PAISES ){
		
		strcpy( regCalc.nombrePais, regPais.nombrePais );
		regCalc.cantHabitantes = regPais.cantHabitantes;
    	tvrPais[cardPaises] = regCalc;
    	cardPaises++;
	}
	
	tvrPais[cardPaises] = regCalc;
    cardPaises++;
    
	OrdxBur (tvrPais, cardPaises);
}

bool LeerParteDiario ( tsParDia &regPD ){
	
	cin.get(regPD.nombrePais, 20);
	cin.ignore();
	cin >> regPD.mes;
	regPD.mes = regPD.mes - 1;
	cin.ignore();
	cin >> regPD.dia;
	cin.ignore();
	cin >> regPD.cantidades[HISOPADOS];
	cin.ignore();
	cin >> regPD.cantidades[INFECTADOS];
	cin.ignore();
	cin >> regPD.cantidades[RECUPERADOS];
	cin.ignore();
	cin >> regPD.cantidades[FALLECIDOS];
	cin.ignore();

	return cin.good();
	
}

bool  BusquedaBinaria ( tsCalc tvrPais[], cad20 nombrePais, int cardPaises, int& index ){
	
	int primero = 0;
	int ultimo = cardPaises-1;
	int medio;
	 
	while ( primero<=ultimo ){

		medio = ( primero + ultimo ) / 2;

        if (strcmp( tvrPais[medio].nombrePais, nombrePais) == 0 ){
        	
        	index = medio;
			return true;
        } else if ( strcmp( tvrPais[medio].nombrePais, nombrePais ) < 0 ){
        	
        	primero = medio + 1;
        }
        else {
        	
            ultimo = medio - 1;
        }

    }
    
	return false;
}

void insertarParteDiario( tsCalc tvrPais[], tsParDia regPD, int& index){
	
	tvrPais[index].estadisticasMensuales[regPD.mes].cantidades[HISOPADOS]   += regPD.cantidades[HISOPADOS];
	tvrPais[index].estadisticasMensuales[regPD.mes].cantidades[INFECTADOS]  += regPD.cantidades[INFECTADOS];
	tvrPais[index].estadisticasMensuales[regPD.mes].cantidades[RECUPERADOS] += regPD.cantidades[RECUPERADOS];
	tvrPais[index].estadisticasMensuales[regPD.mes].cantidades[FALLECIDOS]  += regPD.cantidades[FALLECIDOS];
}

void ProcParteDiario(ifstream &arPartesDiarios, tsCalc tvrPais[], int& cardPaises){

	tsParDia regPD;
	int index;
	int i = 0;
	
	while( LeerParteDiario( regPD ) ){
	
		if( BusquedaBinaria( tvrPais, regPD.nombrePais, cardPaises, index ) ){
			
			insertarParteDiario( tvrPais, regPD, index );
		}
	}
	insertarParteDiario( tvrPais, regPD, index );
	calcularTotales( tvrPais, cardPaises );
}	

void headerListado (  cad20 nombreListado ){
	
	cout << setw(75) <<fixed<<"Listado de " << nombreListado << endl;
	cout << setw( 5) << left << "Nro. "
			   << setw(14) << left << "Nom. "
			   << setw(17) << right<< "Cant.Hab. "
			    << right<< "--------------------   Cantidades de " << nombreListado << " por mes   --------------------"
			   << setw(12) << right<< "Cant." 
			   << setw(20) << right<< "Porcentajes"	<< endl;
	cout << setw( 5) << left << "Ord. "
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

void fabricarListado ( tsCalc tvrPais[], estado tipo, cad20 nombreListado ){
	
	double totalPais = 0;
	double totalTipo = 0;
	double habitantesTotal = 0;
	double por100Pais = 0;
	double por100Total = 0;
	
	headerListado(  nombreListado );
	
	for(int i=0 ; i<10 ; i++){
		
		totalPais = 0;
		cout	<< setw( 5) << left << i+1
					<< setw(16) << left  << tvrPais[i].nombrePais
					<< setw(12) << right << tvrPais[i].cantHabitantes;
					habitantesTotal += tvrPais[i].cantHabitantes;
						
					for(int j=0 ; j<CANT_MAX_MESES ; j++){
						cout << setw(11) << setprecision(0) << right << tvrPais[i].estadisticasMensuales[j].cantidades[tipo];
						totalPais += tvrPais[i].estadisticasMensuales[j].cantidades[tipo];
					}
					
					totalTipo += totalPais;	
					cout << setw(15) << setprecision(0) << right << totalPais;
					por100Pais = (totalPais * 100) / tvrPais[i].cantHabitantes;
					cout << setw( 14) << setprecision(6)<< right << por100Pais << " %" << endl;				 
	}
	
	cout << endl << endl;
	cout << "Cantidad total de " << nombreListado << " hasta la fecha actual: " << setprecision(0) << totalTipo << setprecision(5) << endl;
	por100Total = (totalTipo * 100) / habitantesTotal;
	cout << "Porcentaje de "	   << nombreListado << ": " << por100Total << " %" << endl;

}
	

void Listar( tsCalc tvrPais[], int& cardPaises ){
	
	estado tipoListado;
	cad20 nombreListado;
	cad20 nombreArchivo;
	ofstream Paisesitos;
	
	
   	for(int i = 3 ; i >= 0 ; i--){
   		
   		tipoListado = (estado)i;
   		strcpy(nombreListado, "");
   		strcpy(nombreArchivo, "");
   		
   		enumToStr( tipoListado, nombreListado );
   		strcat(nombreArchivo, "Listado");
   		strcat(nombreArchivo, nombreListado);
   		strcat(nombreArchivo, ".txt");
   		
		freopen(nombreArchivo,"w",stdout);
		OrdxBur(tvrPais, tipoListado, cardPaises);
		fabricarListado(tvrPais, tipoListado, nombreListado);
		freopen("CON","w",stdout);
		cout << endl << "Se genero " << nombreArchivo << " con exito." << endl << endl;
	   }		
}
	
int main(){

    ifstream arPaises, arPartesDiarios;
    tsCalc tvrPais[CANT_MAX_PAISES];
    int cardPaises = 0;
    
	Abrir( arPaises, arPartesDiarios );
	freopen( "ParteDiario.Txt", "r", stdin );
	ProcPaises( arPaises, tvrPais, cardPaises ); 
	ProcParteDiario( arPartesDiarios, tvrPais, cardPaises ); 
    Listar( tvrPais, cardPaises );
    freopen( "CON", "r", stdin );
    Cerrar( arPaises, arPartesDiarios );
	return 0;
}
