#!/bin/bash

# $1 = INPUT Dep. Treebank
# $2 = RULES to APPLY
# $3 = OUTPUT COMMAND FILE

java -version
export CLASSPATH=stanford-tregex.jar:lib/slf4j-api.jar:lib/slf4j-simple.jar

rm TMP123

java edu.stanford.nlp.semgraph.semgrex.SemgrexPattern -conlluFile $1 -patternsF $2 | CreateDIFFCommandsTAG | sort -n | uniq > TMP123

sort -n TMP123 | uniq > $3
rm TMP123
