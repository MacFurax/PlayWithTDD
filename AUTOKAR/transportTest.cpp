#include <iostream>
#include <array>
#include <string>
#include <cassert>
#include <vector>

#include "Poco/Timespan.h"
#include "Poco/Net/Net.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/MulticastSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/NetworkInterface.h"

// transport is a base class to define behavior
// for a bus transport

// it provide initialization for the underlying transport
// it provide a method that send char buffers
// it provide a method that receive char buffers

using namespace std;
using namespace Poco;
using namespace Poco::Net;
/*using Poco::TimeSpan;
using Poco::Net::Socket;
using Poco::Net::DatagramSocket;
using Poco::Net::SocketAddress;
using Poco::Net::IPAddress;
using Poco::Net::NetworkInterface;
using Poco::Net::MulticastSocket;*/
//using Poco::Net::SocketList;

enum class TransportStatus {ok, warning, error, other};

class BusTransportBase{
public:
    BusTransportBase() {}

    // allow underlying transport to initialize
    // virtual TransportStatus initialize() = 0;
    // request the underlying transport to send the data
    virtual TransportStatus sendThis( void* data, int size ) = 0;
    // ask underlying transport if there is any data to receive
    virtual bool anythingToReceive() = 0;
    // receive data from the underlying transport
    virtual TransportStatus receive( void* data, int *size) = 0;
    // request underlying status ok, warning, error, other
    TransportStatus status() { return mStatus; }
    // request underlying transport error/warning message
    vector<string> statusMessage() { return mStatusMessages;}
    // clear status flags
    void clearStatus(){ 
        mStatus = TransportStatus::other; 
        mStatusMessages.clear();
    }

protected:
    TransportStatus mStatus =  TransportStatus::error;
    vector<string> mStatusMessages{"Not initialized"};
};

class MultiCastBusTransport : public BusTransportBase
{

public:

    MultiCastBusTransport( string mcIp, int mcPort ) : BusTransportBase(), mGroup(mcIp, mcPort) /*, mIf(findInterface())*/
    {
        //mSocket.bind(SocketAddress(IPAddress(), mGroup.port()), true);
        //mSocket.joinGroup(mGroup.host(), mIf);
        
        clearStatus();
        mStatus = TransportStatus::ok;
    }

    ~MultiCastBusTransport()
    {
        mSocket.close();
    }

    TransportStatus sendThis( void* data, int size ) override 
    {
        MulticastSocket ms;
        //int sentCount = mSocket.sendTo( static_cast<void*>(data), size, mGroup );
        int sentCount = mSocket.sendTo( static_cast<void*>(data), size, mGroup );
        if( sentCount!= size) return TransportStatus::warning;
        return TransportStatus::ok;
    }

    bool anythingToReceive() override 
    {
        Poco::Timespan span(250000);

        if (mSocket.poll(span, Socket::SELECT_READ))
		{
            return true;
        }

        return false;
    }

    TransportStatus receive( void* data, int *size) override 
    {
        SocketAddress from;
        *size = mSocket.receiveFrom( data, *size ,from);
        cout << "received " << *size << " bytes from " << from.toString() << "\n";

        return TransportStatus::error;
    }
    
    NetworkInterface findInterface()
    {
        NetworkInterface::Map m = NetworkInterface::map();
        for (NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it)
        {
            if (it->second.supportsIPv4() && 
                it->second.firstAddress(IPAddress::IPv4).isUnicast() && 
                !it->second.isLoopback() && 
                !it->second.isPointToPoint())
            {
                return it->second;
            }
        }
        return NetworkInterface();
    }

protected:

    MulticastSocket  mSocket;
	SocketAddress    mGroup;

    
};

class BusTransportBaseTest : public BusTransportBase
{
public:
    BusTransportBaseTest( int thisParam ) : BusTransportBase(), thisParam{thisParam} {
        mStatus =  TransportStatus::ok;
    }



    TransportStatus sendThis( void* data, int size ) override {
        return TransportStatus::error;
    }

    bool anythingToReceive() override {
        return false;
    }

    TransportStatus receive( void* data, int *size) override {
        return TransportStatus::error;
    }

    void test01(){
        // after creation transport shuold have an ok status
        cout << "Test 01 : After creation the transport must be ok\n";
        assert( status() == TransportStatus::ok );
    }

    void test02(){
        cout << "Test 02 : Once clear the status should equal to other and messages vector empty\n";
        clearStatus();
        assert( status() == TransportStatus::other && statusMessage().size() == 0 );
    }

    

    void runAllTest(){
        test01();
        test02();
    }

private:
    int thisParam;

};

class MultiCastBusTransportTest : public MultiCastBusTransport{

public:
    MultiCastBusTransportTest() : MultiCastBusTransport("239.255.1.2", 12345) {}

    void test01(){
        cout << "Test 02-01 : After creation the transport must be ok\n";
        assert( status() == TransportStatus::ok );
    }

    void test02(){
        cout << "Test 02-02 : sendThis must return either ok or warning\n";
        TransportStatus s = sendThis((void*)"Hellox",6); 
        assert(  s == TransportStatus::ok || s == TransportStatus::warning );
    }

    void test03(){
        cout << "Test 02-03 : anythingToReceive should return true when something is ready to read\n";
        bool toReceive = anythingToReceive();
        cout << "Anything to read " << toReceive << "\n";
        if( toReceive)
        {
            char buffer[256];
            int size = 256;
            receive( buffer, &size);
            cout << "received buffer " << buffer << " size " << size << "\n";
        }
            
    }

    void test04(){
        
    }

    void runAllTest()
    {
        test01();
        test02();
        test03();
        test04();
    }

protected:

};



int main(){

    BusTransportBaseTest toTest(10);

    toTest.runAllTest();

    MultiCastBusTransportTest toTest2;

    toTest2.runAllTest();

    return 0;
}