#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <math.h>

using namespace std;


class Entrada {
    public:
        int Npor = 1;
        int Ndef;
        int Nmig;
        int Ndav;
        int T;
        int J;

        Entrada(ifstream& fitxer){
            fitxer >> Ndef >> Nmig >> Ndav >> T >> J;
        }
};


class Jugador {
    public:
        string nom;
        string pos; //por, def, mig, dav
        int    preu;
        string club;
        int    punts;

        Jugador(const string& n, const string& pos, int pr, const string& c, int p):
            nom(n), pos(pos), preu(pr), club(c), punts(p){}
};


class Equip {
    public:
        string por; //nom del porter
        vector<string> def; //llista amb els noms dels defenses
        vector<string> mig; //llista amb els noms dels migcampistes
        vector<string> dav; //llista amb els noms dels davanters
        int punts;
        int preu;

        Equip():
            por(""), def(vector<string>()), mig(vector<string>()),
            dav(vector<string>()), punts(0), preu(0){}


        void afegir_jugador(Jugador j){
            if(j.pos=="por") por = j.nom;
            else if(j.pos=="def") def.push_back(j.nom);
            else if(j.pos=="mig") mig.push_back(j.nom);
            else if(j.pos=="dav") dav.push_back(j.nom);
            punts += j.punts;
            preu += j.preu;
        }
};



void write_sol(ofstream& fs, Equip E, auto start){
    //Escriu la solució final al fitxer de sortida fs.

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    fs <<fixed<<setprecision(1)<< duration.count() / 1000 << endl;

    fs << "POR: " << E.por << endl;
    
    bool first = true;
    for (string nom : E.def){
        if(first){first=false;fs<<"DEF: "<<nom;}
        else fs<<";"<<nom;
    }
    fs<<endl;
    first = true;
    for (string nom : E.mig){
        if(first){first=false;fs<<"MIG: "<<nom;}
        else fs<<";"<<nom;
    }
    fs<<endl;
    first = true;
    for (string nom : E.dav){
        if(first){first=false;fs<<"DAV: "<<nom;}
        else fs<<";"<<nom;
    }
    fs<<endl;
    fs << "Punts: " << E.punts << endl;
    fs << "Preu: " << E.preu << endl;
}


bool ordre(Jugador j1, Jugador j2){
    //Ordena segons la relació punts^3/preu per cada jugador.
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (pow(j1.punts, 3)/ j1.preu) > (pow(j2.punts, 3) / j2.preu);
}


void llegir_jugadors(ifstream& dades_jugadors,
                     vector<Jugador>& jugadors,
                     int J){
    //Llegeix els jugadors del fitxer 'bench' d'entrada.
    //I defineix la classe Jugador per cadascun d'ells.

    string nom, pos, club, aux2;
    int preu, punts;
    char aux;

    while(not dades_jugadors.eof()){
        getline(dades_jugadors, nom, ';');
        getline(dades_jugadors, pos, ';');
        dades_jugadors >> preu;
        dades_jugadors >> aux;
        getline(dades_jugadors, club, ';');
        dades_jugadors >> punts;
        getline(dades_jugadors, aux2);
        Jugador j = Jugador(nom,pos,preu,club,punts);

        if(nom=="") break;

        if(preu <= J){
            jugadors.push_back(j);
        }
    }
}


void greedy(Entrada entrada, ifstream& dades_jugadors, ofstream& fitxer_sortida, auto start){
    //Aplica l'algorisme greedy per trobar la combinació d'equip més òptima...
    //...afegint els jugadors a la classe Equip.

    int Npor, Ndef, Nmig, Ndav, T, J;
    Npor = entrada.Npor;
    Ndef = entrada.Ndef;
    Nmig = entrada.Nmig;
    Ndav = entrada.Ndav;
    J = entrada.J;
    T = entrada.T;


    Equip E = Equip();

    vector<Jugador> jugadors;
    llegir_jugadors(dades_jugadors, jugadors, J);
    //ordenem els jugadors amb el ratio punts^3 / preu
    sort(jugadors.begin(), jugadors.end(), ordre);


    int preu_restant = T;

    for(uint i=0; i<jugadors.size() and Npor + Ndef + Nmig + Ndav != 0; i++){
        //sempre tindrem que jugadors[i].preu <= J
        Jugador j = jugadors[i];
        if(j.preu <= preu_restant){
            if(j.pos=="por" and Npor>0){ E.afegir_jugador(j); --Npor; preu_restant -= j.preu;}
            if(j.pos=="def" and Ndef>0){ E.afegir_jugador(j); --Ndef; preu_restant -= j.preu;}
            if(j.pos=="mig" and Nmig>0){ E.afegir_jugador(j); --Nmig; preu_restant -= j.preu;}
            if(j.pos=="dav" and Ndav>0){ E.afegir_jugador(j); --Ndav; preu_restant -= j.preu;}            
            
        }
    }
    return write_sol(fitxer_sortida, E, start);
}


int main(int argc, char** argv){
    if (argc != 4) {
        cerr << "Syntax: " << argv[0] << " data_base.txt" <<
                " bench.txt" << " output.txt" << endl;
        exit(1);
    }

    auto start = std::chrono::system_clock::now();

    ifstream dades_jugadors(argv[1]);
    ifstream plantilla(argv[2]);
    ofstream fitxer_sortida(argv[3]);

    Entrada entrada = Entrada(plantilla);
    plantilla.close();


    greedy(entrada,dades_jugadors, fitxer_sortida, start);
}
