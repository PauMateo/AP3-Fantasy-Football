#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

using namespace std;


class Entrada {   //Plantilla??
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
        string pos;
        int    preu;
        string club;
        int    punts;

        Jugador(const string& n, const string& pos, int pr, const string& c, int p):
            nom(n), pos(pos), preu(pr), club(c), punts(p){}
};


class Equip {
    public:
        string por;
        vector<string> def;
        vector<string> mig;
        vector<string> dav;
        int punts;
        int preu;

        Equip():
            por(""), def(vector<string>()), mig(vector<string>()),
            dav(vector<string>()), punts(0), preu(0){}

        //controlar també el nombre dels jugadors des de la classe? 
        //-> return false quan ja no pots posar més defenses, etc.
        void afegir_jugador(Jugador j){
            if(j.pos=="por") por = j.nom;
            else if(j.pos=="def") def.push_back(j.nom);
            else if(j.pos=="mig") mig.push_back(j.nom);
            else if(j.pos=="dav") dav.push_back(j.nom);
            punts += j.punts;
            preu += j.preu;
        }

        void escriu_jugadors(string pos, ofstream& fs){
            bool first = true;
            vector<string> *x = nullptr;
            if (pos=="POR"){fs << "POR: " << por;}
            else{
                if (pos == "DEF") x = &def;
                if (pos == "MIG") x = &mig;
                if (pos == "DAV") x = &dav;
                fs << pos;
                for(string n : *x){
                        if(first) {fs << n ; first=false;}
                        else fs << n <<";" ;
            }}
            fs << endl;
        }
};



void write_sol(ofstream& fs, Equip E, auto start){

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float,std::milli> duration = end - start;

    fs <<setprecision(1)<< duration.count() / 1000 << fixed << endl;

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

bool ordre0(Jugador j1, Jugador j2){
    if(j1.punts == j2.punts) return j1.preu < j2.preu;
    return (j1.punts) > (j2.punts);
}

bool ordre1(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (j1.punts / j2.preu) > (j2.punts / j2.preu);
}

bool ordre2(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (j1.punts*j1.punts/ j2.preu) > (j2.punts*j2.punts / j2.preu);
}


void llegir_jugadors(ifstream& dades_jugadors,
                     vector<Jugador>& jugadors,
                     int J){
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

    //auto& [Npor, Ndef, Nmig, Ndav, T, J] = entrada;
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
    sort(jugadors.begin(), jugadors.end(), ordre0);

    int preu_restant = T;

    for(uint i=0; i<jugadors.size() and Npor + Ndef + Nmig + Ndav != 0; i++){
        //sempre tindrem que jugadors[i].preu <= J
        Jugador j = jugadors[i];
        if(j.preu <= preu_restant){
            if(j.pos=="por" and Npor>0){ E.afegir_jugador(j); Npor -= 1;}
            if(j.pos=="def" and Ndef>0){ E.afegir_jugador(j); Ndef -= 1;}
            if(j.pos=="mig" and Nmig>0){ E.afegir_jugador(j); Nmig -= 1;}
            if(j.pos=="dav" and Ndav>0){ E.afegir_jugador(j); Ndav -= 1;}            
            preu_restant -= j.preu;
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
