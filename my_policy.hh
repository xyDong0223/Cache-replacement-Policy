#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_EBR_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_EBR_RP_HH__

#include "mem/cache/replacement_policies/base.hh"
#include <vector>

namespace gem5
{
	struct EBRRPParams;
namespace replacement_policy
{

class EBR : public Base
{
  private:
    struct EBRReplData : public ReplacementData
    {
        unsigned recencyCounter;
        unsigned frequencyCounter;

        EBRReplData() : recencyCounter(0), frequencyCounter(0) {}
    };

    unsigned associativity;
    std::vector<unsigned> missCounter;

  public:
    EBR(const Params &p);
    void invalidate(const std::shared_ptr<ReplacementData>&) override;
    void touch(const std::shared_ptr<ReplacementData>&) const override;
    void reset(const std::shared_ptr<ReplacementData>&) const override;
    ReplaceableEntry* getVictim(const ReplacementCandidates&) const override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;
};

} // namespace replacement_policy
} // namespace gem5

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_EBR_RP_HH__
