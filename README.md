Sugar
=====

Sugar language V0.0.2

Installation : 
--------------

Requis:
* llvm-dev  (>= 3.2)
* lib readline-dev (>= 5, théoriquement remplaçable par libedit, non testé)
* lib boost graph
* gcc


Construction:
`make`

Utilisation : 
--------------

* interpréteur: `./sugar example.txt`
* cli: `./suragi`
* compilateur: `./suragc example.txt example && ./example`

Tests : 
-------

`make test`
