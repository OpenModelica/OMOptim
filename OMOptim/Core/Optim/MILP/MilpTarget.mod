# Hubert Heat Load Target w. constraints
#
#


###  SETS AND PARAMETERS  ###
set STk; # Temperatures
set SDTk;
set SStreams; #all streams
set SProcStreams within SStreams; # Process streams
set SUtStreams within SStreams; # Utility streams
set SHotStreams within SStreams;
set SColdStreams within SStreams;
set SUtGroups; #Utility groups
set SUtStrGroups within {iUt in SUtGroups, iUS in SUtStreams};
set SForbConn within {si in SHotStreams, sj in SColdStreams};
 

param nbTemperatures > 0 integer;
param nbUtGroups >=0 integer;

set dk := 1.. nbTemperatures-1;
set k := 1.. nbTemperatures;

### Parameters ###
param DQpk{SProcStreams,ik in dk} default 0;
param DQuk{SUtStreams,ik in dk}  default 0;
param fmin{SUtGroups}  default 0;
param fmax{SUtGroups}  default 0;
param costFix{SUtGroups}  default 0;
param costMult{SUtGroups}  default 1;

###  VARIABLES  ###
var VFactUt 'mult fact' {SUtGroups} >=0;
var VRik 'heat residual' {SHotStreams,k} >=0; 
var Qijk 'heatexchange' {SHotStreams,SColdStreams,dk};
var QHik 'hot load' {SHotStreams,dk};
var QCjk 'cold load' {SColdStreams,dk};


###  CONSTRAINTS  ###
# temperature interval heat equation

s.t. CDQK{si in SHotStreams, ik in dk} :
		sum{sj in SColdStreams} Qijk[si,sj,ik]
		# - Rik+1 + Rik
		- VRik[si,ik+1]+VRik[si,ik]
		#==DQpk[si,ik]
		-QHik[si,ik], =0;

# Qiik = 0
s.t. Qijkpos{si in SHotStreams,sj in SColdStreams, ik in dk} :
Qijk[si,sj,ik] ,>=0;
	
#Qijk = Qck
s.t. QCijk{sj in SColdStreams,ik in dk} :
	sum{si in SHotStreams}(Qijk[si,sj,ik])-QCjk[sj,ik],=0;

# Process : sum Qijk = DQpk
#s.t. Qipk{si in SProcStreams, ik in dk} :
#	sum{sj in SStreams} Qijk[si,sj,ik] - DQpk[si,ik] ,=0;
	
# Process hot streams :
s.t. PHS{si in SHotStreams inter SProcStreams, ik in dk} :
	QHik[si,ik] - DQpk[si,ik] ,=0;

# Process cold streams :
s.t. PCS{sj in SColdStreams inter SProcStreams, ik in dk} :
	QCjk[sj,ik] - DQpk[sj,ik] ,=0;

# Utility hot streams :
s.t. UHS{group in SUtGroups,si in SHotStreams inter SUtStreams, ik in dk : (group,si) in SUtStrGroups} :
	QHik[si,ik] - VFactUt[group]*DQuk[si,ik] ,=0;

# Utility cold streams :
s.t. UCS{group in SUtGroups,sj in SColdStreams inter SUtStreams, ik in dk : (group,sj) in SUtStrGroups} :
	QCjk[sj,ik] - VFactUt[group]*DQuk[sj,ik] ,=0;
	
# Forbidden connections
s.t. ForbConn{si in SHotStreams, sj in SColdStreams,ik in dk : (si,sj) in SForbConn} :
	Qijk[si,sj,ik] ,=0;

#no heat propagation from cold to hot temperature zone
s.t. secPrincipe{si in SHotStreams,ik in k} : VRik[si,ik] , >= 0;

#all needs provided
s.t. noLeak0{si in SHotStreams} : VRik[si,1] , = 0;
s.t. noLeakK{si in SHotStreams} : VRik[si,nbTemperatures] , = 0;

s.t. factMin{iUt in SUtGroups} : VFactUt[iUt] - fmin[iUt] , >=0;
s.t. factMax{iUt in SUtGroups} : VFactUt[iUt] - fmax[iUt] , <=0;

###  OBJECTIVES  ###

minimize totalCost:
	sum{iu in SUtGroups}(costFix[iu]+costMult[iu] * VFactUt[iu]);
       
solve;
	   
###  OUTPUT ###
display{iU in SUtGroups}: iU,VFactUt[iU];
	   
end;
