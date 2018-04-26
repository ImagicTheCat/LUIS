#!/bin/bash

# decode hex data
data=$(printf "$1" | xxd -r -p)
ar=()
 
# lines in array (remove \r)
while read -r line 
do
  ar+=("$(printf "%s" "$line" | tr -d '\r')")
done <<< "$data"

# number of identities
nids=${ar[0]}

service=${ar[$(($nids+1))]}
client=${ar[$(($nids+2))]}
title=${ar[$(($nids+3))]}

contract=""
for i in $(seq $(($nids+4)) $((${#ar[@]}-1))) 
do
  contract="$(printf "%s\n%s" "$contract" "${ar[$i]}")"
done

zenity --title "LUIS" --question --no-markup --text "$(printf "LUIS\nDo you accept this contract ?\n\n%s\n%s\n%s\n%s" "$service" "$client" "$title" "$contract")"
if [[ $? == 0 ]] 
then
  # accepted
  lr=()
  lc=0
  for id in "${ar[@]:1:$nids}"
  do
    lr+=("$lc")
    lr+=("$id")
    lc=$(($lc+1))
  done

  idx=$(zenity --list --column "n°" --column "identity" "${lr[@]}")
  if [[ $? == 0 ]] 
  then
    identity=${ar[$(($idx+1))]}
    echo "$identity" | grep "nopass" > /dev/null
    pass=""
    if [[ $? == 1 ]] 
    then
      pass=$(zenity --password --username "$identity")
    fi

    printf "%s %s" "$idx" "$pass"
  fi
fi
