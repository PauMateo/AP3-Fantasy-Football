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
int millors_punts = -1;
int Npor = 1, Ndef, Nmig, Ndav, T, J;
vector<int> bestpor, bestdef, bestmig, bestdav;  //punts dels jugadors amb més punts
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


void write_sol(string fsname, Equip E, auto start){

    ofstream fs(fsname);
    auto end = std::chrono::system_clock::now();
    chrono::duration<float, milli> duration = end - start;
    
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

    fs.close();
}

void write_sol_cout(string fsname, Equip E, auto start){

    auto end = std::chrono::system_clock::now();
    chrono::duration<float, milli> duration = end - start;
    
    cout <<setprecision(1)<< duration.count() / 1000 << fixed << endl;

    cout << "POR: " << E.por << endl;
    
    bool first = true;
    for (string nom : E.def){
        if(first){first=false;cout<<"DEF: "<<nom;}
        else cout<<";"<<nom;
    }
    cout<<endl;
    first = true;
    for (string nom : E.mig){
        if(first){first=false;cout<<"MIG: "<<nom;}
        else cout<<";"<<nom;
    }
    cout<<endl;
    first = true;
    for (string nom : E.dav){
        if(first){first=false;cout<<"DAV: "<<nom;}
        else cout<<";"<<nom;
    }
    cout<<endl;
    cout << "Punts: " << E.punts << endl;
    cout << "Preu: " << E.preu << endl;
}

void write_perm_cout(string fsname, Equip E, auto start){

    cout << "POR: " << E.por << endl;
    
    bool first = true;
    for (string nom : E.def){
        if(first){first=false;cout<<"DEF: "<<nom;}
        else cout<<";"<<nom;
    }
    cout<<endl;
    first = true;
    for (string nom : E.mig){
        if(first){first=false;cout<<"MIG: "<<nom;}
        else cout<<";"<<nom;
    }
    cout<<endl;
    first = true;
    for (string nom : E.dav){
        if(first){first=false;cout<<"DAV: "<<nom;}
        else cout<<";"<<nom;
    }
    cout<<endl<<endl;
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


void llegir_jugadors(ifstream& dades_jugadors){
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

        if(nom=="") break;
        Jugador j = Jugador(nom,pos,preu,club,punts);

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
    }


    std::sort(listpor.begin(), listpor.end(), ordre0);
    std::sort(listdef.begin(), listdef.end(), ordre0);
    std::sort(listmig.begin(), listmig.end(), ordre0);
    std::sort(listdav.begin(), listdav.end(), ordre0);

    cout << "PORTERS     "<<"DEFENSES     "<<"MIGCAMP     "<<"DAV"<<endl;
    for(int i=0; i<7; i++){
        cout << listpor[i].nom<<" "<<listpor[i].punts<<"  ";
        cout << listdef[i].nom<<" "<<listdef[i].punts<<"  ";
        cout << listmig[i].nom<<" "<<listmig[i].punts<<"  ";
        cout << listdav[i].nom<<" "<<listdav[i].punts<<"  ";
        cout << endl;
    }
    cout<<endl<<endl;

    bestpor = vector<int>(Npor+1, 0);
    bestdef = vector<int>(Ndef+1, 0);
    bestmig = vector<int>(Nmig+1, 0);
    bestdav = vector<int>(Ndav+1, 0);
    int p = 0;
    for(int i=0; i<Npor; i++){
        p += listpor[i].punts;
        bestpor[i+1] = p;
    }
    p = 0;
    for(int i=0; i<Ndef; i++){
        p += listdef[i].punts;
        bestdef[i+1] = p;
    }
    p = 0;
    for(int i=0; i<Nmig; i++){
        p += listmig[i].punts;
        bestmig[i+1] = p;
    }
    p = 0;
    for(int i=0; i<Ndav; i++){
        p += listdav[i].punts;
        bestdav[i+1] = p;
    }

    cout << bestpor[0] << " "<<bestpor[1]<< endl;
    for(uint i=0; i<bestdef.size();i++) cout<<bestdef[i]<<" ";
    cout <<endl;
    for(uint i=0; i<bestmig.size();i++) cout<<bestmig[i]<<" ";
    cout <<endl;
    for(uint i=0; i<bestdav.size();i++) cout<<bestdav[i]<<" ";
    cout <<endl;



    std::sort(listpor.begin(), listpor.end(), ordre3);
    std::sort(listdef.begin(), listdef.end(), ordre3);
    std::sort(listmig.begin(), listmig.end(), ordre3);
    std::sort(listdav.begin(), listdav.end(), ordre3);
}


bool prune(int porres, int defres, int migres, int davres){  //retorna true si podem la branca del backtracking
    /*
    donats el nombre de defenses que ens queden per posar a l'equip (defres), 
    el nombre de migcampistes, i el nombre de davanters... podem la construcció 
    d'aquest equip si tot i que omplim les posicinos que ens queden per omplir amb
    els millors jugadors (els que tene més punts, sense ni tan sols tenir en compte el preu)
    no aconseguiríem obtenir més punts que el millor equip que hem trobat fins ara.
    */
    int punts = 0;
    punts += bestpor[porres];
    punts += bestdef[defres];
    punts += bestmig[migres];
    punts += bestdav[davres];

    if(punts + E.punts < millors_punts) {return true; cout<<"."<<endl;} //si que podem
    return false; //no podem
}



/*
idxdef és la posició de defensa que de defensa; ídem amb idxpor, idxmig, idxdav

*/
void exh_search(Equip& E, int idxpor, int idxdef, int idxmig, int idxdav,
                int kpor, int kdef, int kmig, int kdav,
                int preu_restant, string fitxer_sortida, auto start){
    
    /*if(idxpor==Npor and idxdef==Ndef and idxmig==Nmig and idxdav==Ndav){
        cout << "check" << endl;
        return write_perm_cout(fitxer_sortida, E, start);
    }*/

    if(idxpor==Npor and idxdef>=Ndef and idxmig>=Nmig and idxdav>=Ndav){
        if(E.punts > millors_punts){
            millors_punts = E.punts;
            return write_sol_cout(fitxer_sortida, E, start);
        }
        return;
    }

    if(prune(Npor-idxpor, Ndef-idxdef, Nmig-idxmig, Ndav-idxdav)) return;

    if(idxpor<1){
        for(int i = 0; i<int(listpor.size()); i++){
            Jugador j = listpor[i];
            if(j.preu <= preu_restant){
                E.por = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor+1, idxdef, idxmig, idxdav, kpor, kdef, kmig, kdav, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}

    else if(idxdef<Ndef){ //si hem posat tots els porters, comencem a posar defenses
        //i es index del germà
        //idxdef es la profunditat de la crida
        for(int i = kdef; i<int(listdef.size()); i++){
            Jugador j = listdef[i];
            if(j.preu <= preu_restant){
                E.def[idxdef] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor, idxdef+1, idxmig, idxdav, kpor, i+1, kmig, kdav, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}


    else if(idxmig < Nmig){ //si hem posat tots els defenses, comencem a posar migcampistes
            for(int i = kmig; i<int(listmig.size()); i++){
            Jugador j = listmig[i];
            if(j.preu <= preu_restant){
                E.mig[idxmig] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor, idxdef, idxmig+1, idxdav, kpor, kdef, i+1, kdav, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}
    
    else if(idxdav < Ndav){ //quan hem posat tos els migcampistes, comencem a posar davanters
        for(int i = kdav; i<int(listdav.size()); i++){
            Jugador j = listdav[i];
            if(j.preu <= preu_restant){
                E.dav[idxdav] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor, idxdef, idxmig, idxdav+1, kpor, kdef, kmig, i+1, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}
}


void exh_search(string fitxer_sortida, auto start){
    E = Equip(Ndef, Nmig, Ndav);
    return exh_search(E, 0, 0, 0, 0, 0, 0, 0, 0, T, fitxer_sortida, start);
}


int main(int argc, char** argv){
    if (argc != 4) {
    cerr << "Syntax: " << argv[0] << " data_base.txt" <<
            " bench.txt" << " output.txt" << endl;
    exit(1);
    }
    auto start = std::chrono::system_clock::now();

    ifstream dades_jugadors(argv[1]);
    ifstream plantilla(argv[3]);

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    cout << Npor << " "<<Ndef <<" "<< Nmig <<" "<< Ndav << "  "<<T << " "<<J<<endl;
    plantilla.close();

    llegir_jugadors(dades_jugadors);
    
    exh_search(argv[2], start);
}
