
#ifndef H_CONTRACT
#define H_CONTRACT

#include "tools.hpp"
#include "MapData.hpp"
#include <sodium.h>

// load, add data, sign, write contract
class Contract{
  public:
    void load(const std::string &data);
    void write(std::string &data){ data = content; }

    // verify the entire contract
    // complete: verify the contract as completed (signed by identity)
    bool verify(bool complete) const;

    // add a step if the previous one is already signed (or return the last)
    MapData& next()
    {
      if(data_steps.size() == signatures.size())
        data_steps.push_back(MapData());
      return data_steps[data_steps.size()-1];
    }

    // sign the last added step, update contract content
    // return true on success (meaning the content is modified, a post-verification is recommended)
    bool sign(const std::string& public_key, const std::string& private_key);

    const std::vector<std::pair<size_t, std::string> >& getSignatures()const{ return signatures; }
    const std::vector<MapData>& getDataSteps()const{ return data_steps; }

  private:
    std::string content;
    std::vector<MapData> data_steps;
    std::vector<std::pair<size_t, std::string> > signatures; // start pos / buf signature
};

#endif
