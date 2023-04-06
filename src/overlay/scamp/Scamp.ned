package oversim.overlay.scamp;
import oversim.common.BaseOverlay;
import oversim.common.IOverlay;

simple View {
    @display("i=block/table");
	int maxSize = default(3);						// Size of the view				
}

simple Scamp extends BaseOverlay {
    parameters:
        @class(Scamp);
        double joinDelay @unit(s); 					// number of seconds between join retries
        double heartbeatDelay @unit(s); 			// number of seconds between heartbeat messages
        double pruneDelay @unit(s) = default(3s);
        double heartbeatTimeout @unit(s); 			// number of seconds before a node is considered 
        												//out of the inView
        int joinRetry;
        int joinRequestCopies = default(3);			// SCAMP parameter
        bool useHeartbeats = default(true);
        bool useCwhenLeaving = default(true);
}


module ScampOverlayModules like IOverlay {
    parameters:
        @display("i=block/network2");
        
    gates:
        input udpIn;   	  // gate from the UDP layer
        output udpOut;    // gate to the UDP layer
        input tcpIn;      // gate from the TCP layer
        output tcpOut;    // gate to the TCP layer
        input appIn;      // gate from the application
        output appOut;    // gate to the application

    submodules:
        scamp: Scamp {
            parameters:
                @display("p=60,60");
        }
        inView: View {
            parameters:
                @display("p=150,60");
                maxSize = 0;
        }
        partialView: View {
            parameters:
                @display("p=240,60");
        }
        seenNodesView: View {
            parameters:
                @display("p=330,60");
                maxSize = 42;
        }

    connections allowunconnected:
        udpIn --> scamp.udpIn;
        udpOut <-- scamp.udpOut;
        appIn --> scamp.appIn;
        appOut <-- scamp.appOut;
}
