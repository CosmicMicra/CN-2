


# COEN 233 Programming Assignment 2


## Overview

Client using customized protocol on top of UDP protocol for requesting identification from server for access permission to the network.

## Features

   UDP-based communication.
-   Custom packet format.
-   Server checks `Verification_Database.txt`.
-   Supports 2G, 3G, 4G, 5G.
-   Implements `ack_timer` for retransmissions.

 

## Files Included

`protocol.h`: Header file with protocol definitions. <br>
`protocol.c`: Implementation of protocol functions. <br>
`client.c`: Client implementation for sending access permission requests. <br>
`server.c`: Server implementation for verifying subscriber information. <br>
`Verification_Database.txt`: Database of subscriber information. <br>
`Makefile`: Build configuration.<br>





## How to Run

1.  **Compile**
    
    ```
    make clean
    make
    
    ```
    
2.  **Run Server**
    
    ```sh
    ./server
    
    ```
    
3.  **Run Client**
    
    ```sh
    ./client
    
    ```
    
4. **Client sends subscriber number and technology to requests Access**
    
5.  **Server Verifies & Responds**
    -   Reads `Verification_Database.txt`.
    -   If the subscriber number exists:
    -   If the technology matches and payment is made (`1`), server sends **Access Granted (0xFFFB)**.
        -   If payment is not made (`0`), server sends **Not Paid (0xFFF9)**.
        -   If the technology does not match, server sends **Subscriber Not Found (0xFFFA)**. <br>
    -   If the subscriber number does not exist, server sends **Subscriber Not Found (0xFFFA)**.
    
6.  **Client Receives & Displays Response**
