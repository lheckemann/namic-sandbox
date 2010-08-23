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

/**
 *** This class create an Position objet from bytes received or help to generate
 * bytes to send from it
 * 
 * @author Andre Charles Legendre
 * 
 */
public class PositionMessage extends OpenIGTMessage {

        public static int IGTL_POSITION_MESSAGE_DEFAULT_SIZE = 28;
        public static int IGTL_POSITION_MESSAGE_POSITON_ONLY_SIZE = 12;
        public static int IGTL_POSITION_MESSAGE_WITH_QUATERNION3_SIZE = 24;

        public static int POSITION_ONLY = 0;
        public static int WITH_QUATERNION3 = 3;
        public static int ALL = 4;
        private int quaternionSize = ALL;

        double position[] = new double[3]; // float 32bits
        double quaternion[] = new double[4]; // float 32bits
        private byte[] position_data;
        private int bodyLength;

        /**
         *** Constructor to be used to create message to send them with this
         * constructor you must use method SetImageHeader, then CreateBody and then
         * getBytes to send them
         *** 
         * @param deviceName
         *            Device Name
         **/
        public PositionMessage(String deviceName) {
                super(deviceName);
        }

        /**
         *** Constructor to be used to create message from received data
         * 
         * @param header
         * @param body
         * @throws Exception 
         */
        public PositionMessage(Header header, byte body[]) throws Exception {
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
                position_data = new byte[bodyLength];
                System.arraycopy(body, 0, position_data, 0, bodyLength);
                SetPositionData(position_data);
                return true;
        }

        /**
         *** To create body from image_header and image_data
         *  SetPositionData must have called first
         * 
         *** 
         * @return the bytes array containing the body
         */
        public byte[] PackBody() {
                body = new byte[Header.LENGTH + position_data.length];
                System.arraycopy(position_data, 0, body, 0, position_data.length);
                header = new Header(VERSION, "POSITION", deviceName, body);
                return body;
        }

        /**
         *** To create position_data from image carateristics and to get the byte array to send
         * @param position
         * @param quaternion
         * @param quaternionSize
         *** 
         * @return the bytes array created from the value
         */
        public byte[] SetPositionData(double position[], double quaternion[], int quaternionSize) {
                bytesArray = new BytesArray();
                SetPosition(position);
                bytesArray.putDouble(position[0], 4);
                bytesArray.putDouble(position[1], 4);
                bytesArray.putDouble(position[2], 4);
                this.quaternionSize = quaternionSize;
                SetQuaternion(quaternion);
                for (int i = 0; i < quaternionSize; i++)
                                bytesArray.putDouble(quaternion[i], 4);
                position_data = bytesArray.getBytes();
                return position_data;
        }

        /**
         *** To extract image caracteristics from position_data byte array
         * @param position_data
         */
        public void SetPositionData(byte position_data[]) {
                this.position_data = position_data;
                bytesArray = new BytesArray();
                bytesArray.putBytes(position_data);
                position[0] = bytesArray.getDouble(4); // float32
                position[1] = bytesArray.getDouble(4); // float32
                position[2] = bytesArray.getDouble(4); // float32
                SetPosition(position);
                if (bodyLength == IGTL_POSITION_MESSAGE_DEFAULT_SIZE) {
                        quaternionSize = POSITION_ONLY;
                } else if (bodyLength == IGTL_POSITION_MESSAGE_POSITON_ONLY_SIZE) {
                        quaternionSize = WITH_QUATERNION3;
                } else if (bodyLength == IGTL_POSITION_MESSAGE_WITH_QUATERNION3_SIZE) {
                        quaternionSize = ALL;
                }
                for (int l = 0; l <= 3; l++)
                        quaternion[l] = 0.0; // float32
                for (int i = 0; i < quaternionSize; i++)
                        quaternion[i] = bytesArray.getDouble(4); // float32
                SetQuaternion(quaternion);
        }

        /**
         *** To get position_data byte array
         *** 
         * @return the position_data bytes array
         */
        public byte[] GetPositionData() {
                return this.position_data;
        }

        /**
         *** To set Image position
         * @param position
         *** 
         */
        public void SetPosition(double position[]) {
                this.position = position;
        }

        /**
         *** To set Image position
         * @param x
         * @param y
         * @param z
         *** 
         */
        public void SetPosition(double x, double y, double z) {
                this.position[0] = x;
                this.position[1] = y;
                this.position[2] = z;
        }

        /**
         *** To get Image position
         *** 
         * @return the position bytes array
         */
        public double[] GetPosition() {
                return this.position;
        }

        /**
         *** To set Image quaternion
         * @param quaternion
         *** 
         */
        void SetQuaternion(double quaternion[]) {
                this.quaternion = quaternion;
        }

        /**
         *** To set Image quaternion
         * @param ox
         * @param oy
         * @param oz
         * @param w
         *** 
         */
        void SetQuaternion(double ox, double oy, double oz, double w) {
                quaternion[0] = ox;
                quaternion[1] = oy;
                quaternion[2] = oz;
                quaternion[3] = w;
        }

        /**
         *** To get Image quaternion
         *** 
         * @return the quaternion array
         */
        public double[] GetQuaternion() {
                return quaternion;
        }
}

