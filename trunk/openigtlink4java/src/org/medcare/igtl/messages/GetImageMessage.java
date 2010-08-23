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

import org.medcare.igtl.util.Header;

/**
 *** This class create a GetImage objet from bytes received or help to generate
 * bytes to send from it
 * 
 * @author Andre Charles Legendre
 * 
 */
public class GetImageMessage extends OpenIGTMessage {

        /**
         *** Constructor to be used to create message to send them with this
         * constructor you must use method CreateBody and then
         * getBytes to send them
         *** 
         * @param deviceName
         *            Device Name
         **/
        public GetImageMessage(String deviceName) {
                super(deviceName);
        }

        /**
         *** Constructor to be used to create message from received data
         * 
         * @param header
         * @param body
         * @throws Exception 
         */
        public GetImageMessage(Header header, byte body[]) throws Exception {
                super(header, body);
        }

        /**
         *** To create body to get bytes to send
         * 
         *** 
         * @return the bytes array containing the body
         **/
        public byte[] PackBody() {
                body = new byte[0];
                header = new Header(VERSION, "GET_IMAGE", deviceName, body);
                return body;
        }

        /**
         *** To create body from body array
         * 
         *** 
         * @return true if inpacking is ok
         */
        public boolean UnpackBody() {
                return true;
        }
}

