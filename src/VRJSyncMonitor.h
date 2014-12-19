//a little class that checks RNG synchronization for Juggler apps

#ifndef KENS_VRJ_SYNC_MONITOR_H
#define KENS_VRJ_SYNC_MONITOR_H

namespace RNGSyncMonitor
{
	void init();
	void preFrame();		//generates a new number.  Master will write it and send it over the network
	void latePreFrame();	//compares internal number to that 
	bool isSynced();		//lets us know if we're still in sync
};

#endif
