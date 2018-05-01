
# Config

The config file is located by default at `/etc/luis/luis.conf` for GNU/Linux and at `<exec_path>/luis.conf` for Windows.

This path can be modified with the `--cfg <path>` command option.

## Options

Every config option can be modified in the command line using `--cfg-<option> <value>`.
The config file use the simple "map data" format (like the contract).

### `data-dir`

Path of the data directory (without end `/`, should have `identities` and `keys` directories, they are not created).
If not set, will be `<exec_path>/data` under Windows.

### `cmd-password-term`

Command to ask a password in a terminal, should print nothing in the terminal and write the typed password to stdin.
You can look at a `sh` example [here](../rsc/cmd_password_term.sh).

### `cmd-contract`

Command to review a LUIS contract (ex: `luis-gui-contract`, Qt GUI).

Command data is encoded in hexadecimal and passed as the first argument.

```
<number_of_identities>CRLF
identities as...
<public_key>SPACE<pass|nopass>SPACE<name>CRLF
...
<service_key>SPACE<registered|unregistered>SPACE<name>CRLF
<client_key>SPACE<registered|unregistered>SPACE<name>CRLF
<contract_title>CRLF
<contract_raw_content>
```
