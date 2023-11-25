#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

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


class Tactic {
    public:
        uint goal;
        uint def;
        uint mid;
        uint str;
        Tactic() {}
        Tactic(uint g, uint d, uint m, uint s):goal(g), def(d), mid(m), str(s) {}
};


vector<Jugador> listpor, listdef, listmig, listdav;
Equip E_millor = Equip();
int maxPuntspor=0, maxPuntsdef=0, maxPuntsmig=0, maxPuntsdav=0;

bool ordre1(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (j1.punts*j1.punts / j2.preu) > (j2.punts*j2.punts / j2.preu);
}


void write_sol(string fitxer_sortida_nom, Equip E){

    ofstream fs(fitxer_sortida_nom);


    //time!
    /*E.escriu_jugadors("POR", fs);
    E.escriu_jugadors("def", fs);
    E.escriu_jugadors("mig", fs);
    E.escriu_jugadors("dav", fs);
    fs << "Punts: " << E.punts << endl;
    fs << "Preu: " << E.preu << endl; */

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


void llegir_jugadors(ifstream& dades_jugadors,
                     vector<Jugador>& listpor,
                     vector<Jugador>& listdef,
                     vector<Jugador>& listmig,
                     vector<Jugador>& listdav){
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



        if(pos=="por") {
            maxPuntspor = max(maxPuntspor, j.punts);
            listpor.push_back(j);
        }
        if(pos=="def") {
            maxPuntsdef = max(maxPuntsdef, j.punts);
            listdef.push_back(j);
        }
        if(pos=="mig") {
            maxPuntsmig = max(maxPuntsmig, j.punts);
            listmig.push_back(j);
        }
        if(pos=="dav") {
            maxPuntsdav = max(maxPuntsdav, j.punts);
            listdav.push_back(j);
        }

        //cout<<nom<<";"<<pos<<";"<<preu<<";"<<club<<";"<<punts<<endl;
    }

    sort(listpor.begin(), listpor.end(), ordre1);
    sort(listdef.begin(), listdef.end(), ordre1);
    sort(listmig.begin(), listmig.end(), ordre1);
    sort(listdav.begin(), listdav.end(), ordre1);
}


void exh_search_rec(Equip E, int Npor, int Ndef, int Nmig, int Ndav, int T, int J,
            string fitxer_sortida){
    if( T<0 or (Npor*maxPuntspor + Ndef*maxPuntsdef + Nmig*maxPuntsmig + Ndav*maxPuntsdav < E.punts)) return;
    
    
    if (Npor+Ndef+Nmig+Ndav == 0){
        E_millor = E;
        return write_sol(fitxer_sortida, E);
    }


    if(Npor > 0){
        for(Jugador j : listpor){
            if(j.preu <= J and j.preu <= T){
                E.afegir_jugador(j);
                exh_search_rec(E, Npor-1, Ndef, Nmig, Ndav, T-j.preu, J, fitxer_sortida);
    }}}

    if(Ndef > 0){
        for(Jugador j : listdef){
            if(j.preu <= J and j.preu <= T){
                E.afegir_jugador(j);
                exh_search_rec(E, Npor, Ndef-1, Nmig, Ndav, T-j.preu, J, fitxer_sortida);
    }}}

    if(Nmig > 0){
        for(Jugador j : listmig){
            if(j.preu <= J and j.preu <= T){
                E.afegir_jugador(j);
                exh_search_rec(E, Npor, Ndef, Nmig-1, Ndav, T-j.preu, J, fitxer_sortida);
    }}}

    if(Ndav > 0){
        for(Jugador j : listdav){
            if(j.preu <= J and j.preu <= T){
                E.afegir_jugador(j);
                exh_search_rec(E, Npor, Ndef, Nmig, Ndav-1, T-j.preu, J, fitxer_sortida);
    }}}
}


void exh_search(int Ndef, int Nmig, int Ndav, int T, int J,
            string fitxer_sortida){
    Equip E = Equip();
    return exh_search_rec(E, 0, Ndef, Nmig, Ndav, T, J, fitxer_sortida);
}


int main(int argc, char** argv){
    if (argc != 4) {
    cerr << "Syntax: " << argv[0] << " data_base.txt" <<
            " bench.txt" << " output.txt" << endl;
    exit(1);
    }
    ifstream dades_jugadors(argv[1]);
    ifstream plantilla(argv[2]);
    string fitxer_sortida = argv[3];
    llegir_jugadors(dades_jugadors, listpor, listdef, listmig, listdav);

    int Ndef, Nmig, Ndav, T, J;

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    plantilla.close();

    exh_search(Ndef,Nmig,Ndav,T,J, fitxer_sortida);

}
