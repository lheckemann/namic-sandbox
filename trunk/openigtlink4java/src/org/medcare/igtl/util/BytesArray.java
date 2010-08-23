/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/util/BytesArray.java $
  Language:  java
  Date:      $Date: 2010-08-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.util;

import java.nio.charset.Charset;

/**
 *** For reading/writing binary fields
 * 
 * @author Andre Charles Legendre
 **/

public class BytesArray {
        public static final long MAX_UINT = 4294967295L; // 2^32 - 1
        // ------------------------------------------------------------------------

        private byte bytesArray[] = null;
        private int size = 0;
        private int index = 0;
        Charset charset = Charset.forName("US-ASCII");
        // ------------------------------------------------------------------------

        private static final long[] CRC_TABLE;
        private static final long crc64Polynomial = 0xC96C5795D7870F42L;
        static {
                CRC_TABLE = new long[0x100];
                for (int i = 0; i < 0x100; i++) {
                        long v = i;
                        for (int j = 0; j < 8; j++) {
                                // is current coefficient set?
                                if ((v & 1) == 1) {
                                        // yes, then assume it gets zero'd (by implied x^64
                                        // coefficient of dividend)
                                        // and add rest of the divisor
                                        v = (v >>> 1) ^ crc64Polynomial;
                                } else {
                                        // no? then move to next coefficient
                                        v = (v >>> 1);
                                }
                        }
                        System.out.println("0x" + Long.toHexString(v));
                        CRC_TABLE[i] = v;
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Destination Constructor
         *** 
         * 
         **/
        public BytesArray() {
                this.bytesArray = new byte[0];
                this.size = 0; // no 'size' yet
                this.index = 0; // start at index '0' for writing
        }

        // ------------------------------------------------------------------------

        /**
         *** Write an array of bytes to the bytesArray
         *** 
         * @param n
         *            The bytes to write to the bytesArray
         *** @param nOfs
         *            The offset into <code>n</code> to start reading from
         *** @param nLen
         *            The number of bytes to write from <code>n</code>
         *** @return The number of bytes writen
         **/
        public int putBytes(byte n[], int nOfs, int nLen) {
                /* check for nothing to write */
                if (nLen > 0 && nOfs >= 0 && n != null && (n.length - nOfs >= nLen)) {
                        try {
                                enlarge(nLen);
                                System.arraycopy(n, nOfs, bytesArray, this.size, nLen);
                                this.size = bytesArray.length;
                                return nLen;
                        } catch (Exception e) {
                                e.printStackTrace();
                        }
                }
                // nothing to write
                return 0;
        }

        /**
         *** Write an array of bytes to the bytesArray
         *** 
         * @param n
         *            The bytes to write to the bytesArray
         *** @param nLen
         *            The number of bytes to write from <code>n</code>
         *** @return The number of bytes writen
         **/
        public int putBytes(byte n[], int nLen) {
                return this.putBytes(n, this.index, nLen);
        }

        /**
         *** Write an array of bytes to the bytesArray
         *** 
         * @param n
         *            The bytes to write to the bytesArray
         *** @return The number of bytes writen
         **/
        public int putBytes(byte n[]) {
                return this.putBytes(n, this.index, n.length);
        }

        // ------------------------------------------------------------------------

        /**
         *** For an output/write bytesArray, returns the number of bytes written. For
         * an input/read bytesArray, return the total number of bytes contained in
         * this bytesArray.
         *** 
         * @return The current size of the bytesArray
         **/
        public int getSize() {
                return this.size;
        }

        // ------------------------------------------------------------------------

        /**
         *** Gets the current read/write index return@ The index
         **/
        public int getIndex() {
                return this.index;
        }

        /**
         *** Resets the read/write index to the specified value
         *** 
         * @param ndx
         *            The value to set the index
         **/
        public void setIndex(int ndx) {
                this.index = (ndx <= 0) ? 0 : ndx;
        }

        // ------------------------------------------------------------------------

        /**
         *** Return a byte array representing the data
         * 
         *** 
         * @return A copy of the current bytesArray (as-is)
         **/
        public byte[] getBytes() {
                // return the full copy bytesArray (regardless of the state of
                // 'this.index')
                byte n[] = new byte[this.size];
                System.arraycopy(bytesArray, 0, n, 0, this.size);
                return n;
        }

        // ------------------------------------------------------------------------

        /**
         *** Read <code>length</code< of bytes from the bytesArray
         *** 
         * @param length
         *            The number fo bytes to read from the bytesArray
         *** 
         * @return A copy of the current bytesArray or byte[0]
         **/
        public byte[] getBytes(int length) {
                // This will read 'length' bytes, or the remaining bytes, whichever is
                // less
                int maxLen = ((length >= 0) && ((this.index + length) <= this.size)) ? length
                                : (this.size - this.index);
                if (maxLen <= 0) {
                        // no room left
                        return new byte[0];
                } else {
                        byte n[] = new byte[maxLen];
                        System.arraycopy(this.bytesArray, this.index, n, 0, maxLen);
                        this.index += maxLen;
                        return n;
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Read a <code>long</code> value from bytesArray
         *** 
         * @param length
         *            The number of bytes to decode the value from
         *** @return The decoded long value, or 0L
         **/
        public long getLong(int length) {
                int maxLen = ((this.index + length) <= this.size) ? length
                                : (this.size - this.index);
                if (maxLen <= 0) {
                        // nothing to read
                        return 0L;
                } else {
                        byte n[] = getBytes(maxLen);
                        long val = decodeLong(n, 0, true);
                        return val;
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Read an unsigned <code>long</code> value from bytesArray
         *** 
         * @param length
         *            The number of bytes to decode the value from
         *** @return The decoded long value, or 0L
         **/
        public long getULong(int length) {
                int maxLen = ((this.index + length) <= this.size) ? length
                                : (this.size - this.index);
                if (maxLen <= 0) {
                        // nothing to read
                        return 0L;
                } else {
                        byte n[] = getBytes(maxLen);
                        long val = decodeLong(n, 0, false);
                        return val;
                }
        }

        /**
         *** Read a <code>double</code> value from bytesArray, using IEEE 754 format
         *** 
         * @param length
         *            The number of bytes from which the value is decoded
         *** @return The decoded double value, or 0L
         **/
        public double getDouble(int length) {
                // 'length' must be at least 4
                int maxLen = ((this.index + length) <= this.size) ? length
                                : (this.size - this.index);
                if (maxLen <= 0) {
                        // nothing to read
                        return 0.0;
                } else {
                        byte n[] = getBytes(maxLen);
                        double val = decodeDouble(n, 0);
                        return val;
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Read a string from the bytesArray. The string is read until (whichever
         * comes first):
         *** <ol>
         * <li><code>length</code> bytes have been read</li>
         *** <li>a null (0x00) byte is found (if <code>varLength==true</code>)</li>
         *** <li>end of data is reached</li>
         * </ol>
         *** 
         * @param length
         *            The maximum length to read
         *** @param varLength
         *            Boolean telling if the string can be variable in length (stop
         *            on a null)
         *** @return The read String
         **/
        public String getString(int length, boolean varLength) {
                // Read until (whichever comes first):
                // 1) length bytes have been read
                // 2) a null (0x00) byte is found (if 'varLength==true')
                // 3) end of data is reached
                int m;
                if (length > this.size)
                        length = this.size;
                if (varLength) {
                        // look for the end-of-data, or a terminating null (0x00)
                        for (m = 0; m < length && (this.bytesArray[this.index + m] != 0); m++)
                                ;
                } else {
                        // look for end of data only
                        m = length;
                }
                return getString(m);
        }

        /**
         *** Converts the byte array to a String based on the US-ASCII character set.
         *** 
         *** @param len
         *            The number of bytes to convert to a String
         *** @return The String representation of the specified byte array
         **/
        public String getString(int len) {
                if (bytesArray == null) {
                        return null; // what goes around ...
                } else if (len <= 0) {
                        return ""; // empty length
                } else {
                        int ofs = this.index;
                        this.index += len;
                        try {
                                // Convert bytes to a string.
                                return new String(bytesArray, ofs, len, charset);
                        } catch (Throwable t) {
                                // This should NEVER occur (at least not because of the charset)
                                return "";
                        }
                }
        }

        /**
         *** Add a <code>long</code> value to the bytesArray
         *** 
         * @param val
         *            The value to write
         *** @param wrtLen
         *            The number of bytes to write the value into
         *** @return The number of bytes written
         **/
        public int putLong(long val, int wrtLen) {
                /* check for nothing to write */
                if (wrtLen <= 0)
                        return 0;
                /* write long */
                byte n[] = encodeLong(wrtLen, val, true);
                return putBytes(n);
        }

        /**
         *** Add a unsigned <code>long</code> value to the bytesArray. Same as
         * writeLong()
         *** 
         * @param val
         *            The value to write
         *** @param wrtLen
         *            The number of bytes to write the value into
         *** @return The number of bytes written
         **/
        public int putULong(long val, int wrtLen) {
                /* check for nothing to write */
                if (wrtLen <= 0)
                        return 0;
                /* write long */
                byte n[] = encodeLong(wrtLen, val, false);
                return putBytes(n);
        }

        /**
         *** Calculate Add a <code>timestamp</code> value to the bytesArray
         *** 
         *** @return The long value of timeStamp
         **/
        public long putTimeStamp() {
                byte n[] = encodeTimeStamp(System.currentTimeMillis());
                putBytes(n);
                return decodeTimeStamp(n);
        }

        /**
         *** Add a <code>timestamp</code> value to the bytesArray
         *** 
         *** @param t
         *            float timestamp <code>n</code>
         *** @return The number of bytes written
         **/
        public int putTimeStamp(float t) {
                long seconds = (long) t;
                long fraction = (long) (t * (float) 0x100000000L);
                byte n[] = encodeTimeStamp(seconds, fraction);
                return putBytes(n);
        }

        /**
         *** Calculate and Add a <code>crc</code> value to the bytesArray
         *** 
         * @param buf
         *            The bytes to callculate the crc from
         *** @param len
         *            The number of bytes to calaculate from
         *** @param crc
         *            actual crc value <code>n</code>
         *** @return The long value of crc
         **/
        public long putCrc(byte[] buf, int len, long crc) {
                crc = crc64(buf, len, crc);
                putULong(crc, 8);
                return crc;
        }

        /**
         *** Add a <code>double</code> value to the bytesArray
         *** 
         * @param val
         *            The value to write
         *** @param wrtLen
         *            The number of bytes to write the value into
         *** @return The number of bytes written
         **/
        public int putDouble(double val, int wrtLen) {
                // 'wrtLen' should be either 4 or 8

                /* check for nothing to write */
                if (wrtLen <= 0) {
                        // nothing to write
                        return 0;
                }
                /* write float/double */
                if (wrtLen < 4) {
                        // not enough bytes to encode float/double
                        return 0;
                }
                /* write float/double */
                if (wrtLen < 8) {
                        // 4 <= wrtLen < 8 [float]
                        byte n[] = encodeDouble(4, val);
                        return putBytes(n);
                } else {
                        // 8 <= wrtLen [double]
                        byte n[] = encodeDouble(8, val);
                        return putBytes(n);
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Add a string to the bytesArray. Writes until either <code>wrtLen</code>
         *** bytes are written or the string terminates
         *** 
         * @param s
         *            The string to write
         *** @return The number of bytes written
         **/
        public int putString(String s) {
                /* empty string ('maxLen' is at least 1) */
                if ((s == null) || s.equals("")) {
                        byte n[] = new byte[1];
                        n[0] = (byte) 0; // string terminator
                        return putBytes(n);
                }
                /* write string bytes, and adjust pointers */
                try {
                        return putBytes(s.getBytes(charset));
                } catch (Exception e) {
                        // TODO Auto-generated catch block
                        e.printStackTrace();
                }
                return -1;
        }

        // ------------------------------------------------------------------------

        /**
         *** Encodes a <code>long</code> value into bytes
         *** 
         *** @param len
         *            The number of bytes to encode the value to
         *** @param val
         *            The value to encode
         *** @param signed
         *            true if signed false if unsigned
         *** 
         * @return the bytes array created from the value
         **/
        public byte[] encodeLong(int len, long val, boolean signed) {
                double signMax = Math.pow(2, (len * 8) - 1);
                if (len < 0) {
                        System.out.println("len < 0");
                        return new byte[0];
                }
                if (signed && val > signMax) {
                        System.out.println("signed && val>signMax " + signMax);
                        return new byte[0];
                }
                if (signed && val < -signMax) {
                        System.out.println("signed && val<-signMax -" + signMax);
                        return new byte[0];
                }
                if (!signed && val < 0) {
                        System.out.println("!signed && val<0");
                        return new byte[0];
                }
                if (!signed && val > 2 * signMax) {
                        System.out.println("!signed && val>2*signMax " + 2 * signMax);
                        return new byte[0];
                }

                byte data[] = new byte[len];
                long n = val;
                for (int i = (len - 1); i >= 0; i--) {
                        data[i] = (byte) (n & 0xFF);
                        n >>>= 8;
                }
                return data;
        }

        // ------------------------------------------------------------------------

        /**
         *** Decodes a <code>long</code> value from bytes
         *** 
         * @param data
         *            The byte array to decode the value from
         *** @param ofs
         *            The offset into <code>data</code> to start decoding from
         *** @param signed
         *            If the encoded bytes represent a signed value
         *** @return The decoded value, or 0L
         **/
        public long decodeLong(byte data[], int ofs, boolean signed) {
                if (data != null) {
                        int len = data.length - ofs;
                        long n = (signed && ((data[ofs] & 0x80) != 0)) ? -1L : 0L;
                        for (int i = ofs; i < ofs + len; i++) {
                                n = (n << 8) | ((long) data[i] & 0xFF);
                        }
                        return n;
                } else {
                        return 0L;
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Encodes a <code>double</code> value into bytes
         *** 
         *** @param len
         *            The number of bytes to encode the value to
         *** @param val
         *            The value to encode
         *** 
         * @return the bytes array created from the value
         **/
        public byte[] encodeDouble(int len, double val) {
                // 'len' must be at least 4
                if (len >= 4) {
                        byte data[] = new byte[len];
                        int flen = (len >= 8) ? 8 : 4;
                        long n = (flen == 8) ? Double.doubleToRawLongBits(val)
                                        : (long) Float.floatToRawIntBits((float) val);
                        // Big-Endian order
                        for (int i = (flen - 1); i >= 0; i--) {
                                data[i] = (byte) (n & 0xFF);
                                n >>>= 8;
                        }
                        return data;
                } else {
                        return new byte[0];
                }
        }

        // ------------------------------------------------------------------------

        /**
         *** Decodes a <code>double</code> value from bytes, using IEEE 754 format
         *** 
         * @param data
         *            The byte array from which to decode the <code>double</code>
         *            value
         *** @param ofs
         *            The offset into <code>data</code> to start decoding
         *** @return The decoded value, or 0L
         **/
        public double decodeDouble(byte data[], int ofs) {
                // 'len' must be at lest 4
                if ((data != null) && (data.length >= 4)) {
                        int len = data.length - ofs;
                        int flen = (len >= 8) ? 8 : 4;
                        long n = 0L;
                        // Big-Endian order
                        // { 0x01, 0x02, 0x03, 0x04 } -> 0x01020304
                        for (int i = ofs; i < ofs + flen; i++) {
                                n = (n << 8) | ((long) data[i] & 0xFF);
                        }
                        if (flen == 8) {
                                return Double.longBitsToDouble(n);
                        } else {
                                return (double) Float.intBitsToFloat((int) n);
                        }
                } else {
                        return 0.0;
                }
        }

        /**
         * Enlarge this byte vector so that it can receive n more bytes.
         * 
         * @param size
         *            number of additional bytes that this byte vector should be
         *            able to receive.
         */
        private void enlarge(final int size) {
                byte[] newData = new byte[bytesArray.length + size];
                System.arraycopy(bytesArray, 0, newData, 0, bytesArray.length);
                bytesArray = newData;
        }

        // Inspire from www.pps.jussieu.fr/~balat/Timestamp.java spec from rfc2030
        // but starting
        // date 00:00:00 January 1, 1970, UTC (instead of 1900)
        /**
         * Encodes a time in millisec in a 8 bytes array
         *** 
         * @param t
         *            the time in millis from 00:00:00 January 1, 1970, UTC value
         *** @return The byte array containing the timestamp
         */
        public byte[] encodeTimeStamp(long t) {
                long s, f; // s = seconds part, f = fraction part
                s = (t / 1000L);
                byte array[] = new byte[8];
                array[0] = (byte) (s >> 24);
                array[1] = (byte) ((s >> 16) & 0xFF);
                array[2] = (byte) ((s >> 8) & 0xFF);
                array[3] = (byte) (s & 0xFF);
                f = (t % 1000L) * 1000L * 4295; // 4295 = approximation de 10^-6*2^32
                array[4] = (byte) (f >> 24);
                array[5] = (byte) ((f >> 16) & 0xFF);
                array[6] = (byte) ((f >> 8) & 0xFF);
                array[7] = (byte) (f & 0xFF);
                return array;
        }

        // Inspire from www.pps.jussieu.fr/~balat/Timestamp.java spec from rfc2030
        // but starting
        // date 00:00:00 January 1, 1970, UTC (instead of 1900)
        /**
         * Encodes a time in millisec in a 8 bytes array
         *** 
         * @param s
         *            the seconds from time in millis from 00:00:00 January 1, 1970,
         *            UTC value
         * @param f
         *            the fraction of time in millis from 00:00:00 January 1, 1970,
         *            UTC value
         *** @return The byte array containing the timestamp
         */
        public byte[] encodeTimeStamp(long s, long f) {
                // s = seconds part, f = fraction part
                byte array[] = new byte[8];
                array[0] = (byte) (s >> 24);
                array[1] = (byte) ((s >> 16) & 0xFF);
                array[2] = (byte) ((s >> 8) & 0xFF);
                array[3] = (byte) (s & 0xFF);
                array[4] = (byte) (f >> 24);
                array[5] = (byte) ((f >> 16) & 0xFF);
                array[6] = (byte) ((f >> 8) & 0xFF);
                array[7] = (byte) (f & 0xFF);
                return array;
        }

        /**
         * Will read a 8 bytes array and return it as a millisec timestamp.
         *** 
         * @param array
         *            the array to decode TimeStamp from
         * 
         *** @return The long timestamp value
         */
        public long decodeTimeStamp(byte[] array) {
                long seconds = decodeLong(array, 0, false);
                long fraction = decodeLong(array, 4, false);
                long ms = (long) ((double) fraction / (double) 1000L / (double) 4295 + 0.5);
                return seconds * 1000L + ms;
        }

        // ------------------------------------------------------------------------

        /**
         * Converts an unsigned byte to a short. By default, Java assumes that a
         * byte is signed.
         * 
         * @param b
         *            the byte to get short from
         * 
         *** @return The short value
         */
        public short unsignedByteToShort(byte b) {
                if ((b & 0x80) == 0x80)
                        return (short) (128 + (b & 0x7f));
                else
                        return (short) b;
        }

        /**
         * Calculates CRC from a byte buffer based on the polynom specified in
         * ECMA-182.
         *
         *** Calculate a <code>crc</code> value Adds from a partial byte array to the
         * data checksum.
         *** 
         * @param buffer
         *            The bytes to callculate the crc from
         *** @param len
         *            The number of bytes to calaculate from
         *** @param crc
         *            actual crc value <code>n</code>
         *** @return The long crc64 value
         **/
        public long crc64(byte[] buffer, int len, long crc) {
                // System.out.println("crc len " + len);
                for (int i = 0; i < len; i++) {
                        crc = next_crc(crc, buffer[i]);
                        // System.out.println("crc 0x" + Long.toHexString(crc));
                }
                return crc;
        }

        /**
         *** Calculate a <code>crc</code> value Adds from a complete byte array to the
         * data checksum.
         *** 
         * @param buf
         *            The bytes to callculate the crc from
         *** @param crc
         *            actual crc value <code>n</code>
         *** @return The long crc64 value
         **/
        public long crc64(byte[] buf, long crc) {
                return crc64(buf, buf.length, crc);
        }

        /**
         * Calculates the next crc value.
         *** 
         *** @param crc
         *            actual crc value <code>n</code>
         *** @param ch
         *            The next byte
         *** @return The long crc64 value
         **/
        public long next_crc(long crc, byte ch) {
                return (crc >>> 8) ^ CRC_TABLE[((int) crc ^ ch) & 0xff];
        }
}

