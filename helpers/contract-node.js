_sodium = require('libsodium-wrappers');
(async() => {
  await _sodium.ready;
  sodium = _sodium;
})();

// MapData

var MapData = function()
{
  this.map = {};
}

MapData.prototype.set = function(key, value)
{
  this.map[key] = value;
}

MapData.prototype.unset = function(key)
{
  delete this.map[key];
}

MapData.prototype.get = function(key)
{
  return this.map[key];
}

MapData.prototype.has = function(key)
{
  return this.map[key] != null;
}

MapData.prototype.load = function(str)
{
  var lines = str.split("\r\n");
  for(var i = 0; i < lines.length; i++){
    var line = lines[i];
    var pos = line.indexOf(" ");
    if(pos >= 0)
      this.map[line.substr(0, pos)] = line.substr(pos+1);
  }
}

MapData.prototype.write = function()
{
  var str = "";
  for(var key in this.map)
    str += key+" "+this.map[key]+"\r\n";

  return str;
}

// Contract

function Contract()
{
  this.signatures = [];
  this.data_steps = [];
  this.content = "";
}

Contract.prototype.load = function(content)
{
  this.signatures = [];
  this.data_steps = [];
  this.content = content;

  // parse
  var lpos = 0; // begin cursor
  var spos; // end cursor
  while((spos = content.indexOf("\r\n=", lpos)) != -1){
    var data_step = content.substr(lpos, spos+2-lpos);

    lpos = spos+3;

    // find signature
    var end_sign = content.indexOf("\r\n", lpos);
    if(end_sign != -1){
      var data_sign = sodium.from_hex(content.substr(lpos, end_sign-lpos));

      // add data
      var mdata = new MapData();
      this.data_steps.push(mdata);
      mdata.load(data_step);

      // add signature
      this.signatures.push([spos+2, data_sign]);

      // next step
      lpos = end_sign+2;
    }
  }
}

Contract.prototype.write = function()
{
  return this.content;
}

Contract.prototype.verify = function(complete)
{
  if(this.signatures.length == 0) // invalid without signatures
    return false;

  // invalid if there are data after the last signatures
  var endsign = this.signatures[this.signatures.length-1];
  if(endsign[0]+endsign[1].length*2+3 != this.content.length)
    return false;

  // check each signatures
  for(var i = 0; i < this.signatures.length; i++){
    var end = this.signatures[i][0];
    var sign = this.signatures[i][1];
    if(i <= this.data_steps.length){
      var data_step = this.data_steps[i];

      var public_key;

      // last step and closed, find identity
      if(complete && i == this.signatures.length-1){ 
        if(this.data_steps[0].has("identity")) // identity in first contract step
          public_key = sodium.from_hex(this.data_steps[0].get("identity"));
        else if(data_step.has("identity")) // identity last contract step
          public_key = sodium.from_hex(data_step.get("identity"));
      }
      else if(data_step.has("public_key"))
        public_key = sodium.from_hex(data_step.get("public_key"));

      // check
      if(sign.length == 64 && public_key.length == 32){
        if(!sodium.crypto_sign_verify_detached(sign, this.content.substr(0, end), public_key))
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

Contract.prototype.next = function()
{
  if(this.data_steps.length == this.signatures.length){
    var mdata = new MapData();
    this.data_steps.push(mdata);
    return mdata;
  }
}

Contract.prototype.sign = function(private_key)
{
  if(this.data_steps.length > this.signatures.length && private_key.length == 64){
    // add step data to content
    var data_step = this.data_steps[this.data_steps.length-1];
    var data = data_step.write();
    this.content += data;

    // sign content
    var sign = sodium.crypto_sign_detached(this.content, private_key);

    // add signature
    this.signatures.push([this.content.length, sign]);
    this.content += "="+sodium.to_hex(sign)+"\r\n";
    return true;
  }

  return false;
}

module.exports = Contract;
