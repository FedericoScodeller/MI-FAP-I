#ifndef GREEDY_HH
#define GREEDY_HH


#include "Input.hh"
#include "Output.hh"
#include <vector>

//anche qui uso RVO, implementato da compilatori moderni e versioni più recenti dello std c++ (e per recenti intendo 2011), se non disponibile va cambiate alcune cose nei return dei vector
class Greedy
{
   public:
      Greedy(const Input& in):in(in),out(Output(in)){};

      Output Solution(void) const {return out;};

      void GreedySolver(void);
      void DegreeSolver(void);
      void DSaturSolver(void);
      void RegretSolver(void);

      void ResetSolver(void){out.Reset();};

   protected:
      const Input& in;
      Output out;

      int BestCh4Tx(unsigned tx) const;
      int MaxSatur(const std::vector<unsigned>& satur_vec) const;
      int NextTxRegret(const std::vector<Cost>& vector_delta_cost, const std::vector<unsigned>& vector_satur) const;
      void UpdateSatur(std::vector<unsigned>& satur_vec, std::vector<std::vector<bool>>& mat_blk_ch, int tx_updated, const std::vector<unsigned>& update_list) const;
      void UpdateRegretData(std::vector<std::vector<Cost>>& matrix_cost, std::vector<Cost>& vector_delta_cost, std::vector<unsigned>& vector_satur, int tx_updated, const std::vector<unsigned>& update_list) const;

};

#endif
