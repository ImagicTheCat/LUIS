
#include "Contract.hpp"

void Contract::load(const std::string &data)
{
  // init
  content = data;
  data_steps.clear();

  // parse
  size_t lpos = 0; // begin cursor
  size_t spos; // end cursor
  while((spos = content.find("\r\n=", lpos)) != std::string::npos){
    std::string data_step(content.substr(lpos, spos+2-lpos));

    lpos = spos+3;

    // find signature
    size_t end_sign = content.find("\r\n", lpos);
    if(end_sign != std::string::npos){
      std::string data_sign(hex2buf(content.substr(lpos, end_sign-lpos)));

      // add data
      data_steps.push_back(MapData());
      MapData& mdata = data_steps[data_steps.size()-1];
      mdata.load(data_step);

      // add signature
      signatures.push_back(std::pair<size_t, std::string>(spos+2, data_sign));

      // next step
      lpos = end_sign+2;
    }
  }
}

bool Contract::verify(bool complete) const
{
  if(signatures.size() == 0) // invalid without signatures
    return false;

  // invalid if there are data after the last signatures
  const std::pair<size_t,std::string> &endsign = signatures[signatures.size()-1];
  if(endsign.first+endsign.second.size()*2+3 != content.size())
    return false;

  // check each signatures
  for(size_t i = 0; i < signatures.size(); i++){
    const size_t& end = signatures[i].first;
    const std::string& sign = signatures[i].second;
    if(i <= data_steps.size()){
      const MapData& data_step = data_steps[i];

      std::string public_key;

      // last step and closed, find identity
      if(complete && i == signatures.size()-1){ 
        if(data_steps[0].has("identity")) // identity in first contract step
          public_key = hex2buf(data_steps[0].get("identity"));
        else if(data_step.has("identity")) // identity last contract step
          public_key = hex2buf(data_step.get("identity"));
      }
      else if(data_step.has("public_key"))
        public_key = hex2buf(data_step.get("public_key"));

      // check
      if(sign.size() == 64 && public_key.size() == 32){
        if(crypto_sign_verify_detached((const unsigned char*)sign.c_str(), (const unsigned char*)content.c_str(), end, (const unsigned char*)public_key.c_str()) != 0)
          return false;
      }
      else
        return false;
    }
    else
      return false;
  }

  return true;
}


bool Contract::sign(const std::string& private_key)
{
  if(data_steps.size() > signatures.size() && private_key.size() == 64){
    // add step data to content
    MapData& data_step = data_steps[data_steps.size()-1];
    std::string data;
    data_step.write(data);
    content += data;

    // sign content
    unsigned char signature[64];
    crypto_sign_detached(signature, NULL, (const unsigned char*)content.c_str(), content.size(), (const unsigned char*)private_key.c_str());

    // add signature
    signatures.push_back(std::pair<size_t, std::string>(content.size(), std::string((char*)signature, 64)));
    content += "="+buf2hex((const char*)signature, 64)+"\r\n";
    return true;
  }

  return false;
}
