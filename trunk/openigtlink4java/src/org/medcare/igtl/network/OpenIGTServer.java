/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/network/SocketServer.java $
  Language:  java
  Date:      $Date: 2010-08-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.network;

import java.io.IOException;
import java.net.ServerSocket;

import javax.net.ServerSocketFactory;

import org.medcare.igtl.util.Status;

/**
 * <p>
 * The class can be used to create a server listening a port Messages received
 * will be queued and proceed
 * <p>
 * 
 * @author <a href="mailto:andleg@osfe.org">Andre Charles Legendre </a>
 * @version 0.1a (09/06/2010)
 * 
 */

public class OpenIGTServer {
        private Status status;

        /***************************************************************************
         * Default MessageQueueManager constructor.
         * 
         * @param port
         *            port on which this server will be bind
         * 
         **************************************************************************/
        public OpenIGTServer(int port) throws IOException {
                ServerSocket socket = null;
                boolean listening = true;
                ServerSocketFactory serverSocketFactory = ServerSocketFactory
                                .getDefault();

                try {
                        socket = serverSocketFactory.createServerSocket(port);
                } catch (IOException e) {
                        System.err.println("Could not listen on port: " + port);
                        System.exit(-1);
                }

                while (listening)
                        new ServerThread(socket.accept(), this).start();

                socket.close();
        }

        /**
         *** To set server status
         * @param status
         *** 
         */
        public void setStatus(Status status) {
                this.status = status;
        }

        /**
         *** To get server status
         *** 
         * @return the status status
         */
        public Status getStatus() {
                return this.status;
        }
}

