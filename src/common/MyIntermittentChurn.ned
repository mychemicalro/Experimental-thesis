//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package oversim.common;


// 
// Churn generating module. 
// Removes and adds churnRate nodes spaced by a larger interval than churnInterval
// Example: ---------2-2-2---------2-2-2---------, where 2 are the processes removed and added, and - 
// are the time intervals. 
//

simple MyIntermittentChurn extends ChurnGenerator{
	parameters:
		@class(MyIntermittentChurn);
		int churnRate = default(1); // number of leaving/joining nodes 
		double churnInterval @unit(s) = default(6s); // interval between churns
		double createInterval @unit(s) = default(1s); // interval between creation of nodes
		int joiners = default(2); // how many nodes have to join
		int leavers = default(2); // how many nodes have to leave 
		bool stopSimulation = default(true);
		double endSimulationInterval @unit(s) = default(20s); // end the simulation after this amount of seconds (after start of the stability period)
		int permanentNodes = default(0);
		
		double intervalBetweenChurnPeriods @unit(s) = default(40s); // interval between the churnPeriods. 
		int rounds = default(2);  // rounds of churn in a churn period. 
		
}


