# VASP-auxiliary-script

## agforce.py

The **agforce.py** script will display the vasp job process streamlined information and only needs to read the vasprun.xml file.

and information format like this:

``` bash
Iteration scstep          Energy            dE   MaxForce    Time(s)
        1     57   -525.67101682             -   2.327065      964.6
        2     32   -526.77824896   -1.10723214   1.754279      518.2
        3     31   -527.09996129   -0.32171233   3.240963      497.3
        4     33   -527.22668598   -0.12672469   4.670201      520.1
        5     28   -527.40865346   -0.18196748   1.330238      439.0
        6     29   -527.69991823   -0.29126477   1.201902      455.4
        7     32   -527.95875948   -0.25884125   2.062779      503.5
        8     30   -527.96791523   -0.00915575   2.865800      470.8
        9     26   -528.03539926   -0.06748403   0.573841      408.1
       10     25   -528.10472974   -0.06933048   0.668371      379.1
       11     27   -528.18175925   -0.07702951   1.750242      431.9
       12      9   -528.21415166   -0.03239241   1.027924      182.4
       13     27   -528.22476388   -0.01061222   1.105567      422.2
       14     10   -528.23546062   -0.01069674   0.461812      196.6
       15      8   -528.24760796   -0.01214734   0.440350      161.5
       16     11   -528.27823949   -0.03063153   0.400201      210.9
       17     26   -528.31804547   -0.03980598   0.860867      399.0
       18    10*               -             -          -      176.8
* EDIFFG(all forces less than) = 0.02
```

This script can be used in combination with the **force2xsf.py** script to visualize the unconverged forces in ions.

----

## force2xsf.py

The script **force2xsf.py** will grab the current last calculated structure along with the force and stress information and store all the data in a file in xsf format which can display the force information in the structure via VESTA.

``` bash
CRYSTAL
PRIMVEC
        3.72982932     0.00000000     0.00000000
       -1.86491466     3.23012480     0.00000000
        0.00000000     0.00000000    17.61340000
PRIMCOORD
3 1
 Pt     3.53179567     0.00000000     9.30674443     0.00000000     0.00000000     0.00000000
 Se     1.66688789     1.07670942     7.98885418    -0.00047015    -0.00031347     0.11159362
 Se    -0.19802680     2.15341538    10.62463467    -0.00024832    -0.00092422    -0.11092542
PRIMFORCE
        0.11159505
PRIMSTRESS
       -4.48791712     0.00665968     0.00153784
        0.00665968    -4.49674782     0.01762451
        0.00153793     0.01762446    -2.33791339
```

----

## xdat2arc.py

