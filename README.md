# VASP-auxiliary-script

## agforce.py

agforce.py script will display the information of process of vasp job, which is only needed vasprun.xml file.

and information format like this:

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

the unfinished ion step will end with a star. this script could work with force2xsf.py script which will display the current force of ions.
