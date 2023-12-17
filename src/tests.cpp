
#include <CppUTest/TestHarness.h>
#include <CppUTest/CommandLineTestRunner.h>

#define __TESTS
#include "ports/ports.hpp"

//Ports
TEST_GROUP(Ports)
{
   void setup()
   {

   }

   void teardown()
   {

   }

};

TEST(Ports, ShouldBeWriteable)
{
    volatile byte value;
    Port port( Ports::convert(&value) );

    port = 5;

    CHECK_EQUAL(5, value);
}


TEST(Ports, ShouldBeReadWrite)
{
    volatile byte value;
    Port port( Ports::convert(&value) );

    port = 5;
    const byte result = port;

    CHECK_EQUAL(5, result);
}


TEST(Ports, ShouldAllowToSetBits)
{
    volatile byte value;
    Port port( Ports::convert(&value) );

    port[4] = true;
    CHECK_EQUAL(1 << 4, value);

    port[1] = true;
    CHECK_EQUAL( (1 << 4) | (1 << 1), value );

    port[7] = true;
    CHECK_EQUAL( (1 << 4) | (1 << 1) | (1 << 7), value );
}


TEST(Ports, ShouldAllowToTestBits)
{
    volatile byte value = 0;
    Port port( Ports::convert(&value) );

    port[4] = true;
    port[1] = true;
    port[7] = true;

    const bool test0 = port[0];
    const bool test1 = port[1];
    const bool test2 = port[2];
    const bool test3 = port[3];
    const bool test4 = port[4];
    const bool test5 = port[5];
    const bool test6 = port[6];
    const bool test7 = port[7];

    CHECK_EQUAL(false, test0);
    CHECK_EQUAL(true,  test1);
    CHECK_EQUAL(false, test2);
    CHECK_EQUAL(false, test3);
    CHECK_EQUAL(true,  test4);
    CHECK_EQUAL(false, test5);
    CHECK_EQUAL(false, test6);
    CHECK_EQUAL(true,  test7);
}


//IOPorts
TEST_GROUP(IOPorts)
{
   void setup()
   {
      data.pin = 0;
      data.ddr = 0;
      data.port = 0;
   }

   void teardown()
   {

   }

   struct __attribute__((packed)) Data
   {
       volatile byte pin;
       volatile byte ddr;
       volatile byte port;
   } data;

};


TEST(IOPorts, ShouldWriteToPort)
{
    IOPort ioport( Ports::convert( &(data.port) ) );

    ioport = 5;
    CHECK_EQUAL(5, data.port);
}


TEST(IOPorts, ShouldReadFromPin)
{
    IOPort ioport( Ports::convert( &(data.port) ) );

    data.pin = 0xAB;
    const byte value = ioport;

    CHECK_EQUAL(0xAB, value);
}


TEST(IOPorts, ShouldGiveWriteAccessToDDR)
{
    IOPort ioport( Ports::convert( &(data.port) ) );

    ioport.dir = 0x5A;

    CHECK_EQUAL(0x5A, data.ddr);
}


TEST(IOPorts, ShouldGiveReadWriteAccessToDDR)
{
    IOPort ioport( Ports::convert( &(data.port) ) );

    ioport.dir = 0x98;
    const byte value = ioport.dir;

    CHECK_EQUAL(0x98, value);
}

TEST(IOPorts, ShouldSetProperBitsInPort)
{
    IOPort ioport( Ports::convert( &(data.port) ) );

    ioport[0] = true;
    ioport[3] = true;
    ioport[7] = true;

    CHECK_EQUAL( (1 << 0) | (1 << 3) | (1 << 7), data.port);
    CHECK_EQUAL(0, data.ddr);
    CHECK_EQUAL(0, data.pin);
}


TEST(IOPorts, ShouldAllowToTestBitsOfPin)
{
    IOPort ioport( Ports::convert( &(data.port) ) );

    data.pin = (1 << 1) | (1 << 5) | (1 <<7 );

    const bool test0 = ioport[0];
    const bool test1 = ioport[1];
    const bool test2 = ioport[2];
    const bool test3 = ioport[3];
    const bool test4 = ioport[4];
    const bool test5 = ioport[5];
    const bool test6 = ioport[6];
    const bool test7 = ioport[7];

    CHECK_EQUAL(false, test0);
    CHECK_EQUAL(true,  test1);
    CHECK_EQUAL(false, test2);
    CHECK_EQUAL(false, test3);
    CHECK_EQUAL(false, test4);
    CHECK_EQUAL(true,  test5);
    CHECK_EQUAL(false, test6);
    CHECK_EQUAL(true,  test7);

    CHECK_EQUAL(0, data.ddr);
    CHECK_EQUAL(0, data.port);
}

int main(int ac, char** av)
{
   return CommandLineTestRunner::RunAllTests(ac, av);
}
