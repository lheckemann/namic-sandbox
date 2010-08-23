/*=========================================================================

  Program:   OpenIGTLink Library
  Module:    $HeadURL: http://osfe.org/OpenIGTLink/Source/org/medcare/igtl/util/Tests.java $
  Language:  java
  Date:      $Date: 2010-08-14 10:37:44 +0200 (ven., 13 nov. 2009) $
  Version:   $Revision: 0ab$

  Copyright (c) Absynt Technologies Ltd. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

package org.medcare.igtl.util;

import java.math.BigInteger;

/**
 *** Utility to test util library/BytesArray crc64 and timestamp utilities and
 * differents encoding/decoding
 * 
 * @author Andre Charles Legendre
 **/

public class Tests {

        // ------------------------------------------------------------------------

        /**
         * Main method of the class. Tests of util library
         * 
         * @param args
         *            Can contains args for tests.
         */
        public static void main(String args[]) {
                BigInteger bi = new BigInteger(
                                "1100100101101100010101111001010111010111100001110000111101000010",
                                2);
                System.out
                                .println("Controle du Coefficient correspondant au polynome (should be 0xC96C5795D7870F42) : 0x"
                                                + Long.toHexString(bi.longValue()).toUpperCase());
                System.out
                                .println("Display the crc array calculated from Coefficient corresponding to ECMA-182 polynome");
                BytesArray tool = new BytesArray();
                double nombre = 15678.345 / 10000.0;
                System.out.println("Tests: double [15678.345/10000] " + nombre);
                long entier = (long) nombre;
                long reste = (long) ((nombre - entier) * 1000);
                System.out
                                .println("Tests: extract seconds and fraction from double [15678.345/10000] "
                                                + nombre + " seconds " + entier + " fraction " + reste);

                System.out.println("Tests: encodeLong [1,224,false]");
                byte[] bytes = tool.encodeLong(1, 224, false);
                System.out.println("Tests: length [" + bytes.length + "]");
                System.out.println("Tests: decodeLong ["
                                + tool.decodeLong(bytes, 0, false) + "]");
                System.out.println("Tests: encodeLong [1,224,true]");
                bytes = tool.encodeLong(1, 224, true);
                bytes = new byte[] { (byte) 0x80 };
                System.out
                                .println("CRC64 of \"x80\" (should be 0xC96C5795D7870F42) : 0x"
                                                + Long.toHexString(tool.crc64(bytes, bytes.length, 0))
                                                                .toUpperCase());

                bytes = new byte[] { (byte) 0xDE, (byte) 0xAD, (byte) 0xBE, (byte) 0xEF };
                System.out
                                .println("CRC64 of \"deadbe\" (should be FC232C18806871AF) : 0x"
                                                + Long.toHexString(tool.crc64(bytes, bytes.length, 0))
                                                                .toUpperCase());

                bytes = new byte[] { (byte) 0x99, (byte) 0xEB, (byte) 0x96,
                                (byte) 0xDD, (byte) 0x94, (byte) 0xC8, (byte) 0x8E,
                                (byte) 0x97, (byte) 0x5B, (byte) 0x58, (byte) 0x5D,
                                (byte) 0x2F, (byte) 0x28, (byte) 0x78, (byte) 0x5E, (byte) 0x36 };
                System.out
                                .println("CRC64 of \"99eb96dd94c88e975b585d2f28785e36\" (should be DB7AC38F63413C4E) : 0x"
                                                + Long.toHexString(tool.crc64(bytes, bytes.length, 0))
                                                                .toUpperCase());

                bytes = new byte[] { (byte) 0xDE, (byte) 0xAD };
                System.out
                                .println("CRC64 of \"dead\" (should be 44277F18417C45A5) : 0x"
                                                + Long.toHexString(tool.crc64(bytes, bytes.length, 0))
                                                                .toUpperCase());

                long res = tool.decodeLong(bytes, 0, true);
                System.out.println("Long res  : \t" + res);
                System.out.println("encodeTimeStamp now  : ");

                bytes = tool.encodeTimeStamp(System.currentTimeMillis());
                bi = new BigInteger(bytes);
                System.out.println("Tests: toString [" + bi.toString() + "]");
                System.out.println("Tests: longValue [" + bi.shortValue() + "]");
                System.out.println("Tests: bi.toString(2) [" + bi.toString(2) + "]");
        }
}

