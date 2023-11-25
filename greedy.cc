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

void write_sol(ofstream& fs, Equip E){
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
    
        if(pos=="por") listpor.push_back(j);
        if(pos=="def") listdef.push_back(j);
        if(pos=="mig") listmig.push_back(j);
        if(pos=="dav") listdav.push_back(j);

        //cout<<nom<<";"<<pos<<";"<<preu<<";"<<club<<";"<<punts<<endl;
    }
}


void greedy(int Ndef, int Nmig, int Ndav, int T, int J,
            ifstream& dades_jugadors, ofstream& fitxer_sortida){
    

    Equip E = Equip();

    vector<Jugador> listpor, listdef, listmig, listdav;
    llegir_jugadors(dades_jugadors, listpor, listdef, listmig, listdav);
    
    sort(listpor.begin(), listpor.end(), ordre1);
    sort(listdef.begin(), listdef.end(), ordre1);
    sort(listmig.begin(), listmig.end(), ordre1);
    sort(listdav.begin(), listdav.end(), ordre1);

    int preu_restant = T;
    bool por_trobat = false;

    for(uint i=0; i<listpor.size() and not por_trobat; ++i){
        if(listpor[i].preu <=J and listpor[i].preu<=preu_restant){
            por_trobat = true;
            preu_restant -= listpor[i].preu;
            E.afegir_jugador(listpor[i]);
    }   }

    for(uint i=0; i<listdef.size() and Ndef>0; ++i){
        if(listdef[i].preu <=J and listdef[i].preu<=preu_restant){
            Ndef -= 1;
            preu_restant -= listdef[i].preu;
            E.afegir_jugador(listdef[i]);
    }   }

    for(uint i=0; i<listmig.size() and Nmig>0; ++i){
        if(listmig[i].preu <=J and listmig[i].preu<=preu_restant){
            Nmig -= 1;
            preu_restant -= listmig[i].preu;
            E.afegir_jugador(listmig[i]);
    }   }

    for(uint i=0; i<listdav.size() and Ndav>0; ++i){
        if(listdav[i].preu <=J and listdav[i].preu<=preu_restant){
            Ndav -= 1;
            preu_restant -= listdav[i].preu;
            E.afegir_jugador(listdav[i]);
    }   }

    return write_sol(fitxer_sortida, E);
}


int main(int argc, char** argv){
    if (argc != 4) {
    cerr << "Syntax: " << argv[0] << " data_base.txt" <<
            " bench.txt" << " output.txt" << endl;
    exit(1);
    }

    ifstream dades_jugadors(argv[1]);
    ifstream plantilla(argv[2]);
    ofstream fitxer_sortida(argv[3]);

    int Ndef, Nmig, Ndav, T, J;

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    plantilla.close();

    greedy(Ndef,Nmig,Ndav,T,J,dades_jugadors, fitxer_sortida);

}
