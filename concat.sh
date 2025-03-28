#!/bin/bash

# Nom du fichier final
OUTPUT="code_concatene.c"

# Vide le fichier s'il existe déjà
> "$OUTPUT"

# Boucle sur chaque fichier passé en argument
for file in "$@"; do
    echo "/* ========================================== */" >> "$OUTPUT"
    echo "/* FICHIER : $file */" >> "$OUTPUT"
    echo "/* ========================================== */" >> "$OUTPUT"
    echo "" >> "$OUTPUT"
    cat "$file" >> "$OUTPUT"
    echo -e "\n\n" >> "$OUTPUT"
done

echo "Concaténation terminée dans : $OUTPUT"

