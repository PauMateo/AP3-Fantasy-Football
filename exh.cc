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
};



vector<Jugador> listpor, listdef, listmig, listdav;
int millors_punts = -1;
int Npor = 1, Ndef, Nmig, Ndav, T, J;
vector<int> bestpor, bestdef, bestmig, bestdav;  //punts dels jugadors amb més punts
Equip E = Equip(0, 0, 0);

bool ordre0(Jugador j1, Jugador j2){
    if(j1.punts == j2.punts) return j1.preu < j2.preu;
    return (j1.punts) > (j2.punts);
}


void write_sol(string fsname, Equip E, auto start){

    ofstream fs(fsname);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    fs <<fixed<<setprecision(1)<< duration.count()<< endl;

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
    std::chrono::duration<double> duration = end - start;
    cout <<fixed<<setprecision(1)<< duration.count() << endl;

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


void llegir_jugadors(ifstream& dades_jugadors){
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

        if(nom=="") break;
        Jugador j = Jugador(nom,pos,preu,club,punts);

        if((preu <= J) and ( (preu>0 and punts>0) or (punts==0 and preu==0) )){
            if(pos=="por") {
                listpor.push_back(j);
            }
            if(pos=="def") {
                listdef.push_back(j);
            }
            if(pos=="mig") {
                listmig.push_back(j);
            }
            if(pos=="dav") {
                listdav.push_back(j);
            }
        }
    }


    std::sort(listpor.begin(), listpor.end(), ordre0);
    std::sort(listdef.begin(), listdef.end(), ordre0);
    std::sort(listmig.begin(), listmig.end(), ordre0);
    std::sort(listdav.begin(), listdav.end(), ordre0);

    bestpor = vector<int>(listpor.size()+1, 0);
    bestdef = vector<int>(listdef.size()+1, 0);
    bestmig = vector<int>(listmig.size()+1, 0);
    bestdav = vector<int>(listdav.size()+1, 0);

    int p = 0;
    for(uint i=0; i<listpor.size(); i++){
        p += listpor[i].punts;
        bestpor[i+1] = p;
    }
    p = 0;
    for(uint i=0; i<listdef.size(); i++){
        p += listdef[i].punts;
        bestdef[i+1] = p;
    }
    p = 0;
    for(uint i=0; i<listmig.size(); i++){
        p += listmig[i].punts;
        bestmig[i+1] = p;
    }
    p = 0;
    for(uint i=0; i<listdav.size(); i++){
        p += listdav[i].punts;
        bestdav[i+1] = p;
    } 
}


bool prune(int porres, int defres, int migres, int davres,
            int kpor,   int kdef,   int kmig,   int kdav){  
    /*
    retorna true si podem la branca del backtracking;
    donats el nombre de defenses que ens queden per posar a l'equip (defres), 
    el nombre de migcampistes, i el nombre de davanters... podem la construcció 
    d'aquest equip si tot i que omplim les posicinos que ens queden per omplir amb
    els jugadors que tenen més punts que encara no hem considerat agafar
    no aconseguiríem obtenir més punts que el millor equip que hem trobat fins ara.
    */

    int punts = 0;

    punts += bestpor[kpor+porres] - bestpor[kpor];
    punts += bestdef[kdef+defres] - bestdef[kdef];
    punts += bestmig[kmig+migres] - bestmig[kmig];
    punts += bestdav[kdav+davres] - bestdav[kdav];

    if(punts + E.punts < millors_punts) return true;  //si que podem
    return false; //no podem
}



/*

tots aquests índexs (idxpor, kpor, ...) són només per fer correctament les diferents 
combinacions de conjunts de jugadors (permutacions sense ordre i sense repeticions).
idxdef : nombre de defenses que ja hem agafat per l'equip; és a dir, dins del 
         vector on tenim els defenses, és l'índex on posarem el següent defensa
kdef   : dins del vector on tenim tots els defenses de la base de dades (listdef)
         és la posició a partir de la cual podem agafar defenses per l'equip


*/
void exh_search(Equip& E, int idxpor, int idxdef, int idxmig, int idxdav,
                int kpor, int kdef, int kmig, int kdav,
                int preu_restant, string fitxer_sortida, auto start){
    
    int porres = Npor - idxpor; //nombre de porters que ens queden per posar
    int defres = Ndef - idxdef; //etc
    int migres = Nmig - idxmig;
    int davres = Ndav - idxdav;

    if(prune(porres, defres, migres, davres, kpor, kdef, kmig, kdav)) return;

    if(porres + defres + migres + davres == 0){
        if(E.punts > millors_punts){
            millors_punts = E.punts;
            return write_sol_cout(fitxer_sortida, E, start);
        }
        return; //en cas que no millorem punts
    }


    if(porres>0){ //si no hem agafat cap porter, n'agafem un
        for(uint i = 0; i<listpor.size(); i++){
            Jugador j = listpor[i];
            if(j.preu <= preu_restant){
                E.por = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor+1, idxdef, idxmig, idxdav, kpor, kdef, kmig, kdav, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}

    else if(defres>0){ //si hem posat tots els porters, comencem a posar defenses
        //i es index del germà
        //idxdef es la profunditat de la crida
        for(uint i = kdef; i<=listdef.size()-defres; i++){
            Jugador j = listdef[i];
            if(j.preu <= preu_restant){
                E.def[idxdef] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor, idxdef+1, idxmig, idxdav, kpor, i+1, kmig, kdav, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}


    else if(migres>0){ //si hem posat tots els defenses, comencem a posar migcampistes
        for(uint i = kmig; i<listmig.size()-migres ; i++){
            Jugador j = listmig[i];
            if(j.preu <= preu_restant){
                E.mig[idxmig] = j.nom;
                E.punts += j.punts;
                E.preu += j.preu;
                exh_search(E, idxpor, idxdef, idxmig+1, idxdav, kpor, kdef, i+1, kdav, preu_restant-j.preu, fitxer_sortida, start);
                E.punts -= j.punts;
                E.preu -= j.preu;
    }}}
    
    else if(davres>0){ //quan hem posat tos els migcampistes, comencem a posar davanters
        for(uint i = kdav; i<listdav.size()-davres; i++){
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
    ifstream plantilla(argv[2]);

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    plantilla.close();

    llegir_jugadors(dades_jugadors);
    exh_search(argv[3], start);
}
