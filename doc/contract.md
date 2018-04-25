# Contract

A contract is a message carried and signed by each step from the service to the identity. This allow for example to do an identification process. The first step is the service/client creating the contract, the before last step is the client (if one exists) and the last step is the identity.

A contract is generic and not specifically designed for identification. For example, changing some critical account informations on a web site could require to sign a higher level contract. We could also generate a persistent contract to verify a public information (uploaded content, signature...), bound to a specific service.

## Format

### Steps

The contract steps use a simple "map data" format to define a set of key/values:
```
<key1>SPACE<value1>CRLF
<key2>SPACE<value2>CRLF
...
```

The key can't contains `SPACE`, `CR` or `LF` and the value can't contains `CR` or `LF`.

### Signatures

Each step added to the contract should be signed (sign all the contract data before the signature) with the value of `public_key` from that step, at the exception of the last step when verifying the contract as completed; in this case, the public key is stored in `identity` from the first step (creator of the contract) or (then) from the last step (LUIS step).

Signatures and public keys are stored in plain hexadecimal (without uppercase).

```
...step1
=<signature>
...step2
=<signature>
...
```

### Used contract keys

Any key/value can be added at any step of the contract, but some of them are required to have a valid contract.

* every step except for the LUIS step
  * `public_key`: public_key used to sign this step (to identify each part)
  * `name` (optional): name to use for this public_key (possible size/characters limitations)
* creation step (first)
  * `version`: the contract version (`1` right now, can change if a vulnerability is discovered in the signature algorithm)
  * `title` (optional): title of the contract, ex: "Login" (possible size/characters limitations)
* creation or LUIS step
  * `identity`: public_key of the LUIS identity (user), if not added by the contract creator the user will be able to choose an identity and add it to the final step


## Examples of completed contracts

### Simple website login (with https)

Without https, stealing/using the contract (identity) would be possible (ex: man in the middle).
Also, we can't really encrypt data using the identity and Ed25519 in a secure way in this case because we don't have any client to rely on to verify a temporary private_key (see example without https).

```
version 1
title <contract_title>
public_key <site_public_key>
name <service_name>
identity <identity_public_key>
timestamp <timestamp>
id <connection_id>
=<site_signature>
=<identity_signature>
```

### Website login (without https)

Imagine that we want to create a secure webapp without https (not easy and probably not recommended), we could add a step (client) to the contract.

```
version 1
title <contract_title>
public_key <site_public_key>
name <service_name>
identity <identity_public_key>
timestamp <timestamp>
id <connection_id>
=<site_signature>
public_key <client_public_key>
name <client_name>
encryption_key <tmp_public_encryption_key>
=<client_signature>
=<identity_signature>
```

If we forget cookies and generate a localStorage private_key (that will be registered to LUIS) as the client and a temporary private_key for the encryption, we can now share a secret and use symmetric encryption between the client (the web page) and the service (the server).
If a man in the middle use the contract, since he doesn't have the client private_key and the encryption private_key, he can't modify the contract and can't communicate properly with the server.

#### Game/app login/encryption (without using https)

Same as the website login without https, we would register a local client by generating a private_key (ex: installation) and use it to create the contract with the server/client/identity (adding the temporary public_key for encryption).

#### Local application

Even in local, making the contract unique (timestamp, id...) is relevant: another user looking at TCP packets could save the contract for later use.

```
version 1
title <contract_title>
public_key <local_app_public_key>
name <local_app_name>
identity <identity_public_key>
timestamp <timestamp>
=<app_signature>
=<identity_signature>
```


