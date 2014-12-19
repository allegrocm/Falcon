#include "VRJSyncMonitor.h"

//lots of Juggler includes
#include <vpr/vpr.h>
#include <vpr/IO/SerializableObject.h>
#include <vpr/IO/Socket/SocketDatagram.h>
#include <vpr/IO/TimeoutException.h>
#include <vpr/Util/Debug.h>
#include <vpr/Util/Interval.h>
#include <cluster/ClusterManager.h>
#include <gadget/Node.h>
#include <gadget/NetworkManager.h>

#include <plugins/ApplicationDataManager/UserData.h>
#include <vpr/IO/SerializableObject.h>
#include <vector>
#include <map>

class _syncThing
{
public:
	unsigned int valOut;
	unsigned int valIn;
	_syncThing()	{valOut = -1;  valIn = valOut;}
};

namespace vpr
{
	template<>
	inline void vpr::SerializableObjectMixin<_syncThing>::writeObject(vpr::ObjectWriter* writer)
	{ 
		//printf("Writing %i strings\n", stringData.size());
		writer->writeUint32(valOut);
		

	}

	template<>
	inline void vpr::SerializableObjectMixin<_syncThing>::readObject(vpr::ObjectReader* reader)
	{
		valIn = reader->readUint32();

	}
}


cluster::UserData<vpr::SerializableObjectMixin<_syncThing> > _gSync;		//the big cluster data holder.  used for juggler
namespace RNGSyncMonitor
{
	void init()
	{
		//init with a guid and stuff
		vpr::GUID new_guid("15c09c99-ed6d-4994-bbac-83587d4400d1");
		_gSync.init(new_guid);	
	}
	
	void preFrame()
	{
		_gSync->valOut = rand()%255;
		_gSync->valIn = _gSync->valOut;		//sync now because this won't let overwritten on the master node
	
	}
	
	void latePreFrame()
	{
		printf("RNGSync:  local was %i, received %i\n", _gSync->valOut, _gSync->valIn);
	}
	
	bool isSynced()
	{
		return (_gSync->valOut == _gSync->valIn);
	}
	
}
