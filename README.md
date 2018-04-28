
# LUIS

## Idea

The goal of LUIS is to create a simple service (a local server) listening to HTTP requests to identify a local user to any client/system (TCP/HTTP capable) that requires user identification.
This is a middle step attempt before having a true physical "universal" identification system (see [ideas](https://github.com/ImagicTheCat/ImproveSociety/blob/master/ideas/security/identification/universal_card.md)).

The same identity could be used to register/login on any website/application/game. Following a protocol using an asymmetric encryption signature algorithm, the user will be able to verify the service and the service will identify the user. No more password, except an optional local password to encrypt the private key for more security. 

Instead of managing the whole registration process and passwords, services will only follow the simple LUIS protocol to identify users and manage the public keys.

Of course, in a scenario where only one identity is used for multiple services, if the identify is compromised, all the services will be. But this is no different from the current situation where most people use the same password on every service, or use weak passwords to remember them. Furthermore, nothing guarantee you that a service is actually hashing the password and not keeping it for itself. The services will know less informations that can help to compromise the user identity, so the failure point is now the user, instead of being the service AND the user.

LUIS must be simple as possible, as any security features to reduce the attack surface, while still being fast and simple to use in many scenarios (headless, GUI, ...).

## Basic protocol 

### Example procedure for identification

* an user try to connect to a service with a specific identity (public key) or let the choice of the identity (anonymous mode, identity will be added by LUIS)
* the service create a contract with custom data to identify this connection (timestamp, id, ...) and sign the contract
* the client get the contract (add a step if necessary) and gives it to LUIS
* LUIS receives the contract, checks the signature and informs (show service and client public key) / asks the user (external prompt, ...) for a password to unlock the private key
* user denies/accepts/enters password
* LUIS signs the contract and returns it
* the client receives the signed contract, gives it to the service
* the service checks the contract, checks the identity key, checks its own signature (for integrity) and checks if the content is valid for this connection

An identification is valid in a specific context, so it's up to the service to provide the minimum amount of contextual data in the original message to prevent stealing/reuse of the connection "ticket".

## Notes

* use libsodium signature (Ed25519)
* use libsodium pwhash (Argon2) and secret box (XSalsa20-Poly1305) for the private_key encryption with a passphrase (SENSITIVE level for OPS and MEMORY)
* no interface dependency, the server will call external programs to prompt things

## Config options

* `data-dir`: data directory (ex: `/var/lib/luis`)
