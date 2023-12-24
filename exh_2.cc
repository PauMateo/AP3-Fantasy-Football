#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <math.h>

using namespace std;


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

        Equip(int Ndef, int Nmig, int Ndav):
            por(""), def(vector<string>(Ndef)), mig(vector<string>(Nmig)),
            dav(vector<string>(Ndav)), punts(0), preu(0){}
/*
        Equip(const Jugador& por, 
              const vector<Jugador>& def,
              const vector<Jugador>& mig,
              const vector<Jugador>& dav,
              const int punts, const int preu):
            por(por), def(def), mig(mig), dav(dav), punts(punts), preu(preu){}
*/
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



vector<Jugador> listpor, listdef, listmig, listdav;
int millors_punts = 0;
int Npor = 1, Ndef, Nmig, Ndav, T, J;
vector<Jugador> bestpor, bestdef, bestmig, bestdav;
vector<bool> Upor, Udef, Umig, Udav;
Equip E = Equip(0, 0, 0);

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

bool ordre3(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (pow(j1.punts, 3)/ pow(j1.preu, 1)) > (pow(j2.punts, 3) / pow(j2.preu, 1));
}


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


bool JugInEquip(Jugador& j, Equip& E){
    if(j.pos=="por"){
        if(j.nom == E.por) return true;
    }
    if(j.pos=="def"){
        for(string nomj2 : E.def) {if(j.nom == nomj2) return true;}
    }
    if(j.pos=="mig"){
        for(string nomj2 : E.mig) {if(j.nom == nomj2) return true;}
    }
    if(j.pos=="dav"){
        for(string nomj2 : E.dav) {if(j.nom == nomj2) return true;}
    }
    return false;
}


bool prune(int porres, int defres, int migres, int davres){  //retorna true si podem la branca del backtracking
    int punts = 0;

    for(int i=0; i<porres; i++){
        if(not JugInEquip(bestpor[i], E)) punts += bestpor[i].punts;
        else i-=1;
    }

    for(int i=0; i<defres; i++){
        if(not JugInEquip(bestdef[i], E)) punts += bestdef[i].punts;
        else i-=1;
    }

    for(int i=0; i<migres; i++){
        if(not JugInEquip(bestmig[i], E)) punts += bestmig[i].punts;
        else i-=1;
    }

    for(int i=0; i<davres; i++){
        if(not JugInEquip(bestdav[i], E)) punts += bestdav[i].punts;
        else i-=1;
    }

    if(punts < millors_punts) return true; //si que podem
    return false; //no podem
}


void llegir_jugadors(ifstream& dades_jugadors,
                     vector<Jugador>& listpor,
                     vector<Jugador>& listdef,
                     vector<Jugador>& listmig,
                     vector<Jugador>& listdav,
                     int J){
    string nom, pos, club, aux2;
    int preu, punts;
    char aux;

    int Nportotal=0, Ndeftotal=0, Nmigtotal=0, Ndavtotal=0;

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
            if(pos=="por") {
                listpor.push_back(j);
                Nportotal += 1;
            }
            if(pos=="def") {

                listdef.push_back(j);
                Ndeftotal += 1;
            }
            if(pos=="mig") {
                listmig.push_back(j);
                Nmigtotal += 1;
            }
            if(pos=="dav") {
                listdav.push_back(j);
                Ndavtotal += 1;
            }
        }
        //cout<<nom<<";"<<pos<<";"<<preu<<";"<<club<<";"<<punts<<endl;
    }

    // Upor = vector<bool>(Nportotal); realment no cal
    Udef = vector<bool>(Ndeftotal);
    Umig = vector<bool>(Ndeftotal);
    Udav = vector<bool>(Ndavtotal);


    std::sort(listpor.begin(), listpor.end(), ordre0);
    std::sort(listdef.begin(), listdef.end(), ordre0);
    std::sort(listmig.begin(), listmig.end(), ordre0);
    std::sort(listdav.begin(), listdav.end(), ordre0);
    for(int i=0; i<Npor; i++) bestpor.push_back(listpor[i]);
    for(int i=0; i<Ndef; i++) bestdef.push_back(listdef[i]);
    for(int i=0; i<Nmig; i++) bestmig.push_back(listmig[i]);
    for(int i=0; i<Ndav; i++) bestdav.push_back(listdav[i]);

    std::sort(listpor.begin(), listpor.end(), ordre3);
    std::sort(listdef.begin(), listdef.end(), ordre3);
    std::sort(listmig.begin(), listmig.end(), ordre3);
    std::sort(listdav.begin(), listdav.end(), ordre3);
}


void exh_search_rec(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T, int J,
            ofstream& fitxer_sortida, auto start){

    if (Npor + Ndef + Nmig + Ndav == 0 and E.punts > millors_punts){
        millors_punts = E.punts;
        cout << E.por << endl;
        return write_sol(fitxer_sortida, E, start);
    }

    if( T<0 or prune(Npor, Ndef, Nmig, Ndav)) return;


    if(Npor > 0){
        for(Jugador j : listpor){
            if(j.preu <= T){
                E.por = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search_rec(E, Npor-1, Ndef, Nmig, Ndav, T-j.preu, J, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}

    if(Ndef > 0){
        for(uint i = E.def.size()-Ndef; i<listdef.size(); i++){
            cout << i << endl;
            Jugador j = listdef[i];
            if(j.preu <= T and not Udef[i]){
                E.def[Ndef-1] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                Udef[i] = true;
                exh_search_rec(E, Npor, Ndef-1, Nmig, Ndav, T-j.preu, J, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
                Udef[i] = false;
    }}}

    if(Nmig > 0){
        for(uint i = E.mig.size()-Nmig; i<listmig.size(); i++){
            Jugador j = listmig[i];
            if(j.preu <= T and not Umig[i]){
                E.mig[Nmig-1] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                Umig[i] = true;
                exh_search_rec(E, Npor, Ndef, Nmig-1, Ndav, T-j.preu, J, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
                Umig[i] = false;
    }}}

    if(Ndav > 0){
        for(uint i = E.dav.size()-Ndav; i<listdav.size(); i++){
            Jugador j = listdav[i];
            if(j.preu <= T and not Udav[i]){
                E.dav[Ndav-1] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                Udav[i] = true;
                exh_search_rec(E, Npor, Ndef, Nmig, Ndav-1, T-j.preu, J, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
                Udav[i] = false;
    }}}
}


void exh_search(int Ndef, int Nmig, int Ndav, int T, int J,
            ofstream& fitxer_sortida, auto start){
    E = Equip(Ndef, Nmig, Ndav);
    return exh_search_rec(E, 1, Ndef, Nmig, Ndav, T, J, fitxer_sortida, start);
}


int main(int argc, char** argv){
    if (argc != 4) {
    cerr << "Syntax: " << argv[0] << " data_base.txt" <<
            " bench.txt" << " output.txt" << endl;
    exit(1);
    }
    auto start = std::chrono::system_clock::now();

    ifstream dades_jugadors(argv[1]);
    ofstream fitxer_sortida(argv[2]);
    ifstream plantilla(argv[3]);

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    plantilla.close();

    llegir_jugadors(dades_jugadors, listpor, listdef, listmig, listdav, J);


    exh_search(Ndef,Nmig,Ndav,T,J, fitxer_sortida, start);
}
