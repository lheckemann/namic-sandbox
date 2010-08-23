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

import java.util.ArrayList;
import java.util.Iterator;

import org.medcare.igtl.util.BytesArray;
import org.medcare.igtl.util.Header;

/**
 *** This class create an Capability objet from bytes received or help to generate
 * bytes to send from it
 * 
 * @author Andre Charles Legendre
 * 
 */
public class CapabilityMessage extends OpenIGTMessage {

        public static int IGTL_CAPABILITY_TYPE_LENGTH = 12;

        private byte[] capability_data;
        ArrayList<String> capabilityList = new ArrayList<String>();

        /**
         *** Constructor to be used to create message to send them with this
         * constructor you must use method SetImageHeader, then CreateBody and then
         * getBytes to send them
         *** 
         * @param deviceName
         *            Device Name
         **/
        public CapabilityMessage(String deviceName) {
                super(deviceName);
        }

        /**
         *** Constructor to be used to create message from received data
         * 
         * @param header
         * @param body
         * @throws Exception 
         */
        public CapabilityMessage(Header header, byte body[]) throws Exception {
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
                capability_data = new byte[bodyLength];
                System.arraycopy(body, 0, capability_data, 0, bodyLength);
                SetCapabilityData(capability_data);
                return true;
        }

        /**
         *** To create body from image_header and image_data
         *  SetCapabilityData must have called first
         * 
         *** 
         * @return the bytes array containing the body
         */
        public byte[] PackBody() {
                body = new byte[Header.LENGTH + capability_data.length];
                System.arraycopy(capability_data, 0, body, 0, capability_data.length);
                header = new Header(VERSION, "CAPABILITY", deviceName, body);
                return body;
        }

        /**
         *** To create capability_data from capabilityList and to get the byte array to send
         * @param capabilityList : Capability ArrayList 
         *                        this ArrayList can be created by MessageHandler or by ResponseHandler
         *** 
         * @return the bytes array created from the value
         */
        public byte[] SetCapabilityData(ArrayList<String> capabilityList) {
                this.capabilityList = capabilityList;
                bytesArray = new BytesArray();
                Iterator<String> it = capabilityList.iterator();
                while (it.hasNext()) {
                        bytesArray.putString((String) it.next());
                }
                capability_data = bytesArray.getBytes();
                return capability_data;
        }

        /**
         *** To extract capabilityList capability_data byte array
         * @param capability_data
         */
        public void SetCapabilityData(byte capability_data[]) {
                this.capability_data = capability_data;
                bytesArray = new BytesArray();
                bytesArray.putBytes(capability_data);
                for (int len = capability_data.length; len >= IGTL_CAPABILITY_TYPE_LENGTH; len -= IGTL_CAPABILITY_TYPE_LENGTH) {
                        this.capabilityList.add(bytesArray.getString(IGTL_CAPABILITY_TYPE_LENGTH)); // char 12
                }
        }

        /**
         *** To get capability_data byte array
         *** 
         * @return the capability_data bytes array
         */
        public byte[] GetCapabilityData() {
                return this.capability_data;
        }

        /**
         *** To set Capability ArrayList this ArrayList can be created by MessageHandler or by ResponseHandler
         * @param capabilityList
         *** 
         */
        void SetCapabilityList(ArrayList<String> capabilityList) {
                this.capabilityList = capabilityList;
        }

        /**
         *** To get capabilityList ArrayList
         *** 
         * @return the capabilityList
         */
        public ArrayList<String> GetCapabilityList() {
                return capabilityList;
        }
}

