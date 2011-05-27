# New rigorous one-step MILP formulation for heat exchanger network synthesis, Barbaro, Bagajewicz
# Implemented by Hubert Thieriot, ARMINES CEP - FRANCE
#

###  SETS AND PARAMETERS  ###
set Si; #set of hot streams
set Sj; #set of cold streams
set HU within Si; #set of hot utility streams
set CU within Sj; #set of cold utility streams
set Sz; #set of zones
param ndt >=0 integer; #number of temperature intervals
set Sdt := 1..ndt; #set of temperature intervals

set B within {i in Si, j in Sj} ; # {(i, j)| more than one heat exchanger is permitted between hot stream i and cold stream j}
set Cz{z in Sz} within Sj  default {}; #{j|j is a cold stream present in zone z}
set Cnz{n in Sdt, z in Sz} within Sj default {}; #{j|j is a cold stream present in temperature interval n in zone z}
set CUz{z in Sz} within CU default {}; #{j|j is a cooling utility present in zone z} CUz in Cz
set Hz{z in Sz} within Si default {}; #i|i is a hot stream present in zone z}
set Hmz{m in Sdt, z in Sz} within Si  default {}; #{i|i is a hot stream present in temperature interval m in zone z}
set HUz{z in Sz} within HU  default {}; #{i|i is a heating utility present in zone z} HUz in Hz
set Mz{z in Sz} within Sdt; #{m|m is a temperature interval in zone z}
set mi0{i in Si} within Sdt; #{m|m is the starting temperature interval for hot stream i}
set mif{i in Si} within Sdt ; #{m|m is the final temperature interval for hot stream i}
set Miz{i in Si,z in Sz} within Sdt; #{m|m is a temperature interval belonging to zone z, in which hot stream i is present}
set nj0{j in Sj} within Sdt; #{m|m is the starting temperature interval for cold stream j}
set njf{j in Sj} within Sdt; #{m|m is the final temperature interval for cold stream j}
set Njz{j in Sj,z in Sz} within Sdt;#{m|m is a temperature interval belonging to zone z, in which cold stream j is present}
set NIH within Si; #{i| non-isothermal mixing is permitted for hot stream i}
set NIC within Sj; #{j| non-isothermal mixing is permitted for cold stream j}
set P within {i in Si, j in Sj}; #{(i, j)| a heat exchange match between hot stream i and cold stream j is permitted}
set PimH {i in Si, m in Sdt} within Sj default{}; #{j| heat transfer from hot stream i at interval m to cold stream j is permitted}
set PjnC {j in Sj, n in Sdt} within Si default{}; #{i| heat transfer from hot stream i to cold stream j at interval n is permitted}
set SH within Si; #{i| splits are allowed for hot stream i}
set SC within Sj; #{j| splits are allowed for cold stream j}

param kmax >=0 integer; #maximum number of heat exchangers
set Sk := 1.. kmax; #set of heat exchangers

### Parameters ###
param Aijmaxz{Si,Sj,Sz} default 1e100; #maximum shell area for an exchanger matching hot stream i and cold stream j in zone z
param cijA{Si,Sj} default 1; #ij variable cost for a new heat exchanger matching hot stream i and cold stream j
param cijF{Si,Sj} default 1; #ij fixed cost for a new heat exchanger matching hot stream i and cold stream j
param ciH{Si} default 1; #cost of heating utility i
param cjC{Sj} default 1; #cost of cooling utility j
param Cpim{Si,Sdt} default 1; #heat capacity of hot stream i at temperature interval m
param Cpjn{Sj,Sdt}  default 1; #heat capacity of cold stream j at temperature interval m
param FiU{Si} default 1e100; #upper bound for the flow rate of heating utility i
param FjU{Si} default 1e100; #upper bound for the flow rate of cooling utility j
param hjn{Sj,Sdt} default 1; #film heat transfer coefficient for cold stream j in interval n
param him{Si,Sdt} default 1; #film heat transfer coefficient for hot stream i in interval m
param DHimzH{Si,Sdt,Sz} default 0;#enthalpy change for hot stream i at interval m of zone z
param DHjnzC{Sj,Sdt,Sz}  default 0;#enthalpy change for cold stream j at interval n of zone z
param qijmL{Si,Sj,Sdt} default 0; #lower bound for heat transfer from hot stream i at interval m to cold stream j
param qijnL{Si,Sj,Sdt} default 0; #lower bound for heat transfer from hot stream i at interval m to cold stream j
param DTi{Si}; #temperature range of stream i
param DTj{Sj}; #temperature range of stream j
param TmU{Sdt};#upper temperature of interval m
param TmL{Sdt};#lower temperature of interval m
param DTmnML{Sdt,Sdt};# mean logarithmic temperature difference between intervals m and n
param Eijzmax{Si,Sj,Sz} default 100; #maximum number of heat exchanger between i and j authorized
param Fi{Si diff HU}  ; #flow rate of hot process stream i
param Fj{Sj diff CU} ; #flow rate of cold process stream j

###  VARIABLES  ###
 var Aijz {i in Si,j in Sj,z in Sz}, >=0; #area for an exchanger matching hot stream i and cold stream j in zone z
var Aijzk {i in Si,j in Sj,z in Sz,k in Sk}, >=0; # area of the kth exchanger matching hot stream i and cold stream j in zone z
var Gijmzk {i in Si,j in Sj,m in Sdt,z in Sz,k in Sk}, binary; #auxiliary binary variable that determines whether the k-th exchanger between hot stream i with cold stream j in zone z exists at interval m of when (i, j) not in B.
var KijmzH {i in Si,j in Sj,m in Sdt,z in Sz}, binary; #determines the beginning of a heat exchanger at interval m of zone z for hot stream i with cold stream j.
#Defined as binary when (i, j) in B and as continuous when (i, j) not in B.
var KijnzC {i in Si,j in Sj,n in Sdt,z in Sz}, binary; #determines the beginning of a heat exchanger at interval n of zone z for cold stream j with hot stream i. De?ned
# as binary when (i, j) ? B and as continuous when (i, j) / ? B.
var KeijmzH {i in Si,j in Sj,m in Sdt,z in Sz}, binary; #determines the end of a heat exchanger at interval m of zone z for hot stream i with cold stream j.
#Defined as binary when (i, j) in B and as continuous when (i, j) not in B.
var KeijnzC {i in Si,j in Sj,m in Sdt,z in Sz}, binary; #determines the end of a heat exchanger at interval n of zone z for cold stream j with hot stream i. De?ned
# as binary when (i, j) ? B and as continuous when (i, j) / ? B.
 var qimjnz {i in Si,m in Sdt,j in Sj,n in Sdt,z in Sz}, >=0;# heat transfer from hot stream i at interval m to cold stream j at interval n in zone z
 var qimnzH {i in Si,m in Sdt,n in Sdt,z in Sz}, >=0;#non-isothermal mixing heat transfer for hot stream i between intervals m and n in zone z
 var qjnmzC {j in Sj,n in Sdt,m in Sdt,z in Sz}, >=0;#non-isothermal mixing heat transfer for hot stream i between intervals m and n in zone z
 var qijmzH {i in Si,j in Sj,m in Sdt,z in Sz}, >=0;#heat transfer from hot stream i at interval m to cold stream j in zone z
 var qijnzC {i in Si,j in Sj,n in Sdt,z in Sz}, >=0;#heat transfer to cold stream j at interval n from hot stream j in zone z
var qaijmzH {i in Si,j in Sj,m in Sdt,z in Sz}, >=0;#auxiliary continuous variable utilized to compute the hot side heat load of each heat exchanger when several
#exchangers exist between hot stream i and cold stream j in zone z
var qaijnzC {i in Si,j in Sj,n in Sdt,z in Sz}, >=0;#auxiliary continuous variable utilized to compute the cold side heat load of each heat exchanger when several
#exchangers exist between hot stream i and cold stream j in zone z
var qaimjnz {i in Si,m in Sdt,j in Sj,n in Sdt,z in Sz}, >=0;#auxiliary continuous variable utilized to compute the area of individual heat exchangers between hot stream i
#with cold stream j in zone z when (i, j) in B.
 var Uijz {i in Si,j in Sj,z in Sz}, >=0;# number of shells in the heat exchanger between hot stream i and cold stream j in zone z
 var Uijzk {i in Si,j in Sj,z in Sz,k in Sk}, >=0;# number of shells in the kth heat exchanger between hot stream i and cold stream j in zone z
var Ximjnz {i in Si,m in Sdt,j in Sj,n in Sdt,z in Sz};# auxiliary continuous variable equal to zero when an exchanger ends at interval m for hot stream i and at interval
#n for cold stream j. A value of one corresponds to all other cases.
var YijmzH {i in Si,j in Sj,m in Sdt,z in Sz}, >=0;# determines whether heat is being transferred from hot stream i at interval m to cold stream j. De?ned as binary
#when (i, j) / ? B and as continuous when (i, j) ? B.
var YijnzC {i in Si,j in Sj,n in Sdt,z in Sz}, >=0;# determines whether heat is being transferred from hot stream i to cold stream j at interval n. De?ned as binary
#when (i, j) / ? B and as continuous when (i, j) ? B.
var aijmzH {i in Si,j in Sj,m in Sdt,z in Sz}, >=0;#  auxiliary continuous variable equal to one when heat transfer from interval m of hot stream i to cold stream j
#occurs in zone z and it does not correspond to the beginning nor the ending of a heat exchanger. A value of zero
#corresponds to all other cases.
var aijnzC {i in Si,j in Sj,n in Sdt,z in Sz}, >=0;# auxiliary continuous variable equal to one when heat transfer from hot stream i to interval n of cold stream j
#occurs in zone z and it does not correspond to the beginning nor the ending of a heat exchanger. A value of zero
#corresponds to all other cases.
var Eijz {i in Si, j in Sj, z in Sz}, >=0; #number of heat exchangers between hot stream i and cold stream j
var FiH{HU}, >=0  ; #flow rate of hot utility stream i
var FjC{CU}, >=0 ; #flow rate of cold process stream j
#################
### Equations ###
#################

### Heat balance equations ###
s.t. eq1 'Heat balance for heating utilities' {z in Sz,m in Mz[z],i in Hmz[m,z] inter HUz[z]} :
FiH[i]*(TmU[m]-TmL[m]) ,= sum{n in Mz[z] : TmL[n]<TmU[m]}(sum{j in PimH[i,m] inter Cnz[n,z] : i in PjnC[j,n]}(qimjnz[i,m,j,n,z]));

s.t. eq2 'Heat balance for cooling utilities' {z in Sz,n in Mz[z],j in Cnz[n,z] inter CUz[z]} :
FjC[j]*(TmU[n]-TmL[n]) ,= sum{m in Mz[z] : TmL[n]<TmU[m]}(sum{i in PjnC[j,n] inter Hmz[m,z] : j in PimH[i,m]}(qimjnz[i,m,j,n,z]));

s.t. eq3 'Heat balance for hot process streams' {z in Sz,m in Mz[z],i in Hmz[m,z] diff (HUz[z] union NIH)} :
DHimzH[i,m,z] ,= sum{n in Mz[z] : TmL[n]<TmU[m]}(sum{j in PimH[i,m] inter Cnz[n,z] : i in PjnC[j,n]}(qimjnz[i,m,j,n,z]));

s.t. eq4 'Heat balance for cooling process streams' {z in Sz,n in Mz[z],j in Cnz[n,z] diff (CUz[z] union NIC)} :
DHjnzC[j,n,z] ,= sum{m in Mz[z] : TmL[n]<TmU[m]}(sum{i in PjnC[j,n] inter Hmz[m,z] : j in PimH[i,m]}(qimjnz[i,m,j,n,z]));

s.t. eq5 'Cumulative heat transfer from i at interval m to j' {z in Sz,m in Mz[z],i in Hmz[m,z],j in Cz[z] inter PimH[i,m]} :
qijmzH[i,j,m,z] ,= sum{n in Mz[z] : (TmL[n]<TmU[m]) and (j in Cnz[n,z]) and (i in PjnC[j,n])}(qimjnz[i,m,j,n,z]);

s.t. eq6 'Cumulative heat transfer to j at interval n from i' {z in Sz,n in Mz[z],j in Cnz[n,z],i in Hz[z] inter PjnC[j,n]} :
qijnzC[i,j,n,z] ,= sum{m in Mz[z] : (TmL[n]<TmU[m]) and (i in Hmz[m,z]) and (j in PimH[i,m])}(qimjnz[i,m,j,n,z]);

s.t. eq7 'Heat balance for hot streams (non-isothermal mixing allowed)' {z in Sz,m in Mz[z],i in (Hmz[m,z] diff HUz[z]) inter NIH} :
DHimzH[i,m,z] ,= sum{n in Mz[z] : TmL[n]<TmU[m]}(sum{j in PimH[i,m] inter Cnz[n,z] : i in PjnC[j,n]}(qimjnz[i,m,j,n,z]))
		+ sum{n in Mz[z] : (n > m) and (i in Hmz[n,z])}(qimnzH[i,m,n,z])
		- sum{n in Mz[z] : (n < m) and (i in Hmz[n,z])}(qimnzH[i,m,n,z]);
		
s.t. eq8 'Heat balance for cold streams (non-isothermal mixing allowed)' {z in Sz,n in Mz[z],j in (Cnz[n,z] diff CUz[z]) inter NIC} :
DHjnzC[j,n,z] ,= sum{m in Mz[z] : TmL[n]<TmU[m]}(sum{i in PjnC[j,n] inter Hmz[m,z] : j in PimH[i,m]}(qimjnz[i,m,j,n,z]))
		+ sum{m in Mz[z] : (m < n) and (j in Cnz[m,z])}(qjnmzC[j,n,m,z])
		- sum{m in Mz[z] : (m > n) and (j in Cnz[m,z])}(qjnmzC[j,n,m,z]);

s.t. eq9 'Heat balance for hot streams (non-isothermal mixing allowed)' {z in Sz,m in Mz[z],i in (Hmz[m,z] diff HUz[z]) inter NIH} :
sum{n in Mz[z] : (n < m) and (i in Hmz[n,z])}qimnzH[i,m,n,z] 
,<= sum{n in Mz[z] : TmL[n]<TmU[m]}(sum{j in PimH[i,m] inter Cnz[n,z] : i in PjnC[j,n]}(qimjnz[i,m,j,n,z]));

s.t. eq10 'Heat balance for cold streams (non-isothermal mixing allowed)' {z in Sz,n in Mz[z],j in (Cnz[n,z] diff CUz[z]) inter NIC} :
sum{m in Mz[z] : (m > n) and (j in Cnz[m,z])}(qjnmzC[j,n,m,z])
,<= sum{m in Mz[z] : TmL[n]<TmU[m]}(sum{i in PjnC[j,n] inter Hmz[m,z] : j in PimH[i,m]}(qimjnz[i,m,j,n,z]));

### Heat exchanger definition and counting ###
s.t. eq11 'Bounds on cumulative heat transfer for hot process streams' {z in Sz,m in Mz[z],i in (Hmz[m,z] diff HUz[z]),j in (Cz[z] inter PimH[i,m])} :
qijmzH[i,j,m,z] ,>= qijmL[i,j,m]*YijmzH[i,j,m,z];

s.t. eq11bis 'Bounds on cumulative heat transfer for hot process streams' {z in Sz,m in Mz[z],i in (Hmz[m,z] diff HUz[z]),j in (Cz[z] inter PimH[i,m])} :
qijmzH[i,j,m,z] ,<= DHimzH[i,m,z]*YijmzH[i,j,m,z];

s.t. eq12 'Bounds on cumulative heat transfer for cold process streams' {z in Sz,n in Mz[z],j in (Cnz[n,z] diff CUz[z]),i in (Hz[z] inter PjnC[j,n])} :
qijnzC[i,j,n,z] ,>= qijnL[i,j,n]*YijnzC[i,j,n,z];

s.t. eq12bis 'Bounds on cumulative heat transfer for cold process streams' {z in Sz,n in Mz[z],j in (Cnz[n,z] diff CUz[z]),i in (Hz[z] inter PjnC[j,n])} :
qijnzC[i,j,n,z] ,<= DHjnzC[j,n,z]*YijnzC[i,j,n,z];

s.t. eq13 'Bounds on cumulative heat transfer for heating utilities' {z in Sz,m in Mz[z],i in (Hmz[m,z] inter HUz[z]),j in (Cz[z] inter PimH[i,m])} :
qijmzH[i,j,m,z] ,>= qijmL[i,j,m]*YijmzH[i,j,m,z];

s.t. eq13bis 'Bounds on cumulative heat transfer for heating utilities' {z in Sz,m in Mz[z],i in (Hmz[m,z] inter HUz[z]),j in (Cz[z] inter PimH[i,m])} :
qijmzH[i,j,m,z] ,<= FiU[i]*(TmU[m]-TmL[m]);

s.t. eq14 'Bounds on cumulative heat transfer for cooling utilities' {z in Sz,n in Mz[z],j in (Cnz[n,z] inter CUz[z]),i in (Hz[z] inter PjnC[j,n])} :
qijnzC[i,j,n,z] ,>= qijnL[i,j,n]*YijnzC[i,j,n,z];

s.t. eq14bis 'Bounds on cumulative heat transfer for cooling utilities' {z in Sz,n in Mz[z],j in (Cnz[n,z] inter CUz[z]),i in (Hz[z] inter PjnC[j,n])} :
qijnzC[i,j,n,z] ,<= FjU[i]*(TmU[n]-TmL[n]);
							
s.t. eq15 'Heat exchanger beginning for hot streams (i,j) not in B' {z in Sz,i in Hz[z],m in Mz[z] inter mi0[i], j in Cz[z] inter PimH[i,m] : not ((i,j) in B)} :
KijmzH[i,j,m,z],>= YijmzH[i,j,m,z];			
				
s.t. eq16  {z in Sz,m in Mz[z] inter (2..ndt),i in (Hmz[m,z] inter Hmz[m-1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m-1] : not ((i,j) in B)} :
KijmzH[i,j,m,z] ,<= 2-YijmzH[i,j,m,z] - YijmzH[i,j,m-1,z];

s.t. eq17  {z in Sz,m in Mz[z] inter (2..ndt),i in (Hmz[m,z] inter Hmz[m-1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m-1] : not ((i,j) in B)} :
KijmzH[i,j,m,z] ,<= YijmzH[i,j,m,z];

s.t. eq18  {z in Sz,m in Mz[z] inter (2..ndt),i in (Hmz[m,z] inter Hmz[m-1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m-1] : not ((i,j) in B)} :
KijmzH[i,j,m,z] ,>= YijmzH[i,j,m,z] - YijmzH[i,j,m-1,z];

s.t. eq19  {z in Sz,m in Mz[z] inter (2..ndt),i in (Hmz[m,z] inter Hmz[m-1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m-1] : not ((i,j) in B)} :
KijmzH[i,j,m,z] ,>= 0;

s.t. eq20 'Heat exchanger ending for hot streams (i,j) not in B' {z in Sz,i in Hz[z],m in Mz[z] inter mif[i], j in Cz[z] inter PimH[i,m] : not ((i,j) in B)} :
KeijmzH[i,j,m,z],>= YijmzH[i,j,m,z];			
				
s.t. eq21  {z in Sz,m in Mz[z] inter (1..ndt-1),i in (Hmz[m,z] inter Hmz[m+1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m+1] : not ((i,j) in B)} :
KeijmzH[i,j,m,z] ,<= 2-YijmzH[i,j,m,z] - YijmzH[i,j,m+1,z];

s.t. eq22  {z in Sz,m in Mz[z] inter (1..ndt-1),i in (Hmz[m,z] inter Hmz[m+1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m+1] : not ((i,j) in B)} :
KeijmzH[i,j,m,z] ,<= YijmzH[i,j,m,z];

s.t. eq23  {z in Sz,m in Mz[z] inter (1..ndt-1),i in (Hmz[m,z] inter Hmz[m+1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m+1] : not ((i,j) in B)} :
KeijmzH[i,j,m,z] ,>= YijmzH[i,j,m,z] - YijmzH[i,j,m+1,z];

s.t. eq24  {z in Sz,m in Mz[z] inter (1..ndt-1),i in (Hmz[m,z] inter Hmz[m+1,z]), j in Cz[z] inter PimH[i,m] inter PimH[i,m+1] : not ((i,j) in B)} :
KeijmzH[i,j,m,z] ,>= 0;

s.t. eq25 'Heat exchanger existence on hot streams - (i,j) in B' {z in Sz, m in Mz[z], i in Hmz[m,z], j in (Cz[z] inter PimH[i,m]) : (i,j) in B} :
YijmzH[i,j,m,z] = sum{l in Miz[i,z] : (l<= m) and (j in PimH[i,l])}(KijmzH[i,j,l,z])
				-sum{l in Miz[i,z] : (l<= m-1) and (j in PimH[i,l])}(KeijmzH[i,j,l,z]);		
		
s.t. eq26 'Heat exchanger beginning for  cold streams (i,j) not in B' {z in Sz,j in Cz[z],n in Mz[z] inter nj0[j], i in (Hz[z] inter PjnC[j,n]) : not ((i,j) in B)} :
KijnzC[i,j,n,z],>= YijnzC[i,j,n,z];			
				
s.t. eq27  {z in Sz,n in Mz[z] inter (2..ndt),j in (Cnz[n,z] inter Cnz[n-1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n-1]) : not ((i,j) in B)} :
KijnzC[i,j,n,z] ,<= 2-YijnzC[i,j,n,z] - YijnzC[i,j,n-1,z];

s.t. eq28  {z in Sz,n in Mz[z] inter (2..ndt),j in (Cnz[n,z] inter Cnz[n-1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n-1]) : not ((i,j) in B)} :
KijnzC[i,j,n,z] ,<= YijnzC[i,j,n,z];

s.t. eq29  {z in Sz,n in Mz[z] inter (2..ndt),j in (Cnz[n,z] inter Cnz[n-1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n-1]) : not ((i,j) in B)} :
KijnzC[i,j,n,z] ,>= YijnzC[i,j,n,z] - YijnzC[i,j,n-1,z];

s.t. eq30  {z in Sz,n in Mz[z] inter (2..ndt),j in (Cnz[n,z] inter Cnz[n-1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n-1]) : not ((i,j) in B)} :
KijnzC[i,j,n,z] ,>= 0;

s.t. eq31 'Heat exchanger ending for  cold streams (i,j) not in B' {z in Sz,j in Cz[z],n in Mz[z] inter njf[j], i in (Hz[z] inter PjnC[j,n]) : not ((i,j) in B)} :
KeijnzC[i,j,n,z],>= YijnzC[i,j,n,z];			
				
s.t. eq32  {z in Sz,n in Mz[z] inter (1..ndt-1),j in (Cnz[n,z] inter Cnz[n+1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n+1]) : not ((i,j) in B)} :
KeijnzC[i,j,n,z] ,<= 2-YijnzC[i,j,n,z] - YijnzC[i,j,n+1,z];

s.t. eq33  {z in Sz,n in Mz[z] inter (1..ndt-1),j in (Cnz[n,z] inter Cnz[n+1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n+1]) : not ((i,j) in B)} :
KeijnzC[i,j,n,z] ,<= YijnzC[i,j,n,z];

s.t. eq34  {z in Sz,n in Mz[z] inter (1..ndt-1),j in (Cnz[n,z] inter Cnz[n+1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n+1]) : not ((i,j) in B)} :
KeijnzC[i,j,n,z] ,>= YijnzC[i,j,n,z] - YijnzC[i,j,n+1,z];

s.t. eq35  {z in Sz,n in Mz[z] inter (1..ndt-1),j in (Cnz[n,z] inter Cnz[n+1,z]), i in (Hz[z] inter PjnC[j,n] inter PjnC[j,n+1]) : not ((i,j) in B)} :
KeijnzC[i,j,n,z] ,>= 0;

s.t. eq36 'Heat exchanger existence on cold streams - (i,j) in B' {z in Sz, n in Mz[z], j in Cnz[n,z], i in (Hz[z] inter PjnC[j,n]) : (i,j) in B} :
YijnzC[i,j,n,z] ,= sum{l in Njz[j,z] : (l<= n) and (i in PjnC[j,l])}(KijnzC[i,j,l,z])
				-sum{l in Njz[j,z] : (l<= n-1) and (i in PjnC[j,l])}(KeijnzC[i,j,l,z]);					
	
s.t. eq37 'Number of heat exchangers between hot stream i and cold stream j' {z in Sz, i in Hz[z],j in Cz[z] : (i,j) in P} :
Eijz[i,j,z] = sum{m in Miz[i,z] : j in PimH[i,m]}(KijmzH[i,j,m,z]);

s.t. eq38 {z in Sz, i in Hz[z],j in Cz[z] : (i,j) in P} :
Eijz[i,j,z] ,= sum{n in Njz[j,z] : i in PjnC[j,n]}(KijnzC[i,j,n,z]);
		
s.t. eq39 {z in Sz, i in Hz[z],j in Cz[z] : (i,j) in P} :
Eijz[i,j,z] ,= sum{m in Miz[i,z] : j in PimH[i,m]}(KeijmzH[i,j,m,z]);

s.t. eq40 {z in Sz, i in Hz[z],j in Cz[z] : (i,j) in P} :
Eijz[i,j,z] ,= sum{n in Njz[j,z] : i in PjnC[j,n]}(KeijnzC[i,j,n,z]);		

s.t. eq41 {z in Sz, i in Hz[z],j in Cz[z] : ((i,j) in P) and (not ((i,j) in B))} :
Eijz[i,j,z] ,<= 1;		

s.t. eq42 {z in Sz, i in Hz[z],j in Cz[z] : ((i,j) in P) and ((i,j) in B)} :
Eijz[i,j,z] ,<= Eijzmax[i,j,z];		

### Heat transfer consistency ###
s.t. eq43 'Heat transfer consistency for multiple heat exchangers between the same pair of streams'
{z in Sz,m in Mz[z], n in Mz[z],i in Hmz[m,z],j in Cnz[n,z] : ((i in PjnC[j,n]) and (j in PimH[i,m]) and (TmL[n]<=TmU[m]) and ((i,j) in B))} :
sum{l in Miz[i,z] : l<=m}(qijmzH[i,j,l,z]) - qijmzH[i,j,n,z]
,<= sum{l in Njz[j,z] : l<=n}(qijnzC[i,j,l,z])-qijnzC[i,j,m,z]+4*Ximjnz[i,m,j,n,z]
*max(sum{l in Miz[i,z] : (l<=m) and (j in PimH[i,l])}(DHimzH[i,l,z]),sum{l in Miz[i,z] : (l<=n) and (i in PjnC[j,l])}(DHjnzC[j,l,z]));
					
s.t. eq44 
{z in Sz,m in Mz[z], n in Mz[z],i in Hmz[m,z],j in Cnz[n,z] : ((i in PjnC[j,n]) and (j in PimH[i,m]) and (TmL[n]<=TmU[m]) and ((i,j) in B))} :
sum{l in Miz[i,z] : l<=m}(qijmzH[i,j,l,z]) - qijmzH[i,j,n,z]
,>= sum{l in Njz[j,z] : l<=n}(qijnzC[i,j,l,z])-qijnzC[i,j,m,z]-4*Ximjnz[i,m,j,n,z]
*max(sum{l in Miz[i,z] : (l<=m) and (j in PimH[i,l])}(DHimzH[i,l,z]),sum{l in Miz[i,z] : (l<=n) and (i in PjnC[j,l])}(DHjnzC[j,l,z]));

s.t. eq45 
{z in Sz,m in Mz[z], n in Mz[z],i in Hmz[m,z],j in Cnz[n,z] : ((i in PjnC[j,n]) and (j in PimH[i,m]) and (TmL[n]<=TmU[m]) and ((i,j) in B))} :
Ximjnz[i,m,j,n,z] ,= 2 - KeijmzH[i,j,m,z]-KeijnzC[i,j,n,z]+1/4*sum{l in Njz[j,z] : l<=n}(KeijnzC[i,j,l,z])-1/4*sum{l in Miz[i,z] : l<=m}(KeijmzH[i,j,l,z]);

s.t. eq46 
{z in Sz,m in Mz[z], n in Mz[z],i in Hmz[m,z],j in Cnz[n,z] : ((i in PjnC[j,n]) and (j in PimH[i,m]) and (TmL[n]<TmU[m]) and (TmL[n]>=TmL[m])and ((i,j) in B))} :
sum{l in Miz[i,z] : (l<=m) and (j in PimH[i,l])}(KeijmzH[i,j,l,z])-sum{l in Njz[j,z] : (l<=n) and (i in PjnC[j,l])}(KeijnzC[i,j,l,z]) ,>=0;

s.t. eq47
{z in Sz,m in Mz[z],(i,j) in B : (i in Hmz[m,z]) and (j in PimH[i,m])} :
sum{l in Miz[i,z] : (l<=m) and (j in PimH[i,l])}(KijmzH[i,j,l,z] - KeijmzH[i,j,l,z]) ,<=1;

s.t. eq48
{z in Sz,n in Mz[z],(i,j) in B : (i in PjnC[j,n]) and (j in Cnz[n,z])} :
sum{l in Njz[j,z] : (l<=n) and (i in PjnC[j,l])}(KijnzC[i,j,l,z] - KeijnzC[i,j,l,z]) ,<=1;

s.t. eq49
{z in Sz,m in Mz[z],(i,j) in B : (i in Hmz[m,z]) and (j in PimH[i,m])} :
qaijmzH[i,j,m,z] ,<= qijmzH[i,j,m,z];

s.t. eq50
{z in Sz,m in Mz[z],(i,j) in B : (i in Hmz[m,z]) and (j in PimH[i,m])} :
qaijmzH[i,j,m,z] ,<= KijmzH[i,j,m,z]*DHimzH[i,m,z];

s.t. eq51
{z in Sz,m in Mz[z],(i,j) in B : (i in Hmz[m,z]) and (j in PimH[i,m])} :
qaijmzH[i,j,m,z] ,<= KeijmzH[i,j,m,z]*DHimzH[i,m,z];

s.t. eq52
{z in Sz,m in Mz[z],(i,j) in B : (i in Hmz[m,z]) and (j in PimH[i,m])} :
qaijmzH[i,j,m,z] ,>=0 ;

s.t. eq53
{z in Sz,n in Mz[z],(i,j) in B : (i in PjnC[j,n]) and (j in Cnz[n,z])} :
qaijnzC[i,j,n,z] ,<= qijnzC[i,j,n,z];

s.t. eq54
{z in Sz,n in Mz[z],(i,j) in B : (i in PjnC[j,n]) and (j in Cnz[n,z])} :
qaijnzC[i,j,n,z] ,<= KijnzC[i,j,n,z]*DHjnzC[j,n,z];

s.t. eq55
{z in Sz,n in Mz[z],(i,j) in B : (i in PjnC[j,n]) and (j in Cnz[n,z])} :
qaijnzC[i,j,n,z] ,<= KeijnzC[i,j,n,z]*DHjnzC[j,n,z];

s.t. eq56
{z in Sz,n in Mz[z],(i,j) in B : (i in PjnC[j,n]) and (j in Cnz[n,z])} :
qaijnzC[i,j,n,z] ,>=0 ;

### Definition of exchanger internal interval for hot streams ###
s.t. eq57
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z]), j in (PimH[i,m] inter PimH[i,m-1]) : (i in SH) and (j in Cz[z])} :
aijmzH[i,j,m,z] ,<= 1 - KijmzH[i,j,m,z]- KijmzH[i,j,m-1,z];

s.t. eq58
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z]), j in (PimH[i,m] inter PimH[i,m-1]) : (i in SH) and (j in Cz[z])} :
aijmzH[i,j,m,z] ,<= 1 - KeijmzH[i,j,m,z]- KeijmzH[i,j,m-1,z];

s.t. eq59
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z]), j in (PimH[i,m] inter PimH[i,m-1]) : (i in SH) and (j in Cz[z])} :
aijmzH[i,j,m,z] ,>= YijmzH[i,j,m,z] - KijmzH[i,j,m,z] - KijmzH[i,j,m-1,z] - KeijmzH[i,j,m,z] - KeijmzH[i,j,m-1,z];

s.t. eq60
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z]), j in (PimH[i,m] inter PimH[i,m-1]) : (i in SH) and (j in Cz[z])} :
aijmzH[i,j,m,z] ,>= 0;

### Flow rate consistency for hot streams in exchanger internal intervals -  i,j in B###
s.t. eq61
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z])} :
qijmzH[i,j,m,z]/(Cpim[i,m]*(TmU[m]-TmL[m]))
,<= qijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1]))+(1-aijmzH[i,j,m,z])*Fi[i];

s.t. eq62
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z])} :
qijmzH[i,j,m,z]/(Cpim[i,m]*(TmU[m]-TmL[m]))
,>= qijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1]))-(1-aijmzH[i,j,m,z])*Fi[i];

### Flow rate consistency for hot streams in extreme intervals - i,j not in B###
s.t. eq63
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z]) : not ((i,j) in B) } :
qijmzH[i,j,m,z]/(Cpim[i,m]*(TmU[m]-TmL[m]))
,>= qijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1]))- (1+KeijmzH[i,j,m-1,z] + KeijmzH[i,j,m,z] - KijmzH[i,j,m-1,z])*Fi[i];

s.t. eq64
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z]) : not ((i,j) in B) } :
qijmzH[i,j,m,z]/(Cpim[i,m]*(TmU[m]-TmL[m]))
,<= qijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1])) + (1+KijmzH[i,j,m-1,z] + KijmzH[i,j,m,z] - KeijmzH[i,j,m,z])*Fi[i];


### Flow rate consistency for hot streams in extreme intervals - i,j  in B###
s.t. eq65
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z]) :  ((i,j) in B) } :
qijmzH[i,j,m,z]/(Cpim[i,m]*(TmU[m]-TmL[m]))
,>= qijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1]))- (1+KeijmzH[i,j,m-1,z] + KeijmzH[i,j,m,z] - KijmzH[i,j,m-1,z])*Fi[i];

s.t. eq66
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z]) :  ((i,j) in B) } :
qijmzH[i,j,m,z]/(Cpim[i,m]*(TmU[m]-TmL[m]))
,>= qaijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1])) - (2+KeijmzH[i,j,m,z] - KijmzH[i,j,m-1,z] - YijmzH[i,j,m-1,z])*Fi[i];

s.t. eq67
{z in Sz, m in Mz[z] inter (2..ndt), i in (Hmz[m,z] inter Hmz[m-1,z] inter SH diff HUz[z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z]) :  ((i,j) in B) } :
(qijmzH[i,j,m,z]-qaijmzH[i,j,m,z])/(Cpim[i,m]*(TmU[m]-TmL[m]))
,>= qijmzH[i,j,m-1,z]/(Cpim[i,m-1]*(TmU[m-1]-TmL[m-1])) - (2+KijmzH[i,j,m-1,z] - KeijmzH[i,j,m,z] - YijmzH[i,j,m,z])*Fi[i];


### Flow rate constistency for ho strams - i not in SH ###
s.t. eq68
{z in Sz, m in Mz[z] inter (2..ndt-1), i in (Hmz[m,z] inter Hmz[m-1,z] inter Hmz[m+1,z]), j in (PimH[i,m] inter PimH[i,m-1] inter Cz[z] inter PimH[i,m+1]) :  not (i in SH) } :
qijmzH[i,j,m,z]
,>= (YijmzH[i,j,m,z] - KijmzH[i,j,m,z] - KeijmzH[i,j,m,z])*DHimzH[i,m,z];


### Definition of exchanger internal interval for cold streams ###
s.t. eq69
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) } :
aijnzC[i,j,n,z] ,<= 1 - KijnzC[i,j,n,z]- KijnzC[i,j,n-1,z];

s.t. eq70
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) } :
aijnzC[i,j,n,z] ,<= 1 - KeijnzC[i,j,n,z]- KeijnzC[i,j,n-1,z];

s.t. eq71
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) } :
aijnzC[i,j,n,z] ,>= YijnzC[i,j,n,z] - KijnzC[i,j,n,z] - KijnzC[i,j,n-1,z] - KeijnzC[i,j,n,z] - KeijnzC[i,j,n-1,z];

s.t. eq72
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) } :
aijnzC[i,j,n,z] ,>= 0;

### Flow rate consistency for hot streams in exchanger internal intervals -  i,j in B###
s.t. eq73
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC diff CUz[z]), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) } :
qijnzC[i,j,n,z]/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,<= qijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1]))+(1-aijnzC[i,j,n,z])*Fj[j];

s.t. eq74
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC diff CUz[z]), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) } :
qijnzC[i,j,n,z]/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,>= qijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1]))-(1-aijnzC[i,j,n,z])*Fj[j];



### Flow rate consistency for hot streams in extreme intervals - i,j not in B###
s.t. eq75
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) : not ((i,j) in B) } :
qijnzC[i,j,n,z]/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,>= qijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1]))- (1+KeijnzC[i,j,n-1,z] + KeijnzC[i,j,n,z] - KijnzC[i,j,n-1,z])*Fj[j];

s.t. eq76
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) : not ((i,j) in B) } :
qijnzC[i,j,n,z]/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,<= qijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1])) + (1+KijnzC[i,j,n-1,z] + KijnzC[i,j,n,z] - KeijnzC[i,j,n,z])*Fj[j];


### Flow rate consistency for hot streams in extreme intervals - i,j  in B###
s.t. eq77
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC diff CUz[z]), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) :  ((i,j) in B) } :
qijnzC[i,j,n,z]/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,>= qijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1]))- (1+KeijnzC[i,j,n-1,z] + KeijnzC[i,j,n,z] - KijnzC[i,j,n-1,z])*Fj[j];

s.t. eq78
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC diff CUz[z]), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) :  ((i,j) in B) } :
qijnzC[i,j,n,z]/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,>= qaijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1])) - (2+KeijnzC[i,j,n,z] - KijnzC[i,j,n-1,z] - YijnzC[i,j,n-1,z])*Fj[j];

s.t. eq79
{z in Sz, n in Mz[z] inter (2..ndt), j in (Cnz[n,z] inter Cnz[n-1,z] inter SC diff CUz[z]), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) :  ((i,j) in B) } :
(qijnzC[i,j,n,z]-qaijnzC[i,j,n,z])/(Cpjn[j,n]*(TmU[n]-TmL[n]))
,>= qijnzC[i,j,n-1,z]/(Cpjn[j,n-1]*(TmU[n-1]-TmL[n-1])) - (2+KijnzC[i,j,n-1,z] - KeijnzC[i,j,n,z] - YijnzC[i,j,n,z])*Fj[j];


### Flow rate constistency for ho strams - i not in SH ###
s.t. eq80
{z in Sz, n in Mz[z] inter (2..ndt-1), j in (Cnz[n,z] inter Cnz[n-1,z] inter Cnz[n+1,z]), i in (PjnC[j,n] inter PjnC[j,n-1] inter Hz[z]) :  (not((i,j) in B)) and (not (j in SC)) } :
qijnzC[i,j,n,z]
,>= (YijnzC[i,j,n,z] - KijnzC[i,j,n,z] - KeijnzC[i,j,n,z])*DHjnzC[j,n,z];


### Temperature feasibility constraints - i not in SH, j not in SC ###
s.t. eq81
{z in Sz, m in Mz[z],n in Mz[z],i in Hmz[m,z], j in Cnz[n,z]
:(not (i in SH)) and (i in PjnC[j,n]) 
and (j in PimH[i,m]) and (not (j in SC)) 
and (TmL[n]<=TmU[m]) and (TmU[m] >= TmL[m]) } :
TmL[m] + qijmzH[i,j,m,z]/(Fi[i]*Cpim[i,m])
,>= TmL[m] + qijnzC[i,j,n,z]/(Fj[j]*Cpjn[j,n]) - (2-KijmzH[i,j,m,z]-KijnzC[i,j,n,z])*TmU[n];

s.t. eq82
{z in Sz, m in Mz[z],n in Mz[z],i in Hmz[m,z], j in Cnz[n,z] :(not (i in SH)) and (i in PjnC[j,n]) and (j in PimH[i,m]) and (not (j in SC))  and (TmL[n]<=TmU[m]) and (TmU[m] >= TmL[m])} :
TmU[m] + qijmzH[i,j,m,z]/(Fi[i]*Cpim[i,m])
,>= TmU[m] + qijnzC[i,j,n,z]/(Fj[j]*Cpjn[j,n]) - (2-KeijmzH[i,j,m,z]-KeijnzC[i,j,n,z])*TmU[n];

### Temperature feasibility constraints - i in SH, j in SC, (i,j) not in B ###
s.t. eq83
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
KeijnzC[i,j,n,z] ,<= 2-KijmzH[i,j,m,z] - KijnzC[i,j,n,z];

s.t. eq84
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
qijnzC[i,j,n,z]/(TmU[m]-TmL[n])
,<= qijnzC[i,j,n+1,z]/(TmU[n+1]-TmL[n+1])*Cpjn[j,n]/Cpjn[j,n+1]+(2-KijmzH[i,j,m,z] - KijnzC[i,j,n,z])*DHjnzC[j,n,z]/(TmU[m]-TmL[n]);

s.t. eq85
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
qijmzH[i,j,m,z]/(min(TmU[m],TmU[n])-TmL[n])
,>= qijmzH[i,j,m+1,z]/(TmU[m+1]-TmL[m+1])*Cpim[i,m]/Cpim[i,m+1]+(2-KijmzH[i,j,m,z] - KijnzC[i,j,n,z])*DHimzH[i,m+1,z]/(TmU[m+1]-TmL[m+1]);

s.t. eq86
{z in (Sz inter (2..ndt)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m-1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n-1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n-1])) and (j in (PimH[i,m] inter PimH[i,m-1]))} :
KijmzH[i,j,m,z] ,<= 2 - KeijmzH[i,j,m,z] - KeijnzC[i,j,n,z];

s.t. eq87
{z in (Sz inter (2..ndt)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m-1,z]),
j in (/*SC inter*/ Cnz[n,z] inter Cnz[n-1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n-1])) and (j in (PimH[i,m] inter PimH[i,m-1]))} :
qijmzH[i,j,m,z]/(TmU[m]-TmL[n])
,<= qijmzH[i,j,m-1,z]/(TmU[m-1]-TmL[m-1])*Cpim[i,m]/Cpim[i,m-1]+(2-KeijmzH[i,j,m,z] - KeijnzC[i,j,n,z])*DHimzH[i,m,z]/(TmU[m]-TmL[n]);

s.t. eq88
{z in (Sz inter (2..ndt)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m-1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n-1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n-1])) and (j in (PimH[i,m] inter PimH[i,m-1]))} :
qijnzC[i,j,n,z]/(TmU[n] - max(TmL[m],TmL[n]))
,>= qijnzC[i,j,n-1,z]/(TmU[n-1]-TmL[n-1])*Cpjn[j,n]/Cpjn[i,n-1]+(2-KeijmzH[i,j,m,z] - KeijnzC[i,j,n,z])*DHjnzC[j,n-1,z]/(TmU[n-1]-TmL[n-1]);

### Temperature feasibility constraints - i in SH, j in SC, (i,j) in B ###
s.t. eq89
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
KeijnzC[i,j,n,z] ,>= 1+YijnzC[i,j,n,z] - KijmzH[i,j,m,z] - KijnzC[i,j,n,z];

s.t. eq90
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
qijnzC[i,j,n,z]/(TmU[m]-TmL[n])
,<= qijnzC[i,j,n+1,z]/(TmU[n+1]-TmL[n+1])*Cpjn[j,n]/Cpjn[j,n+1]+(1 + YijnzC[i,j,n,z]-KijmzH[i,j,m,z] - KijnzC[i,j,n,z])*DHjnzC[j,n,z]/(TmU[m]-TmL[n]);

s.t. eq91
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
qaijnzC[i,j,n,z]/(TmU[m]-TmL[n])
,<= qijnzC[i,j,n+1,z]/(TmU[n+1]-TmL[n+1])*Cpjn[j,n]/Cpjn[j,n+1]+(2-KijmzH[i,j,m,z] - KijnzC[i,j,n,z])*DHjnzC[j,n,z]/(TmU[m]-TmL[n]);

s.t. eq92
{z in (Sz inter (1..ndt-1)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m+1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n+1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n+1])) and (j in (PimH[i,m] inter PimH[i,m+1]))} :
qijmzH[i,j,m,z]/(min(TmU[m],TmU[n])-TmL[n])
,>= qijmzH[i,j,m+1,z]/(TmU[m+1]-TmL[m+1])*Cpim[i,m]/Cpim[i,m+1]+(2-KijmzH[i,j,m,z] - KijnzC[i,j,n,z])*DHimzH[i,m+1,z]/(TmU[m+1]-TmL[m+1]);

s.t. eq93
{z in (Sz inter (2..ndt)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m-1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n-1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n-1])) and (j in (PimH[i,m] inter PimH[i,m-1]))} :
KijmzH[i,j,m,z] ,<= 1 + YijmzH[i,j,m,z] - KeijmzH[i,j,m,z] - KeijnzC[i,j,n,z];

s.t. eq94
{z in (Sz inter (2..ndt)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m-1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n-1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n-1])) and (j in (PimH[i,m] inter PimH[i,m-1]))} :
(qijmzH[i,j,m,z] - qaijmzH[i,j,m,z])/(TmU[m]-TmL[n])
,<= qijmzH[i,j,m-1,z]/(TmU[m-1]-TmL[m-1])*Cpim[i,m]/Cpim[i,m-1]+(2-KeijmzH[i,j,m,z] - KeijnzC[i,j,n,z])*DHimzH[i,m,z]/(TmU[m]-TmL[n]);

s.t. eq95
{z in (Sz inter (2..ndt)), m in Mz[z],n in Mz[z], i in (SH inter Hmz[m,z] inter Hmz[m-1,z]),
j in (SC inter Cnz[n,z] inter Cnz[n-1,z]) : (TmL[n]<TmU[m]) and (TmU[n] > TmL[m]) and (i in (PjnC[j,n] inter PjnC[j,n-1])) and (j in (PimH[i,m] inter PimH[i,m-1]))} :
(qijnzC[i,j,n,z] - qaijnzC[i,j,n,z])/(TmU[n]-max(TmL[n],TmL[m]))
,<= qijnzC[i,j,n-1,z]/(TmU[n-1]-TmL[n-1])*Cpjn[j,n]/Cpjn[j,n-1]-(2-KeijmzH[i,j,m,z] - KeijnzC[i,j,n,z])*DHjnzC[j,n-1,z]/(TmU[n-1]-TmL[n-1]);

### Heat exchanger area calculation ###
s.t. eq96
{z in Sz, i in Hz[z], j in Cz[z] : (i,j) in P} :
Aijz[i,j,z] ,= sum{m in Miz[i,z]}(sum{n in Njz[j,z] : (TmL[n]<TmU[m])and(j in PimH[i,m]) and (i in PjnC[j,n])}(qimjnz[i,m,j,n,z]*(him[i,m]+hjn[j,n])/(DTmnML[m,n]*him[i,m]*hjn[j,n])));


# s.t. eq97
# {z in Sz, m in Mz[z],i in Hmz[m,z], j in (Cz[z] inter PimH[i,m]), k in (1..kmax-1) : (i,j) in B} :
# Aijzk[i,j,z,k] ,<=
# sum{l in Miz[i,z] : l<=m}(sum{n in Njz[j,z] : (TmL[n]<TmU[m])and(j in PimH[i,m]) and (i in PjnC[j,n])}
# ((qimjnz[i,l,j,n,z]-qaimjnz[i,l,j,n,z])*(him[i,l]+hjn[j,n])/(DTmnML[l,n]*him[i,l]*hjn[j,n])))
# -sum{h in (1..k-1)}(Aijzk[i,j,z,h])
# +Aijmaxz[i,j,z]*(2-KeijmzH[i,j,m,z]-Gijmzk[i,j,m,z,k]);


# s.t. eq98
# {z in Sz, m in Mz[z],i in Hmz[m,z], j in (Cz[z] inter PimH[i,m]), k in (1..kmax-1) : (i,j) in B} :
# Aijzk[i,j,z,k] ,>= 
# sum{l in Miz[i,z] : l<m}(sum{n in Njz[j,z] : (TmL[n]<TmU[m])and(j in PimH[i,m]) and (i in PjnC[j,n])}
# ((qimjnz[i,l,j,n,z]-qaimjnz[i,l,j,n,z])*(him[i,l]+hjn[j,n])/(DTmnML[l,n]*him[i,l]*hjn[j,n])))
# -sum{h in (1..k-1)}(Aijzk[i,j,z,h])
# -Aijmaxz[i,j,z]*(2-KeijmzH[i,j,m,z]-Gijmzk[i,j,m,z,k]);


# s.t. eq99
# {z in Sz, m in Mz[z],i in Hmz[m,z], j in (Cz[z] inter PimH[i,m]), k in Sk : (i,j) in B} :
# Aijzk[i,j,z,k] ,>=
# Aijz[i,j,z]-sum{h in (1..k-1)}(Aijzk[i,j,z,h]);

# s.t. eq100
# {z in Sz, m in Mz[z],i in Hmz[m,z], j in (Cz[z] inter PimH[i,m]) : (i,j) in B} :
# sum{h in Sk}(h*Gijmzk[i,j,m,z,h])
# ,= sum{l in Miz[i,z] : (l<=m) and (j in PimH[i,m])}(KijmzH[i,j,l,z]+1-YijmzH[i,j,m,z]);

# s.t. eq101
# {z in Sz, m in Mz[z],i in Hmz[m,z], j in (Cz[z] inter PimH[i,m]) : (i,j) in B} :
# sum{n in Njz[j,z] : (TmL[n]<TmU[m])and(j in PimH[i,m]) and (i in PjnC[j,n])}
# (qaimjnz[i,m,j,n,z]) ,= qaijmzH[i,j,m,z];

# s.t. eq102
# {z in Sz, m in Mz[z],n in Mz[z], i in Hmz[m,z], j in (Cnz[n,z] inter PimH[i,m]) : ((i,j) in B) and (TmL[n]<TmU[m])} :
# qaimjnz[i,m,j,n,z] ,<= qimjnz[i,m,j,n,z];

# ### Number of shells ###
# s.t. eq103
# {z in Sz, i in Hz[z], j in Cz[z] : ((i,j) in P) and (not ((i,j) in B))} :
# Aijz[i,j,z] ,<= Aijmaxz[i,j,z]*Uijz[i,j,z];

# s.t. eq104
# {z in Sz, i in Hz[z], j in Cz[z], k in Sk : ((i,j) in P) and  ((i,j) in B)} :
# Aijzk[i,j,z,k] ,<= Aijmaxz[i,j,z]*Uijzk[i,j,z,k];


###################
###  OBJECTIVE  ###
###################


minimize totalCost:
sum{z in Sz, i in HUz[z],j in Cz[z] : (i,j) in P}(ciH[i]*FiH[i]*DTi[i])
+	sum{z in Sz, j in CUz[z],i in Hz[z] : (i,j) in P}(cjC[j]*FjC[j]*DTj[j])
# + 	sum{z in Sz, i in Hz[z],j in Cz[z] : ((i,j) in P) and(not ((i,j) in B))}(cijF[i,j]*Uijz[i,j,z]+cijA[i,j]*Aijz[i,j,z])
# +	sum{k in Sk, z in Sz, i in Hz[z],j in Cz[z] : (i,j) in P}(cijF[i,j]*Uijzk[i,j,z,k] + cijA[i,j]*Aijzk[i,j,z,k]);       
+ 	sum{z in Sz, i in Hz[z],j in Cz[z] : ((i,j) in P) }(cijF[i,j]*Uijz[i,j,z]+cijA[i,j]*Aijz[i,j,z]);

solve;
	   
###  OUTPUT ###
#display{iU in SUtGroups}: iU,VFactUt[iU];
	   
end;
