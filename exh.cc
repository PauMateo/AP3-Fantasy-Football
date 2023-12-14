#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

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
Equip E_millor = Equip(0, 0, 0);
int maxPuntspor=0, maxPuntsdef=0, maxPuntsmig=0, maxPuntsdav=0;
vector<bool> Upor, Udef, Umig, Udav;

bool ordre1(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (j1.punts*j1.punts / j2.preu) > (j2.punts*j2.punts / j2.preu);
}

bool ordre2(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (j1.punts*0.7/ j2.preu*0.3) > (j2.punts*0.7 / j2.preu*0.3);
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
                maxPuntspor = max(maxPuntspor, j.punts);
                listpor.push_back(j);
                Nportotal += 1;
            }
            if(pos=="def") {
                maxPuntsdef = max(maxPuntsdef, j.punts);
                listdef.push_back(j);
                Ndeftotal += 1;
            }
            if(pos=="mig") {
                maxPuntsmig = max(maxPuntsmig, j.punts);
                listmig.push_back(j);
                Nmigtotal += 1;
            }
            if(pos=="dav") {
                maxPuntsdav = max(maxPuntsdav, j.punts);
                listdav.push_back(j);
                Ndavtotal += 1;
            }
        }
        //cout<<nom<<";"<<pos<<";"<<preu<<";"<<club<<";"<<punts<<endl;
    }

    Upor = vector<bool>(Nportotal);
    Udef = vector<bool>(Ndeftotal);
    Umig = vector<bool>(Ndeftotal);
    Udav = vector<bool>(Ndavtotal);

    std::sort(listpor.begin(), listpor.end(), ordre1);
    std::sort(listdef.begin(), listdef.end(), ordre1);
    std::sort(listmig.begin(), listmig.end(), ordre1);
    std::sort(listdav.begin(), listdav.end(), ordre1);

}


void exh_search_rec(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T, int J,
            ofstream& fitxer_sortida, auto start){
    if (Npor + Ndef + Nmig + Ndav == 0 and E.punts > E_millor.punts){
        E_millor = E;
        cout << E.por << endl;
        return write_sol(fitxer_sortida, E_millor, start);
    }

    if( T<0 or (E.punts + Npor*maxPuntspor + Ndef*maxPuntsdef + Nmig*maxPuntsmig + Ndav*maxPuntsdav < E_millor.punts)) return;


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
        for(uint i=listdef.size()-Ndef; i<listdef.size(); i++){
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
        for(uint i=listmig.size()-Nmig; i<listmig.size(); i++){
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
        for(uint i=listdav.size()-Ndav; i<listdav.size(); i++){
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
    Equip E = Equip(Ndef, Nmig, Ndav);
    E_millor = Equip(Ndef, Nmig, Ndav);
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
    int Ndef, Nmig, Ndav, T, J;

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    plantilla.close();

    llegir_jugadors(dades_jugadors, listpor, listdef, listmig, listdav, J);


    exh_search(Ndef,Nmig,Ndav,T,J, fitxer_sortida, start);

    cout<< E_millor.por << endl;
}
