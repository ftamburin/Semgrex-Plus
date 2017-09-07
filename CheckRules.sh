#!/bin/bash

# $1 = RULES FILE to APPLY

export CLASSPATH=Semgrex-Plus.jar:lib/slf4j-api.jar:lib/slf4j-simple.jar

java edu.stanford.nlp.semgraph.semgrex.SemgrexPattern -conlluFile examples/dummy.conll -patternsF $1 2>&1 | CheckRuleIterference

