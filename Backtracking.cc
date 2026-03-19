#include "../include/Backtracking.hh"
#include "../include/Greedy.hh"
#include <algorithm>
#include <chrono>
#include <utility>
#include <vector>

Backtracking::Backtracking(const Input& in)
             :in(in),out(in),best(in),level_ch_index(in.NetworkSize(),0)
{
   level = -1;
   final_level=in.NetworkSize() - 1;

   Greedy intial_best(in);
   intial_best.DSaturSolver();

   best = intial_best.Solution();

   for (unsigned tx = 0; tx < in.NetworkSize(); ++tx)
      vec_tx.push_back(std::pair<int, unsigned> {in.Degree(tx) , tx});

   std::sort(vec_tx.rbegin(),vec_tx.rend());
}

bool Backtracking::TimerSearch(unsigned timeout_sec)
{
   auto start = std::chrono::steady_clock::now();
   auto now   = std::chrono::steady_clock::now();
   auto timer = std::chrono::seconds(timeout_sec);

   do {
      if (Solution() & Improvement())
      {
         best = out;
         std::cerr << "new best found " << best.TotCost() << std::endl;
      }


      if (Improvement())
         Down();
      else
         Up();
      now = std::chrono::steady_clock::now();

   }while (!AtRoot() && (now - start < timer));

   return AtRoot();
}

void Backtracking::Down()
{
   ++level;
   std::vector<std::pair<Cost, unsigned>> vec_cost;

   for (int ch = 0; ch < in.TotCh(); ++ch)
      vec_cost.push_back(std::pair<Cost, unsigned> {out.ChCost(vec_tx[level].second,ch), ch});

   std::sort(vec_cost.begin(),vec_cost.end());

   bck_trk_mtr.push_back(vec_cost);

   for (level_ch_index[level]=0; in.ChBlocked(vec_tx[level].second, bck_trk_mtr[level][level_ch_index[level]].second); ++level_ch_index[level]);

   out.AssignChCost(vec_tx[level].second, bck_trk_mtr[level][level_ch_index[level]].second, bck_trk_mtr[level][level_ch_index[level]].first);
}

void Backtracking::Up()
{
   do {
      out.RemoveChCost(vec_tx[level].second, bck_trk_mtr[level][level_ch_index[level]].first);
      bck_trk_mtr.pop_back();
      --level;
      if (AtRoot())
         return;

      do {
         ++(level_ch_index[level]);
      }while(in.ChBlocked(vec_tx[level].second, bck_trk_mtr[level][level_ch_index[level]].second));

   }while (level_ch_index[level] == in.TotCh());

   out.AssignChCost(vec_tx[level].second, bck_trk_mtr[level][level_ch_index[level]].second, bck_trk_mtr[level][level_ch_index[level]].first);

}
