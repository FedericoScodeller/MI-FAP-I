//file network.hpp

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include "json.hpp"

enum class transmitter_type {bcch, tch}; 

//uso principalmente oggetti pieni è accettabile?
//se l'interpretazione del handover è sbagliata basta invertire il from e to 
class CellRelation{
   friend std::ostream& operator<<(std::ostream& os, const CellRelation& r);
   public:
      CellRelation(nlohmann::json relation);
      std::string CellId(void){return cell_bonded;};
      double SameChannel(void){return same_channel;};
      double AdjacentChannel(void){return adjacent_channel;};
      bool Handover(void){return handover;};
   private:
      std::string cell_bonded;
      bool handover;
      double same_channel;
      double adjacent_channel;
};

class Transmitter{
   friend std::ostream& operator<<(std::ostream& os, const Transmitter& t);
   public:
      
      Transmitter(nlohmann::json cell, transmitter_type channel,int n_freq, int offset = 0);
      std::string IdCell(void){ return id_cell; };
      std::string Site(void){ return site; };
      void AddRelation(nlohmann::json rel);
      int AssignedFreq(void){ return assigned_freq;};
      void UpdateFreqCost(int freq, double cost);//la voglio più raffinata questa sostituisce il costo voglio una che sommi o crei qualcosa di più complesso
      transmitter_type Type(void){return type;};
      bool Handover(int r){return relations[r].Handover();};
      double SameChannel(int r){return relations[r].SameChannel();};
      double AdjacentChannel(int r){return relations[r].AdjacentChannel();};
      //METODI PER LA SOLUZIONE

      int Degree(void);
      void AssignFreq(void); //mabey I shuold rename it just to avoid confusion
      int FindRelation(std::string id);
   private:
      
      //the frequency used in the class aren't the one actually use, there is an offset for an easier use of the freq_cost vector

      int assigned_freq;
      std::string id_cell;
      std::string site;
      transmitter_type type;
      std::vector <double> freq_cost;
      std::vector <CellRelation> relations;
};

//testare se ci sono casi in cui max e min interference sono violate

class Network{
   friend std::ostream& operator<<(std::ostream& os, const Network& N);
   public:

      Network(nlohmann::json data);
      //potrei chiudere tutto in uno?
      void AddTransmitters(nlohmann::json cells);
      void AddRelations(nlohmann::json relations);
      int NFreqAvailable(void){return max_freq-min_freq+1;};
      
      //metodi per la soluzione del MI-FAP
      
      //void AssignFreq_mk1(void);
      void AssignFreq_mk2(void);
      int LargestDegree(void);
      void UpdateCost(int changed_transmitter, int changed_freq);


   private:

      int bcch_bcch;
      int bcch_tch;
      int tch_bcch;
      int tch_tch;
      
      int min_freq;
      int max_freq;
//NON HAI TENUTO CONTO DEI CANALI BLOCCATI GLOBALMENTE!!!
      int co_site_separetion;
      int co_cell_separetion;

      std::vector<Transmitter> transmitters;
};
#endif