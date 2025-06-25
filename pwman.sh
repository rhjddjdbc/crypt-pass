#!/usr/bin/env bash

VAULT="vault"
mkdir -p "$VAULT"

read -s -p "🔐 Master password: " master
echo

echo "What would you like to do?"
select mode in " Save new entry" " Decrypt entry" " Cancel"; do
  case "$mode" in
    " Save new entry")
      read -p "Entry name (e.g. github): " name
      read -s -p "Password: " pw
      echo
      read -s -p "Repeat: " pw2
      echo
      [[ "$pw" != "$pw2" ]] && echo "❌ Not identical." && exit 1

      tmp=$(mktemp)
      echo -n "$pw" > "$tmp"
      openssl enc -aes-256-cbc -pbkdf2 -iter 100000 -salt -in "$tmp" -out "$VAULT/$name.bin" -pass pass:"$master"
      rm -f "$tmp"
      echo "✅ Saved as $VAULT/$name.bin"
      break
      ;;

    " Decrypt entry")
      selected=$(find "$VAULT" -type f -name "*.bin" | /usr/bin/fzf --prompt=" Choose entry: ")
      [[ -z "$selected" ]] && echo "Cancelled." && exit 1

      openssl enc -aes-256-cbc -pbkdf2 -iter 100000 -d -in "$selected" -pass pass:"$master"
      break
      ;;

    " Cancel")
      echo "Goodbye!"
      exit 0
      ;;
  esac
done

