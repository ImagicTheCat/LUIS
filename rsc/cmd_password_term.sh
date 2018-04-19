#!/bin/bash
stty -echo
read password
stty echo
echo -n $password
