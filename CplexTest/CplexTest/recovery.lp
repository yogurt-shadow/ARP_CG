\ENCODING=ISO-8859-1
\Problem name: IloCplex

Minimize
 obj: 100 y_0 + 100 y_1 + 100 y_2 + 100 y_3 + 100 y_4
Subject To
 cover_lg_0: y_2  = 1
 cover_lg_1: y_0  = 1
 cover_lg_2: y_4  = 1
 cover_lg_3: y_3  = 1
 cover_lg_4: y_1  = 1
Bounds
 0 <= y_0 <= 1
 0 <= y_1 <= 1
 0 <= y_2 <= 1
 0 <= y_3 <= 1
 0 <= y_4 <= 1
End
