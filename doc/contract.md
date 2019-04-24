# Contract

A contract is a message carried and signed by each step from the service to the identity. This allow for example to do an identification process. The first step is the service creating the contract and the last step is the identity (if completed). In-between steps can be anything useful specified by the service/client process.

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
=<signature>CRLF
...step2
=<signature>CRLF
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

### Validity

A contract is valid if:
* there is at least one step/signature
* there is no data after the last step/signature
* each step signature is valid
* (if verified as completed) the first or (then) the last step define `identity` and the last step is signed with this identity

Then, a service will usually add more restrictions, ex:
* check a specific amount of steps (2,3...)
* check its own signature to ensure that the contract is the same
* ...

## Examples of completed contracts

### Simple website login (with HTTPS)

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

### Website login (without HTTPS)

Without HTTPS, stealing/using the contract (identity) would be possible (ex: man in the middle attack).
Imagine that we still want to identify an user, we could add a step (client) to the contract.

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
=<client_signature>
=<identity_signature>
```

The client signature would be generated client-side and the service would check the number of steps when completed.

### Game/app login (without using encryption)

Same as the website login without HTTPS, we would register a local client by generating a private_key (ex: installation) and use it to create the contract with the server/client/identity.

### Local application

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

### Note about client steps

Even with HTTPS, if the LUIS server's machine has malicious programs, they could try to identify themselves to a service. Even if the identity has protections (password prompt or anything else), the user could valid the contract thinking of a legit request. Adding a client step can increase awareness; give more information about the client application to the remote service and to the user for further verifications.
