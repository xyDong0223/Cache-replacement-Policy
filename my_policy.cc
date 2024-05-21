#include "mem/cache/replacement_policies/my_policy.hh"

#include <cassert>
#include <algorithm>
#include <random>

#include "params/EBRRP.hh"
#include "sim/cur_tick.hh"

namespace gem5
{

namespace replacement_policy
{

EBR::EBR(const Params &p)
  : Base(p)//, associativity(p.associativity),
           //missCounter(p.associativity, 0) 
{
    // Initialize miss counter for each set
}

void
EBR::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    // Cast and reset counters
    std::static_pointer_cast<EBRReplData>(replacement_data)->recencyCounter = 0;
    std::static_pointer_cast<EBRReplData>(replacement_data)->frequencyCounter = Tick(0);

}

void
EBR::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    // Cast and update recency counter
    std::static_pointer_cast<EBRReplData>(replacement_data)->recencyCounter++;
    std::static_pointer_cast<EBRReplData>(replacement_data)->frequencyCounter = curTick();
}

void
EBR::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
	// Same as invalidate for this policy
    std::static_pointer_cast<EBRReplData>(replacement_data)->recencyCounter = 1;
    std::static_pointer_cast<EBRReplData>(replacement_data)->frequencyCounter = curTick();
}

ReplaceableEntry*
EBR::getVictim(const ReplacementCandidates& candidates) const
{
    assert(!candidates.empty());

    // Search for the block with the smallest rank (based on your ranking algorithm)
    // Initial assumption: the first candidate has the smallest rank
    ReplaceableEntry* victim = candidates[0];
    auto victim_data = std::static_pointer_cast<EBRReplData>(victim->replacementData);
    int smallest_rank = victim_data->recencyCounter; // If recency determines rank
    // or use some function to calculate rank based on both recency and frequency counters

    // Iterate through the candidates to find the one with the smallest rank
    for (const auto& candidate : candidates) {
        auto data = std::static_pointer_cast<EBRReplData>(candidate->replacementData);
        
        // Calculate the rank for the current candidate
        int current_rank = data->recencyCounter; // or use a ranking function
        
        // Compare the current candidate's rank with the smallest rank found so far
        if (current_rank < smallest_rank) {
            smallest_rank = current_rank;
            victim = candidate;
        }
    }

    // If there is a tie in the smallest rank, choose a random victim from those
    std::vector<ReplaceableEntry*> smallest_rank_candidates;
    for (const auto& candidate : candidates) {
        auto data = std::static_pointer_cast<EBRReplData>(candidate->replacementData);
        if (data->recencyCounter == smallest_rank) {
            smallest_rank_candidates.push_back(candidate);
        }
    }
    
    if (smallest_rank_candidates.size() > 1) {
        // Choose randomly among the candidates with the smallest rank
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, smallest_rank_candidates.size() - 1);
        victim = smallest_rank_candidates[dis(gen)];
    }

    // Now that the victim is chosen, reset its counters
    auto victim_repl_data = std::static_pointer_cast<EBRReplData>(victim->replacementData);
    victim_repl_data->recencyCounter = 0;
    victim_repl_data->frequencyCounter = 0;

    return victim;
}


std::shared_ptr<ReplacementData>
EBR::instantiateEntry()
{
    return std::make_shared<EBRReplData>();
}

} // namespace replacement_policy
} // namespace gem5
