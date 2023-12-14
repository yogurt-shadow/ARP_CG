# Column Generation Algorithm for Aircraft Recovery Problem (ARP)

paper:
[A column generation-based heuristic for aircraft recovery problem with airport capacity constraints and maintenance flexibility](https://www.sciencedirect.com/science/article/pii/S0191261517310421)

# Code
1. C++: `SabreCG3_1`
2. Python: `SabreCG_PY`

# Run
1. C++:
 ```shell
 cd SabreCG3_1
 sh run1.sh
 ```

2. Python:
```shell
cd SabreCG_PY
sh run1.sh
```

# Output
- standard output
    - c++: `Scenarios\\Scn1\\output_cpp\\print.txt`
    - python:
`Scenarios\\Scn1\\output_py\\print.txt`

- XML
    - c++:
`Scenarios\\Scn1\\output_cpp\\Output.xml`
    - python:
`Scenarios\\Scn1\\output_py\\Output.xml`

# Other Folder
1. `LP`: Generate Lp format files during algorithms
2. `Scenarios`: Benchmark
3. `CplexTest\\gurobi.py`: compare generated lp files in `LP` folder by gurobipy