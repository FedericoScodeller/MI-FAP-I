#include "relation.hpp"

//RELATION METHOD AND FUNCTION
Relation::Relation(int tx_id, int sep)
{
    tx=tx_id;
    handover=false;
    separetion=sep;
    interf_same_ch=0.0;
    interf_adjacent_ch=0.0;
}

Relation::Relation(nlohmann::json cell_rel,int tx_id, int sep)
{
    /*qui ci sono vari casi per tenere conto di tutte le combinazioni di valori che si possono trovare
    nel file .json, per altre informazioni consultare il readme file*/
    tx=tx_id;

    if (cell_rel["separetion"] == nlohmann::detail::value_t::null)
    {
        if (cell_rel["handover"]== nlohmann::detail::value_t::null)
        {
            handover=false;
            separetion=0;
        }
        else
        {
            handover=true;
            separetion=sep;
        }
        
        if (cell_rel["downlink_area_interference"]["same_channel"]== nlohmann::detail::value_t::null)
        {
            interf_same_ch=0.0;
        }
        else
        {
            interf_same_ch=cell_rel["downlink_area_interference"]["same_channel"];
        }

        if (cell_rel["downlink_area_interference"]["adjacent_channel"]== nlohmann::detail::value_t::null)
        {
            interf_adjacent_ch=0.0;
        }
        else
        {
            interf_adjacent_ch=cell_rel["downlink_area_interference"]["adjacent_channel"];
        }
        
    }
    else
    {
        handover=false;
        separetion=cell_rel["separetion"];
        interf_same_ch=cell_rel["downlink_area_interference"]["same_channel"];
        interf_adjacent_ch=cell_rel["downlink_area_interference"]["adjacent_channel"];
    }
    
    
}

void Relation::ModifyRel(Relation data)
{
    if ((*this).tx != data.Tx())
    {
        std::cout << "ERROR" <<std::endl;
        exit(1);
    }
    
    (*this).handover=data.Handover();
    (*this).separetion=std::min((*this).separetion, data.Separetion());
    (*this).interf_same_ch=data.InterfSameCh();
    (*this).interf_adjacent_ch=data.InterfAdjacentCh();
}

std::ostream& operator<<(std::ostream& os, const Relation& r)
{
   os << "from tx: " << r.tx << std::endl;
   if (r.handover)
   {
      os << "handover: true "<< std::endl;
   }
   else
   {
      os << "handover: false "<< std::endl;
   }
   os << "separetion: " << r.separetion << std::endl;
   os << "same_channel_interference: " << r.interf_same_ch << std::endl;
   os << "adjacent_channel_interference: " << r.interf_adjacent_ch << std::endl;
   return os;
}