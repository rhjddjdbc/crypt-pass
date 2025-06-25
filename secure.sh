#!/usr/bin/env bash

echo "What would you like to do?"
select choice in "Encrypt" "Decrypt" "Pass"; do
    case $choice in
        "Encrypt")
            read -p "File or folder to zip: " input
            read -p "ZIP file name (e.g. secret.zip): " zipname
            read -p "Encrypted output file (e.g. secret.bin): " encryptedname

            ./zipper "$input" "$zipname" || exit 1
            ./encrypt "$zipname" "$encryptedname" || exit 1
            echo "Done: '$zipname' encrypted into '$encryptedname'"

            echo ""
            read -p "Delete original files and ZIP? [y/N]: " confirm
            if [[ "$confirm" =~ ^[Yy]$ ]]; then
                [[ -d "$input" ]] && rm -r "$input" || rm -f "$input"
                rm -f "$zipname"
                echo "Originals deleted."
            else
                echo "Originals were kept."
            fi
            break
            ;;
        "Decrypt")
            read -p "Encrypted input file (.bin): " encryptedfile
            read -p "Name of restored ZIP file (e.g. data.zip): " outputzip

            ./decrypt "$encryptedfile" "$outputzip" || exit 1
            ./unzipper "$outputzip" || exit 1
            rm "$outputzip"
            echo "Done: Extracted to current folder"
            break
            ;;
        "Pass")
            ./pwman.sh
            break
            ;;
        *)
            echo "Please choose 1, 2 or 3."
            ;;
    esac
done

