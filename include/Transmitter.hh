#ifndef TRANSMITTER_HH
#define TRANSMITTER_HH
#include "../libs/json.hh"

enum class TxType {bcch, tch};

class Transmitter
{
   protected:
      std::string site;
      std::string cell;
      TxType type;
      std::vector<unsigned> blocked_freq;

   public:
      Transmitter(nlohmann::json data, TxType type);

      std::string Site(void) const {return site;};
      std::string Cell(void) const {return cell;};
      TxType Type(void) const {return type;};
      std::vector<unsigned> BlockedFreq(void) const {return blocked_freq;}; //RVO does an efficient return by value!


	friend std::ostream& operator<<(std::ostream& os, const Transmitter& tx);

};
#endif
