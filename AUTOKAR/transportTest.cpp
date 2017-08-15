#include <iostream>
#include <array>
#include <string>

// transport is a base class to define behavior
// for a bus transport

// it provide initialization for the underlying transport
// it provide a method that send char buffers
// it provide a method that receive char buffers

using namespace std;

enum class TransportStatus {ok, warning, error, other};

class BusTransportBase{
public:
    BusTransportBase() {}

    // request the underlying transport to send the data
    virtual int sendThis( char* data, int size );
    // ask underlying transport if there is any data to receive
    virtual bool anythingToReceive();
    // receive data from the underlying transport
    virtual int receive( char* data, int *size);
    // request underlying status ok, warning, error, other
    virtual int status( TransportStatus& status );
    // request underlying transport error/warning message
    virtual int statusMessage( string& statusMessage );

protected:

};




int main(){





    return 0;
}