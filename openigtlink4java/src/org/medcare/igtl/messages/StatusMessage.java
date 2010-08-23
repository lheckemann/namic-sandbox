/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/messages/ImageMessage.java $
  Language:  java
  Date:      $Date: 2010-18-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.messages;

import org.medcare.igtl.util.BytesArray;
import org.medcare.igtl.util.Header;
import org.medcare.igtl.util.Status;

/**
 *** This class create an Status objet from bytes received or help to generate
 * bytes to send from it
 * 
 * @author Andre Charles Legendre
 * 
 */
public class StatusMessage extends OpenIGTMessage {

        public static int IGTL_STATUS_HEADER_SIZE = 30;

        public static int IGTL_STATUS_ERROR_NAME_LENGTH = 20;

        long code; // Unsigned short 16bit
        long subCode; // int 64bit
        String errorName; // char[12]
        String statusString; // char[ BodySize - 30 ]
        private byte[] status_data;

        private Status status;

        /**
         *** Constructor to be used to create message to send them with this
         * constructor you must use method SetImageHeader, then CreateBody and then
         * getBytes to send them
         *** 
         * @param deviceName
         *            Device Name
         **/
        public StatusMessage(String deviceName) {
                super(deviceName);
        }

        /**
         *** Constructor to be used to create message from received data
         * 
         * @param header
         * @param body
         * @throws Exception 
         */
        public StatusMessage(Header header, byte body[]) throws Exception {
                super(header, body);
        }

        /**
         *** To create body from body array
         * 
         *** 
         * @return true if inpacking is ok
         */
        public boolean UnpackBody() {
                int bodyLength = body.length;
                status_data = new byte[bodyLength];
                System.arraycopy(body, 0, status_data, 0, bodyLength);
                SetStatusData(status_data);
                return true;
        }

        /**
         *** To create body from image_header and image_data
         *  SetStatusData must have called first
         * 
         *** 
         * @return the bytes array containing the body
         */
        public byte[] PackBody() {
                body = new byte[Header.LENGTH + status_data.length];
                System.arraycopy(status_data, 0, body, 0, status_data.length);
                header = new Header(VERSION, "STATUS", deviceName, body);
                return body;
        }

        /**
         *** To create status_data from server or client status and to get the byte array to send
         * @param status
         *** 
         * @return the bytes array created from the value
         */
        public byte[] SetStatusData(Status status) {
                this.status = status;
                this.code = status.getCode();
                this.subCode = status.getSubCode();
                this.errorName = status.getErrorName();
                this.statusString = status.getStatusString();
                bytesArray = new BytesArray();
                bytesArray.putULong(code, 2);
                bytesArray.putLong(subCode, 8);
                bytesArray.putString(errorName);
                bytesArray.putString(statusString);
                status_data = bytesArray.getBytes();
                return status_data;
        }

        /**
         *** To extract image caracteristics from status_data byte array
         * @param status_data
         */
        public void SetStatusData(byte status_data[]) {
                this.status_data = status_data;
                bytesArray = new BytesArray();
                bytesArray.putBytes(status_data);
                this.code = bytesArray.getLong(2); // Unsigned short 16bits
                this.subCode = bytesArray.getLong(8); // int 64
                this.errorName = bytesArray.getString(IGTL_STATUS_ERROR_NAME_LENGTH); // char 20
                //Control status_data.length-IGTL_STATUS_HEADER_SIZE-1 == '\0' ???
                this.statusString = bytesArray.getString(status_data.length-IGTL_STATUS_HEADER_SIZE);
                this.status = new Status((int) code, (int) subCode, errorName, statusString);
        }

        /**
         *** To get status_data byte array
         *** 
         * @return the status_data bytes array
         */
        public byte[] GetStatusData() {
                return this.status_data;
        }

        /**
         *** To set client or server status
         * @param code
         *** 
         */
        public void SetCode(long code) {
                this.code = code;
        }

        /**
         *** To set client or server status
         * @param status
         *** 
         */
        public void SetStatus(Status status) {
                this.status = status;
        }

        /**
         *** To get client or server status
         *** 
         * @return the status code
         */
        public Status GetStatus() {
                return this.status;
        }

        /**
         *** To set Image subCode
         * @param subCode
         *** 
         */
        void SetSubCode(long subCode) {
                this.subCode = subCode;
        }

        /**
         *** To get Image subCode
         *** 
         * @return the subCode array
         */
        public long GetSubCode() {
                return subCode;
        }

        /**
         *** To set Error Name
         * @param errorName
         *** 
         */
        void SetErrorName(String errorName) {
                this.errorName = errorName;
        }

        /**
         *** To get errorName subCode
         *** 
         * @return the errorName
         */
        public String GetErrorName() {
                return errorName;
        }

        /**
         *** To set Status String
         * @param statusString
         *** 
         */
        void SetStatusString(String statusString) {
                this.statusString = statusString;
        }

        /**
         *** To get statusString subCode
         *** 
         * @return the statusString
         */
        public String GetStatusString() {
                return statusString;
        }
}

