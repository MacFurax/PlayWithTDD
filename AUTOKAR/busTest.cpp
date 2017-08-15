#include <iostream>
#include <string>
#include <cassert>
#include <queue>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

// bus initialize with a node name must be unique
// bus send heart beat on regular basis
// sending of data using a thread
// message to send are first added to a queue
// the sending thread consume the queue to sendthe messages
// bus allow service to connect to him

using namespace std;

class Bus{
public:
    Bus(string nodeName) : mNodeName{nodeName} {

    }

    string getNodeName() { return mNodeName; }

    bool sendMessage( string message ) { 
        {
            lock_guard<mutex> guard(mMessageQueueMutex);
            mMessageQueue.push( message);
        } 
    }
    int messageLeftToSendCount() { return mMessageQueue.size(); }
    int messageSentCount() { return mMessageSentCount.load(); }

    void start() {
        mStartedThread.store(true);
        mThread = thread( &Bus::threadSendAndReceive, this );
    }

    void stop() {
        mStartedThread.store(false);
        mThread.join();
    }

protected:
    string mNodeName{""};
    queue<string> mMessageQueue;
    atomic<bool> mStartedThread{false};
    thread mThread;
    mutex mMessageQueueMutex;
    atomic<int> mMessageSentCount{0};

    // thread method to send message over transport layer
    void threadSendAndReceive() {

        string message{""};
        int messageToSendPerLoop = 10;

        while( mStartedThread.load() )
        {

            { // this is there to make sure lock_guard release mutex upon exit of scope
                lock_guard<mutex> guard(mMessageQueueMutex);
                while( mMessageQueue.size() > 0 && messageToSendPerLoop != 0 ){
                    
                    message = mMessageQueue.front();
                    mMessageQueue.pop();
                    mMessageSentCount.store( mMessageSentCount.load() + 1);
                    messageToSendPerLoop--;

                    // do something with message
                    cout << "send message [" << message << "]\n";

                }
            }

            messageToSendPerLoop = 10;
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        return;
    }

};

class BusTest : Bus {

public:
    BusTest() : Bus( "/alterface/master" ) {
        cout << "Is atomic<bool> lock free : " << atomic<bool>{}.is_lock_free() << "\n";
    }

    void test01(){
        // check bus get a node name
         assert( getNodeName() == "/alterface/master" );
    }

    void test02(){
        sendMessage("blabla01");
        // check queue is well filled
        assert( messageLeftToSendCount() == 1);
    }

    void test03(){
        // check that we have a message and check content
        string tmp = mMessageQueue.front();
        assert( tmp == "blabla01" );
    }

    void test04(){
        // test consumption of the message by thread ?
        start();
        this_thread::sleep_for(chrono::milliseconds(200));
        stop();
        // test that thread cosume the message
        assert( messageLeftToSendCount() == 0);
    }

    void test05(){
        // test concurent access is ok
        start();
        sendMessage("Message01");
        sendMessage("Message02");
        sendMessage("Message03");
        sendMessage("Message04");
        sendMessage("Message05");
        sendMessage("Message06");
        sendMessage("Message07");
        sendMessage("Message08");
        sendMessage("Message09");
        sendMessage("Message10");
        sendMessage("Message11");
        sendMessage("Message12");
        sendMessage("Message13");
        this_thread::sleep_for(chrono::milliseconds(200));
        stop();
        assert( messageLeftToSendCount() == 0);
        assert( messageSentCount() == 14 );
    }

    void runAllTest(){
        test01();
        test02();
        test03();
        test04();
        test05();
    }

};


int main()
{
    // create a bus with a node name
    BusTest toTest;

    toTest.runAllTest();

    return 0;
}