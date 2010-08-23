/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/network/SocketClientFactory.java $
  Language:  java
  Date:      $Date: 2010-08-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.network;

import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * RequestQueueManager create its own OpenIGTClient connected to one
 * OpenIGTServer. Application add Messages request to RequestQueueManager queue.
 * OpenIGTClient will send Messages to its OpenIGTServer respecting the order.
 * 
 * @author Andre Charles Legendre
 */
public class RequestQueueManager extends Thread {
        private static String VERSION = "0.1a";

        private long sleep;
        public ConcurrentLinkedQueue<byte[]> openIGT_Queue = new ConcurrentLinkedQueue<byte[]>();
        private boolean alive = true;
        private OpenIGTClient openIGTClient = null;

        /***************************************************************************
         * Default RequestQueueManager constructor.
         * 
         * @param host
         *            to be connected
         * 
         * @param port
         *            of the host
         * 
         **************************************************************************/
        public RequestQueueManager(String host, int port) {
                super("RequestQueueManager");
                this.openIGTClient = new OpenIGTClient(host, port);
        }

        /**
         * Starts the thread and process requests in queue
         */
        public void run() {
                boolean res = true;
                do {
                        try {
                                Thread.sleep(sleep); // Wait 100 milli before alive again
                                if (!openIGT_Queue.isEmpty()) {
                                        // On prefere perdre des impressions que rester coince
                                        byte[] bytes = (byte[]) openIGT_Queue.poll();
                                        if (bytes != null) {
                                                try {
                                                        res = openIGTClient.sendBytes(bytes);
                                                        if (!res)
                                                                System.out
                                                                                .println("PB openIGTClient.sendBytes ");
                                                } catch (Exception e) {
                                                        System.out.println("PB openIGTClient.sendBytes "
                                                                        + e.getLocalizedMessage());
                                                } finally {
                                                        System.out.println("OK");
                                                }
                                        } else {
                                                System.out.println("openIGTRequest null");
                                        }
                                }
                        } catch (InterruptedException e) {
                                System.out.println("PB dans Thread " + e.getLocalizedMessage());
                        }
                } while (alive && res);
        }

        /**
         * add a new request to the request queue
         * 
         * @param request
         * 
         */
        public void addRequest(byte[] request) {
                if (request != null) {
                        openIGT_Queue.add(request);
                }
        }

        /**
         * stop the thread
         * 
         */
        public void destroy() {
                alive = false;
        }

        /**
         *** Gets the current sleep time value return@ The sleep time value
         **/
        public long getSleepTime() {
                return sleep;
        }

        /**
         * Sets the time the listener thread will wait between actions
         * 
         * @param sleep
         */
        public void setSleepTime(long sleep) {
                this.sleep = sleep;
        }

        /**
         *** Gets the current version return@ The version value
         **/
        public String getVersion() {
                return VERSION;
        }
}

