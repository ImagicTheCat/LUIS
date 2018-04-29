var Contract = require("../../helpers/contract-node.js");
const _sodium = require("libsodium-wrappers");
var http = require("http");

(async() => {
  try{

  await _sodium.ready;
  const sodium = _sodium;

  // generate keypair (testing purpose)
  var kp = sodium.crypto_sign_keypair();

  // create test contract
  var contract = new Contract();
  var mdata = contract.next();
  mdata.set("version", "1");
  mdata.set("title", "Test Contract");
  mdata.set("name", "test");
  mdata.set("public_key", sodium.to_hex(kp.publicKey));

  // custom data
  mdata.set("timestamp", new Date().getTime()+"");

  // sign contract
  contract.sign(kp.privateKey);

  // verify contract before sending it

  if(contract.verify(false)){
    console.log("Contract: \n\n"+contract.write());
    var req = http.request({
      host: "localhost",
      port: "25519",
      path: "/contract",
      method: "POST",
      headers: {
        "Content-Length": Buffer.byteLength(contract.write())
      }
    }, function(res){
      res.on("data", function(chunk){
        console.log("Response: \n\n"+chunk);

        var rcontract = new Contract();
        rcontract.load(chunk.toString());
        console.log((rcontract.verify(true) ? "CONTRACT VALID" : "CONTRACT INVALID"));
      });
    });

    req.write(contract.write());
    req.end();
  }
  else
    console.log("created contract is invalid");

  }catch(e){ console.log(e); }
})();
