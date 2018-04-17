
# LUIS

## Idea

The goal of LUIS is to create a simple service (a local server) listening to HTTP requests to identify a local user to any client/system (TCP/HTTP capable) that requires user identification.
This is a middle step attempt before having a true physical "universal" identification system (see [ideas](https://github.com/ImagicTheCat/ImproveSociety/blob/master/ideas/security/identification/universal_card.md)).

The same identity could be used to register/login on any website/application/game. Following a protocol using an asymmetric encryption signature algorithm, the user will be able to verify the service and the service will identify the user. No more password, except an optional local password to encrypt the private key for more security. 

Instead of managing the whole registration process and passwords, services will only follow the simple LUIS protocol to identify users and manage the public keys.

Of course, in a scenario where only one identity is used for multiple services, if the identify is compromised, all the services will be. But this is no different from the current situation where most people use the same password on every service, or use weak passwords to remember them. Furthermore, nothing guarantee you that a service is actually hashing the password and not keeping it for itself. The services will know less informations that can help to compromise the user identity, so the failure point is now the user, instead of being the service AND the user.

LUIS must be simple as possible, as any security features to reduce the attack surface, while still being fast and simple to use in many scenarios (headless, GUI, ...).

## Basic protocol 

### Service registration

* a service asks to register itself (associate a name to the public key), the message is signed
* the user accept or deny (by comparing with the original service public key)
  * accept: the service is now accepted for requests
  * deny: the service is rejected for the next requests

If the service is already registered, will do nothing and answer without errors.

### Choose identity

* a service asks to let the user choose an identity (not signed, no checks)
* user denies/choose an identity (returns the public key)

Useful for user registration or direct "anonymous" login.

### Identification

* an user try to connect to a service with a specific identity (public key)
* the service makes an HTTP POST request to LUIS, passing a body with the public key followed by custom data to identify this connection (timestamp, id, ...), then the body is signed with the service private key
* LUIS receives the request, checks the signature and informs (show service public key) / asks the user (external prompt, ...) for a password to unlock the private key
* user denies/accepts/enters password
* LUIS adds a timestamp to the service signed message, signs it and returns it
* the service receives the double signed message, checks the user signature, checks its own signature and checks if the content is valid for this connection

An identification is valid in a specific context, so it's up to the service to provide the minimum amount of contextual data in the original message to prevent stealing/reuse of the connection "ticket".
