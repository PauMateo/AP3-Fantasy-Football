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

vector<Jugador> listpor, listdef, listmig, listdav;  //ordenats per ratio greedy
vector<bool> Upor, Udef, Umig, Udav;  //llistes dels jugadors que ja hem fet servir
vector<Jugador> jugadors;
int millors_punts = 0;
int Ndef, Nmig, Ndav, T, J;
int Npor = 1;

class Equip {
    public:
        int por;         //índex a la llista listpor; listpor[por] és el porter de l'equip
        vector<int> def; //índexs dels defenses a la llista listdef; els defenses son listdef[def[i]]
        vector<int> mig; //etc
        vector<int> dav;
        int punts;
        int preu;

        Equip(int Ndef, int Nmig, int Ndav):
            por(0), def(vector<int>(Ndef)), mig(vector<int>(Nmig)),
            dav(vector<int>(Ndav)), punts(0), preu(0){}

        Equip(const Equip& E):  //copy constructor
            por(E.por), def(E.def), mig(E.mig), dav(E.dav), punts(E.punts), preu(E.preu){}

        void afegir_jugador(Jugador j, int i_j){
            if(j.pos=="por") por = i_j;
            else if(j.pos=="def") def.push_back(i_j);
            else if(j.pos=="mig") mig.push_back(i_j);
            else if(j.pos=="dav") dav.push_back(i_j);
            punts += j.punts;
            preu += j.preu;
        }

        void canviar_jugador(int iold, Jugador jnew, int inew){
            //jnew: jugador nou que posem a l'equip
            //iold: index del jugador que traiem de l'equip; és a dir, si 
            //      el jugador que estem posant nou és defensa, aleshores 
            //      el canviem pel jugador this.def[iold]
            //inew: index d'on es troba el jugador jnew al vector de jugadors 
            //      de la seva posició; per exemple, listdef[inew] = jnew 
            Jugador j1;
            if(jnew.pos == "por"){ 
                j1 = listpor[por];
                por = inew;
            }
            else if(jnew.pos == "def"){
                j1 = listdef[def[iold]];
                def[iold] = inew;
            }
            else if(jnew.pos == "mig"){
                j1 = listmig[mig[iold]];
                mig[iold] = inew;
            }
            else if(jnew.pos == "dav"){
                j1 = listdav[dav[iold]];
                dav[iold] = inew;
            }
            punts -= j1.punts;
            preu -= j1.preu;
            punts += jnew.punts;
            preu += jnew.preu;
        }
};


bool ordre_greedy(Jugador j1, Jugador j2){
    if(j1.punts == 0) return false;
    if(j2.punts == 0) return true;
    return (pow(j1.punts, 3)/ pow(j1.preu, 1)) > (pow(j2.punts, 3) / pow(j2.preu, 1));
}


void write_sol(string sortida, Equip E, auto start){
    ofstream fs(sortida);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float,std::milli> duration = end - start;

    fs <<setprecision(1)<< duration.count() / 1000 << fixed << endl;

    fs << "POR: " << listpor[E.por].nom << endl;
    
    bool first = true;
    for (int i : E.def){
        if(first){first=false;fs<<"DEF: "<<listdef[i].nom;}
        else fs<<";"<<listdef[i].nom;
    }
    fs<<endl;
    first = true;
    for (int i : E.mig){
        if(first){first=false;fs<<"MIG: "<<listmig[i].nom;}
        else fs<<";"<<listmig[i].nom;
    }
    fs<<endl;
    first = true;
    for (int i : E.dav){
        if(first){first=false;fs<<"DAV: "<<listdav[i].nom;}
        else fs<<";"<<listdav[i].nom;
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

    cout << "POR: " << listpor[E.por].nom << endl;
    
    bool first = true;
    for (int i : E.def){
        if(first){first=false;cout<<"DEF: "<<listdef[i].nom;}
        else cout<<";"<<listdef[i].nom;
    }
    cout<<endl;
    first = true;
    for (int i : E.mig){
        if(first){first=false;cout<<"MIG: "<<listmig[i].nom;}
        else cout<<";"<<listmig[i].nom;
    }
    cout<<endl;
    first = true;
    for (int i : E.dav){
        if(first){first=false;cout<<"DAV: "<<listdav[i].nom;}
        else cout<<";"<<listdav[i].nom;
    }
    cout<<endl;
    cout << "Punts: " << E.punts << endl;
    cout << "Preu: " << E.preu << endl;
    fs.close();
}


void llegir_jugadors(ifstream& dades_jugadors){
    // llegeix les dades dels jugadors i els guarda als vector
    // listpor, listdef... Ordena tals vectors amb la funció ordre_greedy().
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

    std::sort(listpor.begin(), listpor.end(), ordre_greedy);
    std::sort(listdef.begin(), listdef.end(), ordre_greedy);
    std::sort(listmig.begin(), listmig.end(), ordre_greedy);
    std::sort(listdav.begin(), listdav.end(), ordre_greedy);

    return;
}

void construir_sol_greedy(Equip& E, int Npor, int Ndef, int Nmig, int Ndav){
    // construeix una solució inicial per al simulated annealing
    int preu_restant = T;

    Upor = vector<bool>(listpor.size(), false);
    Udef = vector<bool>(listdef.size(), false);
    Umig = vector<bool>(listmig.size(), false);
    Udav = vector<bool>(listdav.size(), false);
    for(uint i=0; i<listpor.size() and Npor>0; ++i){
        if(listpor[i].preu<=preu_restant){
            preu_restant -= listpor[i].preu;
            E.afegir_jugador(listpor[i], i);
            Upor[i] = true;
            --Npor;
    }   }

    for(uint i=0; i<listdef.size() and Ndef>0; ++i){
        if(listdef[i].preu<=preu_restant){
            Ndef--;
            preu_restant -= listdef[i].preu;
            E.afegir_jugador(listdef[i], i);
            Udef[i] = true;
    }   }

    for(uint i=0; i<listmig.size() and Nmig>0; ++i){
        if(listmig[i].preu<=preu_restant){
            Nmig--;
            preu_restant -= listmig[i].preu;
            E.afegir_jugador(listmig[i], i);
            Umig[i] = true;
    }   }

    for(uint i=0; i<listdav.size() and Ndav>0; ++i){
        if(listdav[i].preu<=preu_restant){
            Ndav--;
            preu_restant -= listdav[i].preu;
            E.afegir_jugador(listdav[i], i);
            Udav[i] = true;
    }   }

}


int tria_jugador(const Equip& s1, Jugador& old){
    // funció que escull un jugador del nostre equip a l'atzar per tal
    // de treure'l i intercanviar-lo amb un altre jugador.
    // Retorna l'índex on es troba el jugador dins del vector de l'equip
    // i guarda el jugador a la variable old.
    int m = 1 + (rand() % 11); //pos=0 -> triem un porter
                               //pos=1 -> triem un defensa
                               //...
    if(m==1){
        old = listpor[s1.por];
        return 1;
    }
    --m;
    if(m<=Ndef){
        old = listdef[s1.def[m-1]];
        return m-1;
    }
    m -= Ndef;
    if(m<=Nmig){
        old = listmig[s1.mig[m-1]];
        return m-1;
    }
    m -= Nmig;
    old = listdav[s1.dav[m-1]];
    return m-1;
}

int tria_vei(const Equip& s1, Equip& s2, const Jugador& old, const int i_old ){
    // Funció que busca una solució veïna de s1. El jugador old és el que traurem
    // de l'equip s1, i buscarem un nou jugador a l'atzar per a substituir-lo.
    // Returna -1 si no aconseguim trobar cap jugador o, en cas de trobar-lo, 
    // retorna l'index d'on es troba el nou jugador a les llistres globals (listpor...).
    int i_new=0;
    int preu_restant = T - s1.preu;
    s2 = Equip(s1);

    int repeticions=0;  // controlarem les repeticions de cada while( ) per evitar
                        // quedar-nos estancats en casos particulars de la base
                        // de dades o del joc de proves

    preu_restant += old.preu;

    bool found = false;
    if(old.pos=="por"){ // busequem porter
        i_new = rand() % listpor.size();
        while(not found){
            if(not Upor[i_new] and listpor[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listpor[i_new], i_new);
                found = true;
            }
            else i_new = rand() % listpor.size();
            if(repeticions>10000) return -1;
            repeticions++;
        }
    }

    else if(old.pos=="def"){
        i_new = rand() % listdef.size();
        while(not found){
            if(not Udef[i_new] and listdef[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listdef[i_new], i_new);
                found = true;
            }
            else i_new = rand() % listdef.size();
            if(repeticions>10000) return -1;
            repeticions++;
        }
    }
    
    else if(old.pos=="mig"){ 
        i_new = rand() % listmig.size();
        while(not found){
            if(not Umig[i_new] and listmig[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listmig[i_new], i_new);
                found = true;
            }
            else i_new = rand() % listmig.size();
            if(repeticions>10000) return -1;
            repeticions++;
        }
    }

    else if(old.pos=="dav"){
        i_new = rand() % listdav.size();
        while(not found){
            if(not Udav[i_new] and listdav[i_new].preu <= preu_restant){
                s2.canviar_jugador(i_old, listdav[i_new], i_new);
                found = true;
            }
            else i_new = rand() % listdav.size();
            if(repeticions>10000) return -1;
            repeticions++;
        }
    }

    return i_new;
}

void accepta_vei(Equip& s1, const Equip& s2, const Jugador& jold, int i_old, int i_new){
    // actualitzem la solució s1 (s1 = s2) i modifiquem els vector
    // Upor, Udef... en funció del jugador que hem tret i del que hem agafat.

    if(jold.pos == "por"){
        Upor[s1.por] = false;
        Upor[i_new] = true;
    }
    else if(jold.pos == "def"){
        Udef[s1.def[i_old]] = false;

        Udef[i_new] = true;
    }
    else if(jold.pos == "mig"){
        Umig[s1.mig[i_old]] = false;
        Umig[i_new] = true;
    }
    else if(jold.pos == "dav"){
        Udav[s1.dav[i_old]] = false;
        Udav[i_new] = true;
    }

    s1 = Equip(s2);
}


float prob(const Equip& s1, const Equip& s2, int t){
    //distribució de Boltzmann
    if(t < 0.000001) {
        return 0.0;
    }
    return exp(-(s1.punts - s2.punts)*1.0 / t);
}


void simulated_annealing(Equip& E, string fitxer_sortida, auto start){
    Equip s1 = Equip(E);
    Equip s2 = Equip(E);
    float t = 100.0;   //temperatura inicial
    float a = 0.97;    //factor geomètric   
    int iteracions_sense_canvi = 0;  //les iteracions que portem sense canviar de solució 

    if (s1.punts > millors_punts) {
        write_sol(fitxer_sortida, s2, start);
        millors_punts = s1.punts;
    }

    //generador de nombres aleatoris entre [0, 1):
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<double> dis(0.0, 1.0);
    double randomValue;

    while(iteracions_sense_canvi < 30){ // quan portem moltes iteracions amb la mateixa solució, 
                                        // considerem que hem arribat a un màxim local i acabem
                                        // el simulated annealing

        
        

        //busquem veí de s1:
        Jugador jold, jnew;
        int i_old, i_new;

        i_old = tria_jugador(s1, jold);  //triem quin jugador traurem de l'equip
        i_new = tria_vei(s1, s2, jold, i_old); //triem quin jugador agafarem
        
        if(i_new == -1) continue;  //no hem pogut trobar cap veí

        if (s2.punts > s1.punts) {
            if (s2.punts > millors_punts) {
                write_sol(fitxer_sortida, s2, start);
                millors_punts = s2.punts;
            }
            accepta_vei(s1, s2, jold, i_old, i_new);  //actualitzem s1 i els vectors Upor, Udef...
            iteracions_sense_canvi = 0;
        }
        else{
            double p = prob(s1, s2, t);
            randomValue = dis(gen);
            if (randomValue < p) {
                accepta_vei(s1, s2, jold, i_old, i_new); 
                iteracions_sense_canvi = 0;
            }
            else ++iteracions_sense_canvi;
        }

        t = a*t;
    }
};

void grasp(string fitxer_sortida, auto start){

    while(true){
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

    llegir_jugadors(dades_jugadors);
    grasp(fitxer_sortida, start);
}
