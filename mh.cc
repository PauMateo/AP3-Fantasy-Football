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

        void afegir_jugador(Jugador j){
            if(j.pos=="por") por = j.nom;
            else if(j.pos=="def") def.push_back(j.nom);
            else if(j.pos=="mig") mig.push_back(j.nom);
            else if(j.pos=="dav") dav.push_back(j.nom);
            punts += j.punts;
            preu += j.preu;
        }

        void canviar_jugador(Jugador j, Jugador j2){
            if (j2.pos=="por") por = j2.nom;
            else if(j2.pos=="def"){
                for (uint i=0; i < def.size(); i++){
                    if (def[i] == j.nom) def[i] = j2.nom;
                }
            }
            else if(j2.pos=="mig"){
                for (uint i=0; i < mig.size(); i++){
                    if (mig[i] == j.nom) mig[i] = j2.nom;
                }
            }
            else if(j2.pos=="dav"){
                for (uint i=0; i < dav.size(); i++){
                    if (dav[i] == j.nom) dav[i] = j2.nom;
                }
            }

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
vector<Jugador> equip; //jugadors de l'equip actual ordenats de manera aleatòria
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


        if ((preu <= J) and (preu > 0 and punts > 0) or (punts==0 and preu==0)) {
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
    }

    std::sort(jugadors.begin(), jugadors.end(), ordre3);

    std::sort(listpor.begin(), listpor.end(), ordre3);
    std::sort(listdef.begin(), listdef.end(), ordre3);
    std::sort(listmig.begin(), listmig.end(), ordre3);
    std::sort(listdav.begin(), listdav.end(), ordre3);

}

int construir_sol_greedy(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T){
    int preu_restant = T;
    /*map<int, Jugador> jugadors_no_usats;
    for (int i = 0; i < jugadors.size(); ++i) {
        jugadors_no_usats[i] = jugadors[i];
    }*/
    for(uint i=0; i<jugadors.size() and Npor + Ndef + Nmig + Ndav != 0; i++){
        //sempre tindrem que jugadors[i].preu <= J
        Jugador j = jugadors[i];
        
        if(j.preu <= preu_restant){
            if(j.pos=="por" and Npor>0){ E.afegir_jugador(j); equip.push_back(j); Npor -= 1;}
            if(j.pos=="def" and Ndef>0){ E.afegir_jugador(j); equip.push_back(j); Ndef -= 1;}
            if(j.pos=="mig" and Nmig>0){ E.afegir_jugador(j); equip.push_back(j); Nmig -= 1;}
            if(j.pos=="dav" and Ndav>0){ E.afegir_jugador(j); equip.push_back(j); Ndav -= 1;}            
            preu_restant -= j.preu;
        }
    }
    return preu_restant;
}

Equip buscar_vei(int k, Jugador j, vector<Jugador>& equip2, Equip s2, int preu_restant){
    bool vei_trobat = false;
    uint i = 0;
    if (j.pos=="por"){
        while(vei_trobat == false){
            if((listpor[i].preu <= j.preu+preu_restant) and (j.nom != listpor[i].nom)){
                s2.canviar_jugador(j, listpor[i]);
                equip2[k] = listpor[i];
                vei_trobat = true;
            }
            else i++;
        }
    }
    else if (j.pos=="dav"){
        while(vei_trobat == false){
            if((listdav[i].preu <= j.preu+preu_restant) and (j.nom != listdav[i].nom)){
                s2.canviar_jugador(j, listdav[i]);
                equip2[k] = listdav[i];
                vei_trobat = true;
            }
            else i++;
        }
    }
    else if (j.pos=="def"){
        while(vei_trobat == false){
            if((listdef[i].preu <= j.preu+preu_restant) and (j.nom != listdef[i].nom)){
                s2.canviar_jugador(j, listdef[i]);
                equip2[k] = listdef[i];
                vei_trobat = true;
            }
            else i++;
        }
    }
    else if (j.pos=="mig"){
        while(vei_trobat == false){
            if((listmig[i].preu <= j.preu+preu_restant) and (j.nom != listmig[i].nom)){
                s2.canviar_jugador(j, listmig[i]);
                equip2[k] = listmig[i];
                vei_trobat = true;
            }
            else i++;
        }
    }
    return s2;
}

void simulated_annealing(Equip& E, vector<Jugador>& listdav,  vector<Jugador>& listdef, 
                             vector<Jugador>& listmig,  vector<Jugador>& listpor, int preu_restant,
                             ofstream& fitxer_sortida, auto start){
    Equip s = E;
    float t = 1;
    int k = 1;
    int n = equip.size();
    vector<Jugador> equip2 = equip; 

    for(uint i = 0; i<n; i++){
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dis(0.0, 1.0);
        double randomValue = dis(gen);
        Equip s2 = buscar_vei(i, equip[i], equip2, s, preu_restant);
        int fs = s.punts;
        int fs2 = s2.punts;

        if (fs2 > fs) {
            s = s2;
            equip = equip2;
            i = 0;
            write_sol(fitxer_sortida, E, start);
        }
        else{
            if (randomValue < 0.3) s = s2;
        }
        k = k+1;
        t = 1/k;
    }

};

void mh(Equip& E, int Npor, int Ndef, int Nmig, int Ndav, int T,
            ofstream& fitxer_sortida, auto start){
    
    while(true){
        //construïr solució inicial :
        int preu_restant = construir_sol_greedy(E, Npor, Ndef, Nmig, Ndav, T);
        //simulated annealing
        simulated_annealing(E, listdav, listdef, listmig, listpor, preu_restant, fitxer_sortida, start);
    }
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
    int Npor = 1;

    llegir_jugadors(dades_jugadors, listpor, listdef, listmig, listdav, J);
    Equip E = Equip(Ndef, Nmig, Ndav);

    mh(E, Npor, Ndef, Nmig, Ndav, T, fitxer_sortida, start);
}
