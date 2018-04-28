
window.sodium = {
onload: function()
{
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
    var h = document.createElement("h1");
    h.innerHTML = "Contract";
    document.body.appendChild(h);

    var pre = document.createElement("pre");
    pre.innerText = contract;
    document.body.appendChild(pre);

    var h = document.createElement("h1");
    h.innerHTML = "Response";
    document.body.appendChild(h);

    // send contract to LUIS
    fetch("http://localhost:25519/contract", {
      method: "POST",
      body: contract
    }).then(function(r){
      if(r.ok){
        r.text().then(function(body){
          var pre = document.createElement("pre");
          pre.innerText = body;
          document.body.appendChild(pre);
        });
      }
      else{
        var pre = document.createElement("pre");
        pre.innerText = "error, status: "+r.status;
        document.body.appendChild(pre);
      }
    }).catch(function(e){
      var pre = document.createElement("pre");
      pre.innerText = JSON.stringify(e);
      document.body.appendChild(pre);
    });
  }
}
}
