/*
 * Main requesting class
 */
package org.medcare.igtl.network;

import java.util.ArrayList;

import org.medcare.igtl.util.Header;
import org.medcare.igtl.util.Status;

/**
 * Perform request correpondind to the message received performRequest methods
 * must be adapted corresponding to the need of each use
 * User must adapt Capability enum, to be able to send the right capability list
 * and to have a correct error management for incorrect capability request
 * 
 * @author Andre Charles Legendre
 */
public class MessageHandler {

        String err = "MessageHandler.performRequest() failed.";
        byte[] body;
        Header header;
        ServerThread serverThread;
        public enum Capability {GET_CAPABIL, GET_IMAGE, GET_IMGMETA, GET_LBMETA, GET_STATUS, GET_TRAJ, CAPABILITY, COLORTABLE, IMAGE, IMGMETA, POINT, POSITION, STATUS, STP_TDATA, STT_TDATA, TDATA, TRAJ, TRANSFORM};

        /***************************************************************************
         * Default MessageQueueManager constructor.
         * 
         * @param header
         *            of the message
         * 
         * @param body
         *            of the message
         * @param serverThread
         *            serverThread object
         * 
         **************************************************************************/
        public MessageHandler(Header header, byte[] body, ServerThread serverThread) {
                this.header = header;
                this.body = body;
                this.serverThread = serverThread;
        }

        /**
         * Perform the requestjob on the message performRequest methods must be
         * corresponding to the need of each use
         * 
         * @return True if response job performed successfull
         * @throws Exception received by perform method
         */
        public boolean performRequest() throws Exception {
                String messageType = this.header.getDataType();
                for (Capability capablity : Capability.values())
                        if (messageType.equals(capablity.toString())) {
                                return perform(messageType);
                        }
                manageError(messageType, Status.STATUS_NOT_FOUND);
                throw new AssertionError("Unknown op: " + messageType);
        }

        /**
         * Perform the requestjob  this method must be adapted for each use
         * 
         * @param messageType
         *            The messageType
         * @return True if response job performed successfull
         * @throws Exception CrcException will be thrown at crc check during message unpacking
         * Other exceptions can be thrown 
         */
        public boolean perform (String messageType) throws Exception {
                //FIXME User have to create a new message corresponding to the messageType
                return false;
        }

        /**
         * manage error this method must be adapted for each use
         * 
         * @param messageType
         *            The messageType
         * @param status
         *            The status
         * @return True if response job performed successfull
         * @throws Exception
         */
        public boolean manageError (String messageType, int status) throws Exception {
                //FIXME Will have to create an error management for each supported messageType 
                return false;
        }

        /**
         *** Gets the enum of Types implemented in this Handler
         **/
        public ArrayList<String> getCapability() {
                ArrayList<String> capabilityList = new ArrayList<String>();
                for (Capability capablity : Capability.values())
                        capabilityList.add(capablity.toString());
                return capabilityList;
        }

        /**
         *** To set server status
         * @param status
         *** 
         */
        public void setStatus(Status status) {
                this.serverThread.setStatus(status);
        }

        /**
         *** To set server status
         *** 
         * @return the status status
         */
        public Status getStatus() {
                return this.serverThread.getStatus();
        }
}

