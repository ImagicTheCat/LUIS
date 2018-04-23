#!/bin/bash
data=$(echo $1 | xxd -p -r)
code=$(zenity --entry --title "LUIS registration" --text "Enter registration code for\n$data")
rcode=$?

if [[ $rcode == 1 ]]
then
  echo -n "deny"
else
  echo -n $code
fi
