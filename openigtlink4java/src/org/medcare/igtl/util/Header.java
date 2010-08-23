/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/util/header.java $
  Language:  java
  Date:      $Date: 2010-08-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.util;

/**
 *** For reading/writing OpenIGTLink headers
 * 
 * @author Andre Charles Legendre
 **/

public class Header {
        public static int LENGTH = 58;
        int base = 1000000000; /* 10^9 */
        long version; // unsigned int 16bits
        String dataType; // char 12 bits
        String deviceName; // char 20 bits
        long timestamp; // unsigned int 64 bits
        long body_size; // unsigned int 64 bits
        long crc; // unsigned int 64 bits
        BytesArray bytesArray;

        // ------------------------------------------------------------------------

        /**
         *** Destination Constructor
         *** 
         * @param version
         *            ; // Version number unsigned int 16bits
         * @param _type
         *            ; // Type name of data char 12 bits
         * @param _name
         *            ; // Unique device name char 20 bits
         * @param timestamp
         *            ; // TimeStamp or 0 if unused unsigned int 64 bits
         * @param body_size
         *            ; // Size of body in bytes unsigned int 64 bits
         * @param crc
         *            ; // 64 bit CRC for body data unsigned int 64 bits
         **/
        public Header(long version, String _type, String _name, long timestamp,
                        long body_size, long crc) {
                bytesArray = new BytesArray();
                this.version = version;
                bytesArray.putLong(version, 2);
                byte typeArray[] = new byte[12];
                for (int m = 0; m < 12; m++) {
                        typeArray[m] = 0;
                }
                dataType = new String(typeArray, bytesArray.charset);
                int len = _type.length();
                len = (len <= 12) ? len : 12;
                System.arraycopy(typeArray, 0, _type.toCharArray(), 0, len);
                bytesArray.putString(new String(typeArray));
                byte nameArray[] = new byte[20];
                for (int m = 0; m < 20; m++) {
                        nameArray[m] = 0;
                }
                len = _name.length();
                len = (len <= 20) ? len : 20;
                System.arraycopy(nameArray, 0, _name.toCharArray(), 0, len);
                bytesArray.putString(new String(nameArray));
                this.timestamp = timestamp;
                bytesArray.putTimeStamp(timestamp);
                this.body_size = body_size;
                bytesArray.putLong(body_size, 8);
                this.crc = crc;
                bytesArray.putLong(crc, 8);
        }

        // ------------------------------------------------------------------------

        /**
         *** Destination Constructor
         *** 
         * @param version
         *            ; // Version number unsigned int 16bits
         * @param dataType
         *            ; // Type name of data char 12 bits
         * @param deviceName
         *            ; // Unique device name char 20 bits
         * @param body
         *            ; // body in bytes
         **/
        public Header(long version, String dataType, String deviceName,
                        byte body[]) {
                bytesArray = new BytesArray();
                this.version = version;
                bytesArray.putULong(version, 2);
                byte typeArray[] = new byte[12];
                for (int m = 0; m < 12; m++) {
                        typeArray[m] = 0;
                }
                dataType = new String(typeArray, bytesArray.charset);
                int len = dataType.length();
                len = (len <= 12) ? len : 12;
                System.arraycopy(typeArray, 0, dataType.toCharArray(), 0, len);
                bytesArray.putString(new String(typeArray));
                byte nameArray[] = new byte[20];
                for (int m = 0; m < 20; m++) {
                        nameArray[m] = 0;
                }
                len = deviceName.length();
                len = (len <= 20) ? len : 20;
                System.arraycopy(nameArray, 0, deviceName.toCharArray(), 0, len);
                bytesArray.putString(new String(nameArray));
                this.timestamp = bytesArray.putTimeStamp();
                this.body_size = body.length;
                bytesArray.putULong(body_size, 8);
                this.crc = bytesArray.putCrc(body, body.length, 0L);
        }

        // ------------------------------------------------------------------------

        /**
         *** Destination Constructor
         *** 
         * @param bytes
         *            bytes doit contenir 58 bytes of the header
         **/
        public Header(byte bytes[]) {
                bytesArray = new BytesArray();
                bytesArray.putBytes(bytes);
                version = bytesArray.getLong(2); // unsigned int 16bits
                dataType = bytesArray.getString(12); // char 12 bits
                deviceName = bytesArray.getString(20); // char 20 bits
                timestamp = bytesArray.decodeTimeStamp(bytesArray.getBytes(8));
                body_size = bytesArray.getLong(8); // unsigned int 64 bits
                crc = bytesArray.getLong(8); // unsigned int 64 bits
        }

        // ------------------------------------------------------------------------

        /**
         *** Version number.
         *** 
         * @return The current version of the bytesArray
         **/
        public long getVersion() {
                return this.version;
        }

        // ------------------------------------------------------------------------

        /**
         *** Type name of data
         *** 
         * @return The type of the device
         **/
        public String getDataType() {
                return this.dataType;
        }

        // ------------------------------------------------------------------------

        /**
         *** Unique device name.
         *** 
         * @return The current name of the device
         **/
        public String getDeviceName() {
                return this.deviceName;
        }

        // ------------------------------------------------------------------------

        /**
         *** TimeStamp or 0 if unused.
         *** 
         * @return The timestamp at the creation of the header
         **/
        public double getTimeStamp() {
                return this.timestamp;
        }

        // ------------------------------------------------------------------------

        /**
         *** Size of body in bytes.
         *** 
         * @return The current body_size of the bytesArray
         **/
        public long getBody_size() {
                return this.body_size;
        }

        // ------------------------------------------------------------------------

        /**
         *** 64 bit CRC for body data.
         *** 
         * @return The current crc of the bytesArray
         **/
        public long getCrc() {
                return this.crc;
        }

        // ------------------------------------------------------------------------

        /**
         *** ByteArray which contain header bytes.
         *** 
         * @return The current bytesArray
         **/
        public BytesArray getBytesArray() {
                return this.bytesArray;
        }

        // ------------------------------------------------------------------------

        /**
         *** this header bytes.
         *** 
         * @return A copy of the byte array currently in the bytesArray (as-is)
         **/
        public byte[] getBytes() {
                return this.bytesArray.getBytes();
        }
}

