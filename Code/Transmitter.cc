#include "Transmitter.hh"

Transmitter::Transmitter(nlohmann::json data, TxType type) : type(type)
{
   site = data["site"];
   cell = data["id"];

   if (data["blocked_channels"] != nlohmann::detail::value_t::null)
      for (nlohmann::basic_json<>::size_type i = 0;i < data["blocked_channels"].size(); i++)
         blocked_freq.push_back(static_cast<int>(data["blocked_channels"][i]));
}

std::ostream &operator<<(std::ostream &os, const Transmitter &tx)
{
   os << "site: " << tx.site << std::endl;
   os << "cell: " << tx.cell << std::endl;

   if (tx.type == TxType::bcch)
      os << "type: bcch" << std::endl;
   else
      os << "type: tch" << std::endl;

   os << "blocked freq: [ ";
   for (std::vector<int>::size_type i = 0; i < tx.blocked_freq.size(); i++)
   {
      os << tx.blocked_freq[i];
         if (i < tx.blocked_freq.size() - 1)
            os << ", ";
   }
   os << " ]" << std::endl;

   return os;
}
