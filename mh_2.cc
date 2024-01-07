#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <math.h>
#include <random>
#include <cassert>

using namespace std;


class Jugador {
    public:
        string nom;
        string pos; //por, def, mig, dav
        int    preu;
        string club;
        int    punts;

        Jugador(const string& n, const string& pos, int pr, const string& c, int p):
            nom(n), pos(pos), preu(pr), club(c), punts(p){}

        Jugador():
            nom(""), pos(""), preu(0), club(""), punts(0){}

};


class Equip {
    public:
        Jugador por;
        vector<Jugador> def;
        vector<Jugador> mig;
        vector<Jugador> dav;
        int punts;
        int preu;

        Equip(int Ndef, int Nmig, int Ndav):
            por(Jugador()), def(vector<Jugador>(Ndef)), mig(vector<Jugador>(Nmig)),
            dav(vector<Jugador>(Ndav)), punts(0), preu(0){}

        Equip(const Equip& E):
            por(E.por), def(E.def), mig(E.mig), dav(E.dav), punts(E.punts), preu(E.preu){}

        void afegir_jugador(Jugador j){
            if(j.pos=="por") por = j;
            else if(j.pos=="def") def.push_back(j);
            else if(j.pos=="mig") mig.push_back(j);
            else if(j.pos=="dav") dav.push_back(j);
            punts += j.punts;
            preu += j.preu;
        }

        void canviar_jugador(int i, Jugador j2){
            //canvia el jugador j2 pel jugador de la posició corresponent 
            //que es troba a l'índex i dins del vector de l'equip 
            Jugador j1;
            if(j2.pos == "por"){ assert(false);
                j1 = por;
                por = j2;
            }
            else if(j2.pos == "def"){
                j1 = def[i];
                def[i] = j2;
            }
            else if(j2.pos == "mig"){assert(false);
                j1 = mig[i];
                mig[i] = j2;
            }
            else if(j2.pos == "dav"){assert(false);
                j1 = dav[i];
                dav[i] = j2;
            }
            punts -= j1.punts;
            preu -= j1.preu;
            punts += j2.punts;
            preu += j2.preu;
        }
};


vector<Jugador> listpor, listdef, listmig, listdav;  //ordenats per ratio greedy
vector<Jugador> listpor_preu, listdef_preu, listmig_preu, listdav_preu; //ordenats per preu
vector<Jugador> jugadors;
int millors_punts = 0;
int Ndef, Nmig, Ndav, T, J;
int Npor = 1;

bool ordre_preu(Jugador j1, Jugador j2){
    if(j1.preu == 0) return false;
    if(j2.preu == 0) return true;
    return (j1.preu) > (j2.preu);
}


bool ordre_greedy(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (pow(j1.punts, 3)/ pow(j1.preu, 1)) > (pow(j2.punts, 3) / pow(j2.preu, 1));
}


void write_sol(string sortida, Equip E, auto start){
    cout << "...................."<< endl;
    ofstream fs(sortida);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float,std::milli> duration = end - start;

    fs <<setprecision(1)<< duration.count() / 1000 << fixed << endl;

    fs << "POR: " << E.por.nom << endl;
    
    bool first = true;
    for (Jugador j : E.def){
        if(first){first=false;fs<<"DEF: "<<j.nom;}
        else fs<<";"<<j.nom;
    }
    fs<<endl;
    first = true;
    for (Jugador j : E.mig){
        if(first){first=false;fs<<"MIG: "<<j.nom;}
        else fs<<";"<<j.nom;
    }
    fs<<endl;
    first = true;
    for (Jugador j : E.dav){
        if(first){first=false;fs<<"DAV: "<<j.nom;}
        else fs<<";"<<j.nom;
    }
    fs<<endl;
    fs << "Punts: " << E.punts << endl;
    fs << "Preu: " << E.preu << endl;
    fs.close();
}   

void write_sol_cout(string sortida, Equip E, auto start){
    ofstream fs(sortida);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float,std::milli> duration = end - start;

    cout <<setprecision(1)<< duration.count() / 1000 << fixed << endl;

    cout << "POR: " << E.por.nom << endl;
    
    bool first = true;
    for (Jugador j : E.def){
        if(first){first=false;cout<<"DEF: "<<j.nom;}
        else cout<<";"<<j.nom;
    }
    cout<<endl;
    first = true;
    for (Jugador j : E.mig){
        if(first){first=false;cout<<"MIG: "<<j.nom;}
        else cout<<";"<<j.nom;
    }
    cout<<endl;
    first = true;
    for (Jugador j : E.dav){
        if(first){first=false;cout<<"DAV: "<<j.nom;}
        else cout<<";"<<j.nom;
    }
    cout<<endl;
    cout << "Punts: " << E.punts << endl;
    cout << "Preu: " << E.preu << endl;
    fs.close();
}

void write_sol_cout(Equip E){
    cout << "POR: " << E.por.nom << endl;
    
    bool first = true;
    for (Jugador j : E.def){
        if(first){first=false;cout<<"DEF: "<<j.nom;}
        else cout<<";"<<j.nom;
    }
    cout<<endl;
    first = true;
    for (Jugador j : E.mig){
        if(first){first=false;cout<<"MIG: "<<j.nom;}
        else cout<<";"<<j.nom;
    }
    cout<<endl;
    first = true;
    for (Jugador j : E.dav){
        if(first){first=false;cout<<"DAV: "<<j.nom;}
        else cout<<";"<<j.nom;
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


        if ((preu <= J) and ( (preu > 0 and punts > 0) or (punts==0 and preu==0))){
            jugadors.push_back(j);

            if(pos=="por") {
                listpor.push_back(j);
                listpor_preu.push_back(j);
            }
            if(pos=="def") {
                listdef.push_back(j);
                listdef_preu.push_back(j);
            }
            if(pos=="mig") {
                listmig.push_back(j);
                listmig_preu.push_back(j);
            }
            if(pos=="dav") {
                listdav.push_back(j);
                listdav_preu.push_back(j);
            }
        }
    }

    std::sort(jugadors.begin(), jugadors.end(), ordre_greedy);

    std::sort(listpor.begin(), listpor.end(), ordre_greedy);
    std::sort(listdef.begin(), listdef.end(), ordre_greedy);
    std::sort(listmig.begin(), listmig.end(), ordre_greedy);
    std::sort(listdav.begin(), listdav.end(), ordre_greedy);

    std::sort(listpor.begin(), listpor.end(), ordre_preu);
    std::sort(listdef.begin(), listdef.end(), ordre_preu);
    std::sort(listmig.begin(), listmig.end(), ordre_preu);
    std::sort(listdav.begin(), listdav.end(), ordre_preu);
    return;
}

void construir_sol_greedy(Equip& E, int Npor, int Ndef, int Nmig, int Ndav){
    int preu_restant = T;
    /*map<int, Jugador> jugadors_no_usats;
    for (int i = 0; i < jugadors.size(); ++i) {
        jugadors_no_usats[i] = jugadors[i];
    }*/
    for(uint i=0; (i<jugadors.size()) and (Npor + Ndef + Nmig + Ndav != 0); i++){
        //sempre tindrem que jugadors[i].preu <= J
        Jugador j = jugadors[i];
        
        if(j.preu <= preu_restant){
            if(j.pos=="por" and Npor>0){ E.afegir_jugador(j); Npor--; preu_restant -= j.preu;}
            if(j.pos=="def" and Ndef>0){ E.afegir_jugador(j); Ndef--; preu_restant -= j.preu;}
            if(j.pos=="mig" and Nmig>0){ E.afegir_jugador(j); Nmig--; preu_restant -= j.preu;}
            if(j.pos=="dav" and Ndav>0){ E.afegir_jugador(j); Ndav--; preu_restant -= j.preu;}            
        }
    }

    cout << "-----------------------------------------------------------------" << endl;
    write_sol_cout(E);
}


int tria_jugador_prova(const Equip& E, Jugador& old){
    int m = (rand() % Ndef);  //pos=0 -> triem un porter
                            //pos=1 -> triem un defensa
                            //...
    
    old = E.def[m];
    return m;
}


int tria_jugador(const Equip& E, Jugador& old){
    int m = 1 + (rand() % 11);  //pos=0 -> triem un porter
                            //pos=1 -> triem un defensa
                            //...
    if(m==1){
        old = E.por;
        return 1;
    }
    --m;
    if(m<=Ndef){
        old = E.def[m-1];
        return m-1;
    }
    m -= Ndef;
    if(m<=Nmig){
        old = E.mig[m-1];
        return m-1;
    }
    m -= Nmig;
    old = E.dav[m-1];
    return m-1;
}

void buscar_vei(const Equip& s1, Equip& s2){
    //busquem un vei de s1
    //s2 serà el vei de s1
    int preu_restant = T - s1.preu;
    s2 = Equip(s1);

    //triem quin jugador treurem del nostre equip:
    Jugador old;
    int i_old = tria_jugador_prova(s1, old);

    preu_restant += old.preu;

    //ara triem el jugador que afegirem al nostre equip
    int i_new, i_;
    if(old.pos=="por"){ // busequem porter
        assert(false);
        i_new = rand() % listpor_preu.size();
        while(i_new < listpor_preu.size()){
            if(listpor_preu[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listpor_preu[i_new]);
                break;
            }
            else{
                i_ = i_new;
                i_new = ( rand() % (listpor_preu.size()-i_new) ) + i_new;
                // nou índex random en l'interval [j, size()]
                // ja que si no podem agafar el jugador j per preu, 
                // tampoc podrem agafar cap que estigui més a l'esquerra.
                assert(i_ <= i_new);
            }
        }
    }

    if(old.pos=="def"){
        i_new = rand() % listdef_preu.size();
        while(i_new < listdef_preu.size()){
            if(listdef_preu[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listdef_preu[i_new]);
                break;
            }
            else{
                i_ = i_new;
                i_new = ( rand() % (listdef_preu.size()-i_new) ) + i_new;
                assert(i_ <= i_new);
            }
        }
    }
    
    if(old.pos=="mig"){ assert(false);
        i_new = rand() % listmig_preu.size();
        while(i_new < listmig_preu.size()){
            if(listmig_preu[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listmig_preu[i_new]);
                break;
            }
            else{
                i_ = i_new;
                i_new = ( rand() % (listmig_preu.size()-i_new) ) + i_new;
                assert(i_ <= i_new);
            }
        }
    }

    if(old.pos=="dav"){assert(false);
        i_new = rand() % listdav_preu.size();
        while(i_new < listdav_preu.size()){
            if(listdav_preu[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listdav_preu[i_new]);
                break;
            }
            else{
                i_ = i_new;
                i_new = ( rand() % (listdav_preu.size()-i_new) ) + i_new;
                assert(i_ <= i_new);
            }
        }
    }
}

void simulated_annealing(Equip& E, string fitxer_sortida, auto start){
    Equip s1 = Equip(E);
    Equip s2 = Equip(E);
    float t = 1;
    int k = 1;
    int iteracions_sense_canvi = 0;  //les iteracions que portem sense canviar de solució 

    if (s1.punts > millors_punts) {
        cout << "**********************************************************************************" << endl;
        write_sol(fitxer_sortida, s2, start);
        millors_punts = s1.punts;
        cout << "**********************************************************************************" << endl;
    }

    while(iteracions_sense_canvi < 15){ // quan portem moltes iteracions amb la mateixa solució, 
                                        // considerem que hem arribat a un màxim local i acabem
                                        // el simulated annealing

        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<double> dis(0.0, 1.0);
        double randomValue = dis(gen);

        buscar_vei(s1, s2);
        //write_sol_cout(fitxer_sortida, s2, start);



        if (s2.punts > s1.punts) {
            if (s2.punts > millors_punts) {
                cout << "**********************************************************************************" << endl;
                write_sol(fitxer_sortida, s2, start);
                millors_punts = s2.punts;
                cout << "**********************************************************************************" << endl;
            }
            s1 = Equip(s2);
            iteracions_sense_canvi = 0;
        }
        else{
            if (randomValue < 1) {s1 = s2; iteracions_sense_canvi = 0;}
            else ++iteracions_sense_canvi;
        }
        k = k+1;
        t = 1/k;
    }
};

void mh(string fitxer_sortida, auto start){
    
    while(true){
        //construïr solució inicial :
        Equip E = Equip(0, 0, 0);
        construir_sol_greedy(E, Npor, Ndef, Nmig, Ndav);
        //simulated annealing
        simulated_annealing(E, fitxer_sortida, start);
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
    ifstream plantilla(argv[2]);
    string fitxer_sortida(argv[3]);

    plantilla >> Ndef >> Nmig >> Ndav >> T >> J;
    plantilla.close();

    ofstream fs(fitxer_sortida);
    fs << "hola" << endl;
    fs.close();
    llegir_jugadors(dades_jugadors);
    mh(fitxer_sortida, start);
}
