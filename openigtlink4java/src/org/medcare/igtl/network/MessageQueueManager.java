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

import org.medcare.igtl.util.CrcException;

/**
 * One MessageQueueManager is created by each ServerThread to add MessageHandler
 * to MessageQueueManager queue as soon as they receive messages.
 * MessageQueueManager will perform MessageHandler respecting the order. Using
 * eventually ServerThread to send responses to its ServerThread client.
 * 
 * @author Andre Charles Legendre
 */
public class MessageQueueManager extends Thread {
        private static String VERSION = "0.1a";

        private long sleep;
        public ConcurrentLinkedQueue<MessageHandler> openIGT_Queue = new ConcurrentLinkedQueue<MessageHandler>();
        private boolean alive = true;
        ServerThread serverThread;

        /***************************************************************************
         * Default MessageQueueManager constructor.
         * 
         * @param serverThread
         *            to use to send responses to OpenIGTclient
         * 
         **************************************************************************/
        public MessageQueueManager(ServerThread serverThread) {
                super("MessageQueueManager");
                this.serverThread = serverThread;
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
                                        MessageHandler messageHandler = (MessageHandler) openIGT_Queue
                                                        .poll();
                                        if (messageHandler != null) {
                                                try {
                                                        res = messageHandler.performRequest();
                                                        if (!res)
                                                                System.out.println("PB messageHandler ");
                                                } catch (AssertionError a) {
                                                        //FIXME Add error management
                                                        System.out.println("PB messageHandler "
                                                                        + a.getLocalizedMessage());
                                                } catch (CrcException c) {
                                                        //FIXME Add error management
                                                        System.out.println("PB responseHandler ");
                                                } catch (Exception e) {
                                                        //FIXME Add error management
                                                        System.out.println("PB messageHandler "
                                                                        + e.getLocalizedMessage());
                                                } finally {
                                                        System.out.println("OK");
                                                }

                                        } else {
                                                System.out.println("messageHandler null");
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
         * @param messageHandler
         * 
         */
        public void addMessage(MessageHandler messageHandler) {
                openIGT_Queue.add(messageHandler);
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

