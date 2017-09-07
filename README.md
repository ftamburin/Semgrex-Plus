# Semgrex-Plus
A modified version of the Stanford Semgrex package for dependency structures rewriting useful for treebank maintenance, revision and conversion.

## Requirements:
* ant
* a C/C++ compiler (gcc)

Build the entire package with:
```shell
> make
```
Test the package with the provided scripts using the examples in [1]:
```shell
> ApplyRules.sh examples/treebank.conll examples/tbconv.rules examples/tbconv.CMDs
> ApplyDIFFCommands examples/treebank.conll < examples/tbconv.CMDs > examples/treebankCONV.conll
> CheckRules.sh examples/tbconv.rules
> CheckRules.sh examples/tbconvOVERLAP.rules
```

## References

[1] Tamburini F. (2017). Semgrex-Plus: a tool for automatic dependency-graph rewriting. In Proc. International Conference on Dependency Linguistics - DepLing2017, Pisa. 

[2] Alfieri L., Tamburini F. (2016). (Almost) Automatic Conversion of the Venice Italian Treebank into the Merged Italian Dependency Treebank Format. In Proc. 3rd Italian Conference on Computational Linguistics - CLiC-IT 2016, Napoli, 5-6 December 2016, 19-23. [PDF](http://corpora.ficlit.unibo.it/People/Tamburini/Pubs/CLiCIT2016-VIT2MIDT.pdf).
