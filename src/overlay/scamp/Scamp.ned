package oversim.overlay.scamp;

// import BaseOverlay
import oversim.common.BaseOverlay;
import oversim.common.IOverlay;


simple View {
    @display("i=block/table");
	int maxSize = default(3);
}

// there are a lot of gates and params that are inherited from BaseOverlay
simple Scamp extends BaseOverlay {
    parameters:
        @class(Scamp);
        double joinDelay @unit(s); // delay between join retries (sec)
        double heartbeatDelay @unit(s); // delay between imAlive triggers (sec)
        double pruneDelay @unit(s) = default(3s);
        double heartbeatTimeout @unit(s); // delay after which a node is considered out of the inView
        int joinRetry;
        int joinRequestCopies = default(3);
        bool useHeartbeats = default(true);
        bool useCwhenLeaving = default(true);
}


// all messages that must be sent to scamp, will be sent to the wrapper instead
module ScampOverlayModules like IOverlay {
    parameters:
        @display("i=block/network2");
        
    gates:
        input udpIn;   // gate from the UDP layer
        output udpOut;    // gate to the UDP layer
        input tcpIn;    // gate from the TCP layer
        output tcpOut;    // gate to the TCP layer
        input appIn;   // gate from the application
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
