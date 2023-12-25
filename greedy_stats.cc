#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <dirent.h>
#include <cstring>
#include <math.h>

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


int PUNTS_TOTALS = 0;
int punts_avg;
int preu_avg;
int n_jugadors = 0;
int ratio_avg;
int J_, T_;

void write_sol(ofstream& fs, Equip E, string bench){

    fs << bench << " : "<<E.punts<<endl;

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
    fs << endl;
}

void write_sol2(ofstream& fs, Equip E, string bench){

    fs << bench << " -->  PUNTS: "<<E.punts<<";   PREU: "<<E.preu<<endl;
    PUNTS_TOTALS += E.punts;
}

void write_sol2_cout(ofstream& fs, Equip E, string bench){
    cout << bench << " -->  PUNTS: "<<E.punts<<";   PREU: "<<E.preu<<endl;
    PUNTS_TOTALS += E.punts;
}

bool ordre1(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (j1.punts*j1.punts / j2.preu) > (j2.punts*j2.punts / j2.preu);
}

bool ordre2(Jugador j1, Jugador j2){
    if (j1.preu <= j2.preu and j1.punts >= j2.punts) return 1;
    if (j1.preu > j2.preu and j1.punts < j2.punts) return 0;

    //de moment, si no hem triat encara triem en funció del ratio punts^2/preu
    return (j1.punts*j1.punts / j2.preu) > (j2.punts*j2.punts / j2.preu);
}

bool ordre3(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (pow(j1.punts, 3)/ pow(j1.preu, 1)) > (pow(j2.punts, 3) / pow(j2.preu, 1));
}

bool ordre4(Jugador j1, Jugador j2){
    int r1 = j1.punts / j1.preu;
    int r2 = j2.punts / j2.preu;

    if(r1 > r2 and j1.punts >= punts_avg) return true;
    if(r1 < r2 and j2.punts >= punts_avg) return false;

    else return j1.punts > j2.punts;
}


bool ordre5(Jugador j1, Jugador j2){
    if(j1.punts == j2.punts) return j1.preu < j2.preu;
    int difpunts = j1.punts - j2.punts;
    int difpreu = j1.preu - j2.preu;

    return difpunts < difpreu;
}

void llegir_jugadors(ifstream& dades_jugadors,
                     vector<Jugador>& jugadors){
    string nom, pos, club, aux2;
    int preu, punts;
    char aux;

    int punts_total=0, preu_total=0;
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
        jugadors.push_back(j);
    }

    punts_avg = punts_total / n_jugadors;
    preu_avg = preu_total / n_jugadors;
    ratio_avg = punts_avg / preu_avg; 
}


void greedy(vector<Jugador>& jugadors, int Ndef, int Nmig, int Ndav, int T, int J, ifstream& dades_jugadors, ofstream& fitxer_sortida, string bench){

    Equip E = Equip();
    int Npor = 1;
    int preu_restant = T;

    for(uint i=0; i<jugadors.size() and Npor + Ndef + Nmig + Ndav != 0; i++){
        //sempre tindrem que jugadors[i].preu <= J
        Jugador j = jugadors[i];
        if(j.preu <= preu_restant and j.preu <= J){
            if(j.pos=="por" and Npor>0){ E.afegir_jugador(j); --Npor; preu_restant -= j.preu;}
            if(j.pos=="def" and Ndef>0){ E.afegir_jugador(j); --Ndef; preu_restant -= j.preu;}
            if(j.pos=="mig" and Nmig>0){ E.afegir_jugador(j); --Nmig; preu_restant -= j.preu;}
            if(j.pos=="dav" and Ndav>0){ E.afegir_jugador(j); --Ndav; preu_restant -= j.preu;}            
            
        }
    }
    return write_sol(fitxer_sortida, E, bench);
}


int main(int argc, char** argv){
    
    std::string directorio = "new_benchs/";
    DIR* dir;
    struct dirent* entry;

    dir = opendir(directorio.c_str());
    int Ndef, Nmig, Ndav, T, J;

    ifstream dades_jugadors(argv[1]);
    ofstream fitxer_sortida(argv[2]);

    vector<Jugador> jugadors;
    llegir_jugadors(dades_jugadors, jugadors);
    sort(jugadors.begin(), jugadors.end(), ordre3);

    if (dir != nullptr) {
    while ((entry = readdir(dir)) != nullptr) {
        if (std::strstr(entry->d_name, ".txt") != nullptr) {
            std::string filename = directorio + std::string(entry->d_name);
            std::ifstream file(filename);

            if (file.is_open()) {
                file >> Ndef >> Nmig >> Ndav >> T >> J;
                greedy(jugadors,Ndef,Nmig,Ndav,T,J,dades_jugadors, fitxer_sortida, string(entry->d_name));
                file.close();
            } else {
                std::cout << "No se pudo abrir el archivo: " << filename << std::endl;
                }
            }
        }
        closedir(dir);
    } else {
        std::cout << "No se pudo abrir el directorio." << std::endl;
    }
    cout << "PUNTS TOTALS: "<<PUNTS_TOTALS<<endl;
    return 0;
}
