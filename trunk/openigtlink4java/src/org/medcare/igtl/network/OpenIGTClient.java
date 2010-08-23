/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/network/SocketClient.java $
  Language:  java
  Date:      $Date: 2010-08-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.network;

import java.io.InputStream;
import java.io.OutputStream;
import java.io.IOException;
import java.net.UnknownHostException;

import javax.net.SocketFactory;

import org.medcare.igtl.util.Header;
import org.medcare.igtl.util.Status;

/**
 * <p>
 * The class can be used by first connecting to a server sending requests
 * comming from the requestQueue, listening for response and adding response to
 * the responsqueue
 * <p>
 * 
 * @author <a href="mailto:andleg@osfe.org">Andre Charles Legendre </a>
 * @version 0.1a (09/06/2010)
 * 
 */

public class OpenIGTClient extends Thread {
        private SocketFactory socketFactory;
        private java.net.Socket socket = null;
        private OutputStream outstr;
        private InputStream instr;
        private ResponseQueueManager responseQueue = null;
        private boolean alive;
        private String host;
        private int port;
        private Status status;

        /***************************************************************************
         * Default OpenIGTClient constructor.
         * 
         * @param host
         *            to be connected
         * 
         * @param port
         *            of the host
         * 
         **************************************************************************/
        public OpenIGTClient(String host, int port) {
                super("KnockKnockClient");
                this.host = host;
                this.port = port;
        }

        /***************************************************************************
         * Reader thread. Reads messages from the socket and add them to the
         * ResponseQueueManager
         * 
         **************************************************************************/
        public void run() {
                this.alive = true;
                try {
                        setSocketFactory(SocketFactory.getDefault());
                        this.socket = socketFactory.createSocket(host, port);
                        this.responseQueue = new ResponseQueueManager();
                        outstr = socket.getOutputStream();
                        instr = socket.getInputStream();
                        int ret_read = 0;
                        byte[] headerBuff = new byte[Header.LENGTH];
                        // System.out.println("JE LIS");
                        do {
                                ret_read = instr.read(headerBuff);
                                if (ret_read > 0) {
                                        // System.out.print(new String(buff, 0, ret_read));
                                        Header header = new Header(headerBuff);
                                        byte[] bodyBuf = new byte[(int) header.getBody_size()];
                                        ret_read = instr.read(bodyBuf);
                                        if (ret_read > 0) {
                                                responseQueue.addResponse(new ResponseHandler(header, bodyBuf, this));
                                        }
                                }
                        } while (alive && ret_read >= 0);
                        outstr.close();
                        instr.close();
                        socket.close();
                } catch (UnknownHostException e) {
                        System.err.println("Don't know about host" + host);
                } catch (IOException e) {
                        System.err.println("Couldn't get I/O for the connection to: "
                                        + host);
                } finally {
                        this.interrupt();
                }
        }

        public void setSocketFactory(SocketFactory socketFactory) {
                this.socketFactory = socketFactory;
        }

        /***************************************************************************
         * Sends bytes
         * <p>
         * 
         * @throws IOException
         *             - Exception in I/O.
         *             <p>
         * @param bytes
         *            - byte[] array.
         **************************************************************************/
        final public synchronized boolean sendBytes(byte[] bytes)
                        throws IOException {
                try {
                        outstr.write(bytes);
                        outstr.flush();
                } catch (Exception e) {
                        System.err.println("Exception while sendBytes to socket:"
                                        + e.getMessage());
                        return false;
                }
                return true;
        }

        /***************************************************************************
         * Interrupt this thread
         **************************************************************************/
        public void interrupt() {
                alive = false;
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
         *** To set server status
         *** 
         * @return the status status
         */
        public Status getStatus() {
                return this.status;
        }
}

