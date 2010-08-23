/*
 * Main responseing class
 */
package org.medcare.igtl.network;

import java.util.ArrayList;

import org.medcare.igtl.util.Header;
import org.medcare.igtl.util.Status;

/**
 * Perform response data correpondind to the message received performResponse
 * methods must be adapted corresponding to the need of each use
 * User must adapt Capability enum, to be able to send the right capability list
 * and to have a correct error management for incorrect capability request
 * User must use SetStatus method so it value correspond to the real status of the client.
 * User must use GetStatus method to set 
 * 
 * @author Andre Charles Legendre
 */
public class ResponseHandler {

        String err = "ResponseHandler.performResponse() failed.";
        byte[] body;
        Header responseHeader;
        OpenIGTClient openIGTClient;
        public enum Capability {GET_CAPABIL, GET_IMAGE, GET_IMGMETA, GET_LBMETA, GET_STATUS, GET_TRAJ, CAPABILITY, COLORTABLE, IMAGE, IMGMETA, POINT, POSITION, STATUS, STP_TDATA, STT_TDATA, TDATA, TRAJ, TRANSFORM};

        /***************************************************************************
         * Default ResponseHandler constructor.
         * 
         * @param header
         *            of the response
         * 
         * @param body
         *            of the response
         * 
         * @param openIGTClient
         *            openIGTClient object
         * 
         **************************************************************************/
        public ResponseHandler(Header header, byte[] body, OpenIGTClient openIGTClient) {
                this.responseHeader = header;
                this.body = body;
                this.openIGTClient = openIGTClient;
        }

        /**
         * Perform the responsejob performResponse methods must be adapted
         * corresponding to the need of each use
         * 
         * @return True if response job performed successfull
         * @throws Exception received by perform method
         */
        public boolean performResponse() throws Exception {
                String messageType = this.responseHeader.getDataType();
                for (Capability capablity : Capability.values())
                        if (messageType.equals(capablity.toString())) {
                                return perform(messageType);
                        }
                manageError(messageType, Status.STATUS_NOT_FOUND);
                throw new AssertionError("Unknown op: " + messageType);
        }

        /**
         * Perform the responsejob  this method must be adapted for each use
         * 
         * @param messageType
         *            The messageType
         * @return True if response job performed successfull
         * @throws Exception CrcException will be thrown at crc check during message unpacking
         * Other exceptions can be thrown 
         */
        public boolean perform(String messageType) throws Exception {
                //FIXME Will have to create a new message corresponding to the messageType
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
        public boolean manageError (String messageType, int status)throws Exception  {
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
         *** To set client status
         * @param status
         *** 
         */
        public void setStatus(Status status) {
                this.openIGTClient.setStatus(status);
        }

        /**
         *** To set client status
         *** 
         * @return the status status
         */
        public Status getStatus() {
                return this.openIGTClient.getStatus();
        }
}

