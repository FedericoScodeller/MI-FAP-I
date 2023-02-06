#include "transmitter.hpp"


//TRANSMITTOR METHOD AND FRIEND FUNCTION
Transmitter::Transmitter(nlohmann::json cell_info, transmitter_type channel)
{
    freq=-1; 
    cell = cell_info["id"]; 
    site = cell_info["site"]; 
    type = channel;

    if (cell_info["blocked_channels"] != nlohmann::detail::value_t::null)
    {
        for (nlohmann::basic_json<>::size_type i=0; i < cell_info["blocked_channels"].size();i++)
        {
            int j = cell_info["blocked_channels"][i]; 
            blocked_channels.push_back(j);
        }  
    }
}


std::ostream& operator<<(std::ostream& os, const Transmitter& t)
{
    /*come in network non usalo se non servono informzioni dettagliate*/
    os << "freq: " << t.freq << std::endl;
    os << "id: " << t.cell << std::endl;
    os << "site: " << t.site << std::endl;

    if (t.type == transmitter_type::bcch)
        os << "type: bcch" << std::endl;
    else
        os << "type: tch" << std::endl;

    os << "blocked_channels: [";
    for (std::vector<int>::size_type i = 0; i < t.blocked_channels.size(); i++)
        os << t.blocked_channels[i]<< ", ";
    os << "]" << std::endl<< std::endl<< std::endl;

    return os;
}
