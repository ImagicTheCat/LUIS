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

contract=""
for i in $(seq $(($nids+3)) $((${#ar[@]}-1))) 
do
  contract="$(printf "%s\n%s" "$contract" "${ar[$i]}")"
done

zenity --title "LUIS" --question --no-markup --text "$(printf "LUIS\nDo you accept this contract ?\n\n%s\n%s\n%s" "$service" "$client" "$contract")"
if [[ $? == 0 ]] 
then
fi
