# Spell_Checker

Usage: bin/spell [-h] [-o OUT_FILE] [-i IN_FILE] [-d DICT_FILE] [-An]<br /><br />
Spell Checker using a custom dictionary. Auto corrects any known misspellings in the
text.<br /><br />
Additional function to add new words and randomized misspellings to the dictionary.
Option arguments:<br /><br />
-h Displays this usage.<br />
-o OUT_FILE filename, if omitted output to stdout<br />
-i IN_FILE filename, if omitted input comes from stdin<br />
-d for the dictionary filename, if omitted use default "rsrc/dictionary.txt"<br />
-An Automatically add n (in range 0-5) random misspellings for any word not<br />
in the dictionary.<br />
