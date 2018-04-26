
window.sodium = {
onload: function()
{
  // generate keypair (testing purpose)
  var kp = sodium.crypto_sign_keypair();

  // create test contract
  var contract = "version 1\r\n";
  contract += "name test\r\n";
  contract += "public_key "+sodium.to_hex(kp.publicKey)+"\r\n";

  // custom data
  contract += "timestamp "+(new Date().getTime())+"\r\n";

  // sign contract
  contract += "="+sodium.to_hex(sodium.crypto_sign_detached(contract, kp.privateKey))+"\r\n";

  // CAN verify contract before sending it (avoid leaking informations if the signing process has failed)

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
