//file network.hpp

#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <iostream>
#include <vector>
#include <string>
#include "json.hpp"

enum class transmitter_type {bcch, tch}; 

//uso principalmente oggetti pieni è accettabile?

class CellRelation{
   friend std::ostream& operator<<(std::ostream& os, const CellRelation& r);
   public:
      CellRelation(nlohmann::json relation);
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
      void AddRelation(nlohmann::json rel);

   private:
      
      //the frequency used in the class aren't the one actually use, there is an offset for an easier use of the freq_cost vector

      int assigned_freq;
      std::string id_cell;
      std::string site;
      transmitter_type type;
      std::vector<double> freq_cost;
      std::vector<CellRelation> relations;
};

//testare se ci sono casi in cui max e min interference sono violate

class Network{
   friend std::ostream& operator<<(std::ostream& os, const Network& N);
   public:

      Network(nlohmann::json data);
      //potrei chiudere tutto in uno?
      void AddTransmitters(nlohmann::json cells);
      void AddRelations(nlohmann::json relations);

   private:

      int bcch_bcch;
      int bcch_tch;
      int tch_bcch;
      int tch_tch;
      
      int min_freq;
      int max_freq;

      int co_site_separetion;
      int co_cell_separetion;

      std::vector<Transmitter> transmitters;
};
#endif