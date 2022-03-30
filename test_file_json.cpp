#include <iostream>
#include "json.hpp"
#include <fstream>
using namespace std;
using json = nlohmann::json;
#define NONE nlohmann::detail::value_t::null //uso la macro ma non ci sono alternative, come null=nlohmann::detail::value_t::null 
                                             //che non mi diano errore e perche mi danno errore?

//questo è un test per vedere come gestire ed effettuare gli accessi al file json

int main(){
   ifstream is;
   json j;
   int x;

   is.open("Tiny.json");
   is >> j;

   //test accesso a un normale value

   x=j["general_information"]["handover_separation"]["bcch->bcch"];
   cout << "Il valore visualizzato è bcch->bcch e deve essere 2: " << x << endl;
   
   // test per l'accesso al vettore celle o relazioni celle
   cout << "I valori visualizzati sono tutti gli id deve essere '1''2''3''4''5''6''7': ";
   
   for(auto& cell: j["cells"])
      cout << cell["id"];

   cout << endl;
   
   // questo test è per vedere come gestire valori opzionali e vettori di interi

   cout << "I valori visualizzati sono i canali bloccati e devono essere [5,6][13]: ";

   for(auto& cell: j["cells"]){
      if(cell["blocked_channels"] != NONE){
           cout << cell["blocked_channels"];
      }
   }

   cout << endl;
   


   is.close();
   return 0;
}
