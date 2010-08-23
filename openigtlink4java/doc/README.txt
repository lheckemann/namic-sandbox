*******Specs

This library has been developped from the 2 following documents specifications :
http://www.na-mic.org/Wiki/index.php/OpenIGTLink/Protocol
http://www.spl.harvard.edu/publications/item/view/1709
where document "OpenIGTLink: an open network protocol for image-guided therapy environment" can be downloaded from

*******How to manage get source and how to contribute

        You can browse the code online :
http://code.google.com/p/openigtlink4java/source/browse/
        You should get the code by using the command:
hg clone https://openigtlink4java.googlecode.com/hg/ openigtlink4java

        I selected mercurial version management to be able to work offline.
        For eclipse I use MercurialEclipse plugin following this tutorial.
http://blogs.intland.com/main/entry/39
        You can found some more explanations on how to use it at :
http://www.javaforge.com/project/HGE#download


*******How to use OpenIGTLink4Java Library ?

OpenIGTLink4Java Java Library use a queued request pattern.
We have for each Client 2 queues one where request to server are queued to be sent and one where response from server are queued to be proceeded.

We have for each server one ServerThread for each client and for each ServerThread 2 queues one to where request of client are queued to be proceeded and one where response to client are queued to be sent.

To avoid problems all queues are sorted in First In First Out order and are Thread safe.
I use this pattern to avoid to block on communication, to avoid to create to much thread and to respect processes orders.
This structure allow Clients and Servers to be pusher or puller depending on final implementation.
Push side and pull side are asynchronous because they are in different threads and this for client and server.

On the client side.
Developer will have to use one RequestQueueManager to queue its messages.(using addRequest method) for each server they want to communicate with.
All message Types supported and only them must be declared in enum Capability in ResponseHandler
This enum is use to generate Exception for Types not supported and to build the correct CapabilityMessage.
RequestQueueManager create one OpenIGTClient listening for responses messages.
OpenIGTClient will create a ResponseHandler for each message received and will queue them to the ResponseQueueManager. (using addResponse method)
Users has to adapt methods ResponseHandler.perform to get the right things done when client receive message.
Users has to adapt methods ResponseHandler.manageError to get the right things done when client get an error (checksum, wrong type, software errors etc..)
manageError method is critical for the security of equipments using this library.

On the server side.
Developer will have to use one OpenIGTServer.
All message Types supported and only them must be declared in enum Capability in MessageHandler
This enum is use to generate Exception for Types not supported and to build the correct CapabilityMessage.
OpenIGTServer will create one ServerThread for each client.
ServerThread will create a MessageHandler for each message received and will queue them to the MessageQueueManager. (using addMessage method)
Users has to adapt methods MessageHandler.perform to get the right things done when client receive message.
Users has to adapt methods MessageHandler.manageError to get the right things done when client get an error (checksum, wrong type, software errors etc..)
manageError method is critical for the security of equipments using this library.

For both Clients and servers
Using Messages objects provided in this library users have not to manage Header crc calculation, timestamp calculation, data encoding etc.
Each message to send is created using the default Constructor of differents messages Objects, passing it's deviceName only.
Then user has to use some setters methods to give elements required for the body of the message.
All message Types supported and only them must be declared in enum Capability in MessageHandler

OpenIGTClient and OpenIGTServer has each one a Status Object containing them current status values.
User must call SetStatus() methods from MessageHandler and from ResponseHandler to set Status values when current status change.
User must use GetStatus methods  from MessageHandler and from ResponseHandler to build StatusMessage when required

*******Some explanation about Crc calculation
Looking at 
http://en.wikipedia.org/wiki/Cyclic_redundancy_check
http://en.wikipedia.org/wiki/Mathematics_of_CRC<
http://regregex.bbcmicro.net/crc-catalogue.htm
Actually, the polynomial used for OpenIGTLink's 64-bit CRC is based on ECMA-182 standard.
http://www.ecma-international.org/publications/files/ECMA-ST/Ecma-182.pdf

Actually, the polynomial used for OpenIGTLink's 64-bit CRC is based on ECMA-182 standard.
from Ecma specs we can say that polynome is :
        poly is: x^64 + x^62 + x^57 + x^55 + x^54 + x^53 + x^52 + x^47 + x^46 +
         x^45 + x^40 + x^39 +
         x^38 + x^37 + x^35 + x^33 + x^32 + x^31 + x^29 + x^27 + x^24 + x^23 +
         x^22 + x^21 +
         x^19 + x^17 + x^13 + x^12 + x^10 + x^9 + x^7 + x^4 + x^1 + 1

        // represented here with lsb = highest degree term

        // 1100100101101100010101111001010111010111100001110000111101000010_
        // || | | || || | | |||| | | ||| | |||| ||| |||| | | |
        // || | | || || | | |||| | | ||| | |||| ||| |||| | | +- x^64 (implied)
        // || | | || || | | |||| | | ||| | |||| ||| |||| | |
        // || | | || || | | |||| | | ||| | |||| ||| |||| | +--- x^62
        // || | | || || | | |||| | | ||| | |||| ||| |||| +-------- x^57
        // .......................................................................
        // ||
        // |+---------------------------------------------------------------- x^1
        // +----------------------------------------------------------------- x^0

Calculating crc64Polynomial for this polynome we get : crc64Polynomial = 0xC96C5795D7870F42L;
You can get proof of this from the Test.java code.
To get a fast calculation some algorithms has been developped the most popular use a crc Table
        // input is dividend: as
         0000000000000000000000000000000000000000000000000000000000000000<8-bit byte>
        // where the lsb of the 8-bit byte is the coefficient of the highest degree
        // term (x^71) of the dividend
        // so division is really for input byte * x^64

        // you may wonder how 72 bits will fit in 64-bit data type... well as the
        // shift-right occurs, 0's are supplied
        // on the left (most significant) side ... when the 8 shifts are done, the
        // right side (where the input
        // byte was placed) is discarded

        // when done, table[XX] (where XX is a byte) is equal to the CRC of 00 00 00
        // 00 00 00 00 00 XX
        //

Mathematical discussion about algoritms can be found in the following links
http://www.ifp.illinois.edu/~sarwate/pubs/Sarwate88Computing.pdf
http://www.dtic.mil/cgi-bin/GetTRDoc?AD=ADA013939&Location=U2&doc=GetTRDoc.pdf
http://delivery.acm.org/10.1145/30000/28572/p617-griffiths.pdf?key1=28572&key2=5448951821&coll=GUIDE&dl=GUIDE&CFID=15151515&CFTOKEN=6184618

Explanation detailed below comes from comment that you can find in the code located at followin link :
http://andrewl.dreamhosters.com/archive/52662441.cpp
My implementation does not follow at all this code but I have been able to test my implementation using the tests case describe bellow.

        // will give an example CRC calculation for input array {0xDE, 0xAD}
        //
        // each byte represents a group of 8 coefficients for 8 dividend terms
        //
        // the actual polynomial dividend is:
        //
        // = DE AD 00 00 00 00 00 00 00 00 (hex)
        // = 11011110 10101101 0000000000000000000...0 (binary)
        // || |||| | | || |
        // || |||| | | || +------------------------ x^71
        // || |||| | | |+-------------------------- x^69
        // || |||| | | +--------------------------- x^68
        // || |||| | +----------------------------- x^66
        // || |||| +------------------------------- x^64
        // || ||||
        // || |||+---------------------------------- x^78
        // || ||+----------------------------------- x^77
        // || |+------------------------------------ x^76
        // || +------------------------------------- x^75
        // |+--------------------------------------- x^73
        // +---------------------------------------- x^72
        //

        // the basic idea behind how the table lookup results can be used with one
        // another is that:
        //
        // Mod(A * x^n, P(x)) = Mod(x^n * Mod(A, P(X)), P(X))
        //
        // in other words, an input data shifted towards the higher degree terms
        // changes the pre-computed crc of the input data by shifting it also
        // the same amount towards higher degree terms (mod the polynomial)

        // here is an example:
        //
        // 1) input:
        //
        // 00 00 00 00 00 00 00 00 AD DE
        //          
        // 2) index crc table for byte DE (really for dividend 00 00 00 00 00 00 00
        // 00 DE)
        //
        // we get A8B4AFBDC5A6ACA4
        //
        // 3) apply that to the input stream:
        //
        // 00 00 00 00 00 00 00 00 AD DE
        // A8 B4 AF BD C5 A6 AC A4
        // -----------------------------
        // 00 A8 B4 AF BD C5 A6 AC 09
        //
        // 4) index crc table for byte 09 (really for dividend 00 00 00 00 00 00 00
        // 00 09)
        // 
        // we get 448FCBB7FCB9E309
        //
        // 5) apply that to the input stream
        //
        // 00 A8 B4 AF BD C5 A6 AC 09
        // 44 8F CB B7 FC B9 E3 09
        // --------------------------
        // 44 27 7F 18 41 7C 45 A5
        //
        //

*******Some explanation about TimeStamp calculation

        Looking deeply in TimeStamp Specification I found it very similar to NTP Network Time Protocol (RFC 1305) TimeStam TimeStampp
        but starting  date 00:00:00 January 1, 1970, UTC (instead of 1900)
http://www.faqs.org/rfcs/rfc2030.html
        So I made a code Inspired from www.pps.jussieu.fr/~balat/Timestamp.java which implements spec from rfc2030
        Jussieu is a very good French mathematics university, so it was difficult for me no to trust it...
        I adapt it to use the my bytesArray class to encode values and to be able to put it in the Header object to send.

*******Namic Sandbox :

I have asked and got a passwd to Namic to create a sub-directory to host the project. :
http://www.na-mic.org/Wiki/index.php/Engineering:Subversion_Repository
I need now to comit this project to the SandBox.

