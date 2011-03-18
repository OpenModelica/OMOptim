# Simple Milp Utility Target
#
# Based on article :
# Process integration: Selection of the optimal utility system
# Maréchal, François and Kalitventzeff, Boris


###  SETS AND PARAMETERS  ###
set STk; # Temperatures
set SDTk;
set SProcStreams; # Process streams
set SUtStreams;
set SUtGroups; #Utility groups
set SUtStrGroups within {iUt in SUtGroups, iUS in SUtStreams};

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
var VRk 'heat residual' {k} >=0; 


###  CONSTRAINTS  ###
# temperature interval heat equation

s.t. CDQK{ik in dk}:
		#sum fw * sum(qwk)
		sum{iU in SUtGroups,iUS in SUtStreams : (iU,iUS) in SUtStrGroups} VFactUt[iU]*DQuk[iUS,ik]
		# sum Qik
		+sum{iPS in SProcStreams} DQpk[iPS,ik]
		# + Rk+1 - Rk
		+ VRk[ik+1]-VRk[ik]
		#==0
		, =0;

#no heat propagation from cold to hot temperature zone
s.t. secPrincipe{ik in k} : VRk[ik] , >= 0;

#all needs provided
s.t. noLeak0 : VRk[1] , = 0;
s.t. noLeakK : VRk[nbTemperatures] , = 0;

s.t. factMin{iUt in SUtGroups} : VFactUt[iUt] - fmin[iUt] , >=0;
s.t. factMax{iUt in SUtGroups} : VFactUt[iUt] - fmax[iUt] , <=0;

###  OBJECTIVES  ###

minimize totalCost:
	sum{iu in SUtGroups}(costFix[iu]+costMult[iu] * VFactUt[iu]);
       
solve;
	   
###  OUTPUT ###
display{iU in SUtGroups}: iU,VFactUt[iU];
	   
end;
