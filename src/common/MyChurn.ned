package oversim.common;


// 
// Churn generating module. Creates and removes churnRate nodes each time
//

simple MyChurn extends ChurnGenerator{
	parameters:
		@class(MyChurn);
		int churnRate = default(1); // number of leaving/joining nodes 
		double churnInterval @unit(s) = default(6s); // interval between churns
		double createInterval @unit(s) = default(1s); // interval between creation of nodes
		int joiners = default(2); // how many nodes have to join
		int leavers = default(2); // how many nodes have to leave 
		bool stopSimulation = default(true);
		double endSimulationInterval @unit(s) = default(20s); // end the simulation after this amount of seconds (after start of the stability period)
		int permanentNodes = default(0);
		bool useLease = default(true);
}


