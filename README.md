
# LUIS

## Idea

The goal of LUIS is to create a simple service (a local server) listening to HTTP requests to identify a local user to any client/system (TCP/HTTP capable) that requires user identification.
This is a middle step attempt before having a true physical "universal" identification system (see [ideas](https://github.com/ImagicTheCat/ImproveSociety/blob/master/ideas/security/identification/universal_card.md)).

The same identity could be used to register/login on any website/application/game. Following a protocol using an asymmetric encryption signature algorithm, the user will be able to verify the service and the service will identify the user. No more password, except an optional local password to encrypt the private key for more security. 

Instead of managing the whole registration process and passwords, services will only follow the simple LUIS protocol to identify users and manage the public keys.

Of course, in a scenario where only one identity is used for multiple services, if the identify is compromised, all the services will be. But this is no different from the current situation where most people use the same password on every service, or use weak passwords to remember them. Furthermore, nothing guarantee you that a service is actually hashing the password and not keeping it for itself. The services will know less informations that can help to compromise the user identity, so the failure point is now the user, instead of being the service AND the user.

LUIS must be simple as possible, as any security features to reduce the attack surface, while still being fast and simple to use in many scenarios (headless, GUI, ...).

## Basic protocol 

### Client/service key registration

* a client/service asks to register itself (associate a name to the public key), the message is signed
* the user accept or deny (by comparing with the original client/service public key and entering a code returned to the client)
  * accept: the client/service is now accepted for requests
  * deny: the client/service is rejected for the next requests

If the client/service key is already registered, will do nothing and answer without errors.

### Identification / contract

example:

* an user try to connect to a service with a specific identity (public key) or let the choice of the identity (anonymous mode, identity will be added by LUIS)
* the client makes an HTTP POST request to LUIS, creating a contract with custom data to identify this connection (timestamp, id, ...) and sign the contract
* LUIS receives the contract, checks the signature and informs (show service and client public key) / asks the user (external prompt, ...) for a password to unlock the private key
* user denies/accepts/enters password
* LUIS adds a timestamp to the contract, signs it and returns it
* the client receives the signed contract, checks the user signature, checks its own signature and checks if the content is valid for this connection

An identification is valid in a specific context, so it's up to the service to provide the minimum amount of contextual data in the original message to prevent stealing/reuse of the connection "ticket".

## Contract

A contract is a message carried and signed by each node/step from the service to the identity. This allow to do an identification process with possible encryption at every step. The first node is the service/client initializing the contract, the before last node is the client and the last node is the identity.

### Examples of complete contracts


#### Simple website login (with https)

Without https, stealing/using the contract (identity) would be possible (ex: man in the middle).
Also, we can't really encrypt data using the identity and Ed25519 in a secure way in this case because we don't have any client to rely on to verify a temporary private_key (see example without https).

```
version 1
title <contract_title>
public_key <site_public_key>
identity <identity_public_key>
timestamp <timestamp>
id <random_id>
=<site_signature>
timestamp <luis_timestamp>
=<identity_signature>
```

#### Website login (without https)

Imagine that we want to create a secure webapp without https (not easy and probably not recommended, but why not), we could add a step (client node) to the contract.

```
version 1
title <contract_title>
public_key <site_public_key>
identity <identity_public_key>
timestamp <timestamp>
id <random_id>
=<site_signature>
public_key <client_public_key>
encryption_key <tmp_public_encryption_key>
=<client_signature>
timestamp <luis_timestamp>
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
identity <identity_public_key>
timestamp <timestamp>
=<app_signature>
timestamp <luis_timestamp>
=<identity_signature>
```

## Notes

* use Ed25519 for signatures
* use OpenSSL for more general tasks
* no interface dependency, the server will call external programs to prompt things

## Config options

* `data-dir`: data directory (ex: `/var/lib/luis`)
