#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <math.h>
#include <random>

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
vector<Jugador> jugadors;
//vector<Jugador> porcand, defcand, migcand, davcand;   -> fer-ho amb variables globals o fer-les locals dins de construir_sol (d'aquesta manera podem fer diferents randomized solutions amb diferents n's a diferents temps de l'algoritme)
int millors_punts = 0;
int maxPuntspor=0, maxPuntsdef=0, maxPuntsmig=0, maxPuntsdav=0;
vector<bool> Upor, Udef, Umig, Udav;

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
            jugadors.push_back(j);


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

    // Upor = vector<bool>(Nportotal); realment no cal
    Udef = vector<bool>(Ndeftotal);
    Umig = vector<bool>(Ndeftotal);
    Udav = vector<bool>(Ndavtotal);

    std::sort(jugadors.begin(), jugadors.end(), ordre3);

    std::sort(listpor.begin(), listpor.end(), ordre3);
    std::sort(listdef.begin(), listdef.end(), ordre3);
    std::sort(listmig.begin(), listmig.end(), ordre3);
    std::sort(listdav.begin(), listdav.end(), ordre3);

}


/*
void construir_sol(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T){
    int n = 10;
    vector<Jugador> porcand(listpor.begin(), listpor.begin()+n);
    vector<Jugador> defcand(listdef.begin(), listdef.begin()+n);
    vector<Jugador> migcand(listmig.begin(), listmig.begin()+n);
    vector<Jugador> davcand(listdav.begin(), listdav.begin()+n);

    bool done = false;
    int index = rand() % listpor.size(); // pick a random index
    while(not done){
        if(T>porcand[index].preu){
            E.por = porcand[index].nom;
            E.punts += porcand[index].punts;
            T -= porcand[index].preu;
            done = true;
        }
        else index += 1;
    }

    done = false;
    index = rand() % listpor.size(); // pick a random index
    while(Ndef>0){
        if(T>defcand[index].preu){
            E.def = defcand[index].nom;
            E.punts += defcand[index].punts;
            T -= defcand[index].preu;
            Ndef -= 1;
        }
        else index += 1;
    }

    done = false;
    index = rand() % listpor.size(); // pick a random index
    while(Nmig>0){
        if(T>porcand[index].preu){
            E.por = porcand[index].nom;
            E.punts += porcand[index].punts;
            T -= porcand[index].preu;
        }
        else index += 1;
    }

    done = false;
    index = rand() % listpor.size(); // pick a random index
    while(Ndav>0){
        if(T>porcand[index].preu){
            E.por = porcand[index].nom;
            E.punts += porcand[index].punts;
            T -= porcand[index].preu;
        }
        else index += 1;
    }
}
*/

void construir_sol_greedy(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T){
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
}


void mh(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T,
            ofstream& fitxer_sortida, auto start)

    while(true){
        //construïr solució inicial :
        construir_sol_greedy(E, Npor, Ndef, Nmig, Ndav, T);
        //simulated annealing
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
}
