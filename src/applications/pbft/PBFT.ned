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

package oversim.applications.pbft;

import oversim.common.BaseApp;

simple Blockchain{
	@display("i=block/buffer");
}

simple ReplicaState{
	@display("i=block/table2");
	int f = default(1); 		// number of max simultaneous faulty nodes 
	    
}

simple PBFT extends BaseApp
{
    parameters:
        @class(PBFT);
        int k = default(2); 					   // number of nodes to whom gossip a message
        double joinDelay @unit(s) = default(1s);   // seconds before the application tries to join the network 
        int blockCapacity = default(1);			   // the number of operations a block can contain 
        double requestDelay @unit(s) = default(10s); 	// the number of seconds between a client's request and the next one
        double replyDelay @unit(s) = default(3s); 		// the interval after a request is considered expired by the client, that will resend it. 
        double clientProb = default(0.5); 				// the probability a node is also a client
        int checkpointPeriod = default(16); 			// PBFT K parameter  
        int simDuration = default(200); 					// Simulation duration 
}

import oversim.common.ITier;

module PBFTApplicationModules like ITier
{
    gates:
        input udpIn;             // gate from the UDP layer
        output udpOut;           // gate to the UDP layer
        input from_lowerTier;    // gate from the lower tier
        input from_upperTier;    // gate from the upper tier
        output to_lowerTier;     // gate to the lower tier
        output to_upperTier;     // gate to the upper tier
        input trace_in;          // gate for trace file commands
        input tcpIn;             // gate from the TCP layer
        output tcpOut;           // gate to the TCP layer

    submodules:
        pbft: PBFT{
        	parameters:
                @display("p=60,60");
        }
        chain: Blockchain{
        	parameters:
                @display("p=150,60");
        }
        replicaState: ReplicaState{
        	parameters:
                @display("p=240,60");
        }

    connections allowunconnected:
        from_lowerTier --> pbft.from_lowerTier;
        to_lowerTier <-- pbft.to_lowerTier;
        udpIn --> pbft.udpIn;
        udpOut <-- pbft.udpOut;
        tcpIn --> pbft.tcpIn;
        tcpOut <-- pbft.tcpOut;
        trace_in --> pbft.trace_in;
}

