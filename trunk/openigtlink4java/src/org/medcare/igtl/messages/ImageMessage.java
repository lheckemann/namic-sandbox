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
 *** This class create an Image objet from bytes received or help to generate
 * bytes to send from it
 * 
 * @author Andre Charles Legendre
 * 
 */
public class ImageMessage extends OpenIGTMessage {

        public static int IGTL_IMAGE_HEADER_SIZE = 72;
        public static int IGTL_IMAGE_HEADER_VERSION = 1;

        public static int COORDINATE_LPS = 0;
        public static int COORDINATE_RAS = 1;

        public static int ENDIAN_BIG = 0;
        public static int ENDIAN_LITTLE = 1;

        public static int DTYPE_SCALAR = 1;
        public static int DTYPE_VECTOR = 2;

        public static final int TYPE_INT8 = 2;
        public static final int TYPE_UINT8 = 3;
        public static final int TYPE_INT16 = 4;
        public static final int TYPE_UINT16 = 5;
        public static final int TYPE_INT32 = 6;
        public static final int TYPE_UINT32 = 7;
        public static final int TYPE_FLOAT32 = 10;
        public static final int TYPE_FLOAT64 = 11;

        long version = IGTL_IMAGE_HEADER_VERSION; // V unsigned int 16bits version
        long imageType = DTYPE_SCALAR; // T unsingned int 8bits image_type image_type
        // image_ype
        long scalarType = TYPE_UINT8; // S unsingned int 8bits scalarType
        // scalarType
        long endian = ENDIAN_BIG; // E unsingned int 8bits endian_type endian endian
        long coordinate_type = COORDINATE_RAS; // O unsingned int 8bits
        // coordinate_type coord coordinate
        long dimensions[] = new long[3]; // unsingned int 16bits
        // RI_pixels unsingned int 16bits size[0] dimensions[0]
        // RJ_pixels unsingned int 16bits size[1] dimensions[1]
        // RK_pixels unsingned int 16bits size[2] dimensions[2]
        double matrix[][] = new double[4][4];
        double origin[] = new double[3]; // float 32bits
        // PX_pixels float 32bits origin[0] (matrix[0][3])
        // PY_pixels float 32bits origin[1] (matrix[1][3])
        // PZ_pixels float 32bits origin[2] (matrix[2][3])
        double normals[][] = new double[3][3];
        double norm_i[] = new double[3]; // float 32bits
        // TX_pixels float 32bits norm_i[0] (matrix[0][0])
        // TY_pixels float 32bits norm_i[1] (matrix[1][0])
        // TZ_pixels float 32bits norm_i[2] (matrix[2][0])
        double norm_j[] = new double[3]; // float 32bits
        // SX_pixels float 32bits norm_j[0] (matrix[0][1])
        // SY_pixels float 32bits norm_j[1] (matrix[1][1])
        // SZ_pixels float 32bits norm_j[2] (matrix[2][1])
        double norm_k[] = new double[3]; // float 32bits
        // NX_pixels // float 32bits norm_k[0] (matrix[0][2])
        // NY_pixels // float 32bits norm_k[1] (matrix[1][2])
        // NZ_pixels // float 32bits norm_k[2] (matrix[2][2])
        long subOffset[] = new long[3]; // unsingned int 16bits
        // DI_pixels unsingned int 16bits subvol_offset subOffset[0]
        // DJ_pixels unsingned int 16bits subvol_offset subOffset[1]
        // DK_pixels unsingned int 16bits subvol_offset subOffset[2]
        long subDimensions[] = new long[3]; // unsingned int 16bits
        // DRI_pixels unsingned int 16bits subvol_size subDimensions[0]
        // DRJ_pixels unsingned int 16bits subvol_size subDimensions[1]
        // DRK_pixels unsingned int 16bits subvol_size subDimensions[2]
        // spacing, igtl_image_get_matrix() calculates spacing based on the
        // transformation matrix
        double spacing[] = new double[3]; // float 32bits
        private byte[] image_data;
        private byte[] image_header;

        /**
         *** Constructor to be used to create message to send them with this
         * constructor you must use method SetImageHeader, then CreateBody and then
         * getBytes to send them
         *** 
         * @param deviceName
         *            Device Name
         **/
        public ImageMessage(String deviceName) {
                super(deviceName);
        }

        /**
         *** Constructor to be used to create message from received data
         * 
         * @param header
         * @param body
         * @throws Exception 
         */
        public ImageMessage(Header header, byte body[]) throws Exception {
                super(header, body);
        }

        /**
         *** To create body from body array
         * 
         *** 
         * @return true if inpacking is ok
         */
        public boolean UnpackBody() {
                image_header = new byte[IGTL_IMAGE_HEADER_SIZE];
                System.arraycopy(body, 0, image_header, 0, IGTL_IMAGE_HEADER_SIZE);
                SetImageHeader(image_header);
                return true;
        }

        /**
         *** To create body from image_header and image_data
         *  SetImageHeader and SetImageData must have called first
         * 
         *** 
         * @return the bytes array containing the body
         */
        public byte[] PackBody() {
                body = new byte[Header.LENGTH + image_header.length + image_data.length];
                System.arraycopy(image_header, 0, body, 0, image_header.length);
                System.arraycopy(image_data, 0, body, image_header.length,
                                image_data.length);
                header = new Header(VERSION, "IMAGE", deviceName, body);
                return body;
        }

        /**
         *** To create image_header from image carateristics and to get the byte array to send
         * @param version
         * @param imageType
         * @param scalarType
         * @param endian
         * @param coordinate_type
         * @param dimensions
         * @param origin
         * @param normals
         * @param subOffset
         * @param subDimensions
         *** 
         * @return the bytes array created from the value
         */
        public byte[] SetImageHeader(long version, long imageType,
                        long scalarType, long endian, long coordinate_type,
                        long dimensions[], double origin[], double normals[][],
                        long subOffset[], long subDimensions[]) {
                bytesArray = new BytesArray();
                this.version = version;
                bytesArray.putULong(version, 2);
                this.imageType = imageType;
                bytesArray.putULong(imageType, 1);
                this.scalarType = scalarType;
                bytesArray.putULong(scalarType, 1);
                this.endian = endian;
                bytesArray.putULong(endian, 1);
                this.coordinate_type = coordinate_type;
                bytesArray.putULong(coordinate_type, 1);
                this.dimensions = dimensions;
                bytesArray.putULong(dimensions[0], 2);
                bytesArray.putULong(dimensions[1], 2);
                bytesArray.putULong(dimensions[2], 2);
                SetOrigin(origin);
                bytesArray.putDouble(origin[0], 4);
                bytesArray.putDouble(origin[1], 4);
                bytesArray.putDouble(origin[2], 4);
                SetNormals(normals);
                for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                                bytesArray.putDouble(normals[i][j], 4);
                SetMatrix(origin, normals);
                if (SetSubVolume(subDimensions, subOffset)) {
                        bytesArray.putULong(subOffset[0], 2);
                        bytesArray.putULong(subOffset[1], 2);
                        bytesArray.putULong(subOffset[2], 2);
                        bytesArray.putULong(subDimensions[0], 2);
                        bytesArray.putULong(subDimensions[1], 2);
                        bytesArray.putULong(subDimensions[2], 2);
                        image_header = bytesArray.getBytes();
                } else {
                        image_header = new byte[0];
                }
                return image_header;
        }

        /**
         *** To create image_header from image carateristics and to get the byte array to send
         * @param dimensions
         * @param origin
         * @param normals
         * @param subOffset
         * @param subDimensions
         *** 
         * @return the bytes array created from the value
         */
        public byte[] SetImageHeader(long dimensions[], double origin[],
                        double normals[][], long subOffset[], long subDimensions[]) {
                return SetImageHeader(this.version, this.imageType, this.scalarType,
                                this.endian, this.coordinate_type, dimensions, origin, normals,
                                subOffset, subDimensions);
        }

        /**
         *** To extract image caracteristics from image_header byte array
         * @param image_header
         */
        public void SetImageHeader(byte image_header[]) {
                this.image_header = image_header;
                bytesArray = new BytesArray();
                bytesArray.putBytes(image_header);
                version = bytesArray.getLong(2); // unsigned Short
                imageType = bytesArray.getLong(1); // unsigned int8
                scalarType = bytesArray.getLong(1); // unsigned int8
                endian = bytesArray.getLong(1); // unsigned int8
                coordinate_type = bytesArray.getLong(1); // unsigned int8
                dimensions[0] = bytesArray.getLong(2); // unsigned int16
                dimensions[1] = bytesArray.getLong(2); // unsigned int16
                dimensions[2] = bytesArray.getLong(2); // unsigned int16

                origin[0] = bytesArray.getDouble(4); // float32
                origin[1] = bytesArray.getDouble(4); // float32
                origin[2] = bytesArray.getDouble(4); // float32
                SetOrigin(origin);
                for (int i = 0; i < 3; i++)
                        for (int j = 0; j < 3; j++)
                                normals[i][j] = bytesArray.getDouble(4); // float32
                SetNormals(normals);
                SetMatrix(origin, normals);
                subOffset[0] = bytesArray.getLong(2); // unsigned int16
                subOffset[1] = bytesArray.getLong(2); // unsigned int16
                subOffset[2] = bytesArray.getLong(2); // unsigned int16
                subDimensions[0] = bytesArray.getLong(2); // unsigned int16
                subDimensions[1] = bytesArray.getLong(2); // unsigned int16
                subDimensions[2] = bytesArray.getLong(2); // unsigned int16
                //We don't test the result, subvolume is supposed to have been tested during the byte array creation
                SetSubVolume(subDimensions, subOffset);
        }

        /**
         *** To get image_header byte array
         *** 
         * @return the image_header bytes array
         */
        public byte[] GetImageHeader() {
                return this.image_header;
        }

        /**
         *** To get image_data byte array
         *** 
         * @return the image_data bytes array
         */
        public byte[] GetImageData() {
                return this.image_data;
        }

        /**
         *** To Set image_data byte array
         *** 
         * @param image_data the image_data bytes array
         */
        public void SetImageData(byte image_data[]) {
                this.image_data = image_data;
        }

        /**
         *** To set Image dimensions
         * @param dimensions
         *** 
         */
        public void SetDimensions(long dimensions[]) {
                this.dimensions = dimensions;
        }

        /**
         *** To set Image dimensions
         * @param i
         * @param j
         * @param k
         *** 
         */
        public void SetDimensions(long i, long j, long k) {
                this.dimensions[0] = i;
                this.dimensions[1] = j;
                this.dimensions[2] = k;
        }

        /**
         *** To get Image dimensions
         *** 
         * @return the dimensions bytes array
         */
        public long[] GetDimensions() {
                return this.dimensions;
        }

        /**
         *** To set Image dimensions
         * @param subDimensions
         * @param subOffset
         *** 
         * @return true if Ok false if not
         */
        public boolean SetSubVolume(long subDimensions[], long subOffset[]) {
                // make sure that sub-volume fits in the dimensions
                if (subOffset[0] + subDimensions[0] <= dimensions[0]
                                && subOffset[1] + subDimensions[1] <= dimensions[1]
                                && subOffset[2] + subDimensions[2] <= dimensions[2]) {
                        this.subDimensions = subDimensions;
                        this.subOffset = subOffset;
                        return true;
                } else {
                        return false;
                }
        }

        /**
         *** To set Image dimensions
         * @param dimi
         * @param dimj
         * @param dimk
         * @param offi
         * @param offj
         * @param offk
         *** 
         * @return true if Ok false if not
         */
        public boolean SetSubVolume(int dimi, int dimj, int dimk, int offi, int offj,
                        int offk) {
                // make sure that sub-volume fits in the dimensions
                if (offi + dimi <= dimensions[0] && offj + dimj <= dimensions[1]
                                && offk + dimk <= dimensions[2]) {
                        subDimensions[0] = dimi;
                        subDimensions[1] = dimj;
                        subDimensions[2] = dimk;
                        subOffset[0] = offi;
                        subOffset[1] = offj;
                        subOffset[2] = offk;
                        return true;
                } else {
                        return false;
                }
        }

        /**
         *** To get SubVolume dimensions
         *** 
         * @return the SubVolume dimensions bytes array
         */
        public long[] GetSubDimensions() {
                return subDimensions;
        }

        /**
         *** To get SubVolume offset
         *** 
         * @return the SubVolume offset bytes array
         */
        public long[] GetSubOffset() {
                return subOffset;
        }

        /**
         *** To set Image spacing
         * @param spacing
         *** 
         */
        public void SetSpacing(double spacing[]) {
                this.spacing = spacing;
        }

        /**
         *** To set Image spacing
         * @param si
         * @param sj
         * @param sk
         *** 
         */
        public void SetSpacing(float si, float sj, float sk) {
                spacing[0] = si;
                spacing[1] = sj;
                spacing[2] = sk;
        }

        /**
         *** To get Image spacing
         *** 
         * @return the spacing bytes array
         */
        public double[] GetSpacing() {
                return spacing;
        }

        /**
         *** To set Image origin
         * @param origin
         *** 
         */
        public void SetOrigin(double origin[]) {
                this.origin = origin;
        }

        /**
         *** To set Image origin
         * @param px
         * @param py
         * @param pz
         *** 
         */
        public void SetOrigin(double px, double py, double pz) {
                this.origin[0] = px;
                this.origin[1] = py;
                this.origin[2] = pz;
        }

        /**
         *** To get Image origin
         *** 
         * @return the origin bytes array
         */
        public double[] GetOrigin() {
                return this.origin;
        }

        /**
         *** To set Image normals
         * @param normals
         *** 
         */
        void SetNormals(double normals[][]) {
                this.normals = normals;
        }

        /**
         *** To set Image normals
         * @param t array
         * @param s array
         * @param n array
         *** 
         */
        void SetNormals(double t[], double s[], double n[]) {
                normals[0][0] = t[0];
                normals[1][0] = t[1];
                normals[2][0] = t[2];
                normals[0][1] = s[0];
                normals[1][1] = s[1];
                normals[2][1] = s[2];
                normals[0][2] = n[0];
                normals[1][2] = n[1];
                normals[2][2] = n[2];
        }

        /**
         *** To get Image normals
         *** 
         * @return the normals matrix
         */
        public double[][] GetNormals() {
                return normals;
        }

        /**
         *** To set Image matrix
         * @param spacing array
         * @param origin array
         * @param norm_i array
         * @param norm_j array
         * @param norm_k array
         *** 
         */
        public void SetMatrix(double spacing[], double origin[],
                            double norm_i[], double norm_j[], double norm_k[]) {
                matrix[0][0] = norm_i[0] * spacing[0];
                matrix[1][0] = norm_i[1] * spacing[0];
                matrix[2][0] = norm_i[2] * spacing[0];
                matrix[0][1] = norm_j[0] * spacing[1];
                matrix[1][1] = norm_j[1] * spacing[1];
                matrix[2][1] = norm_j[2] * spacing[1];
                matrix[0][2] = norm_k[0] * spacing[2];
                matrix[1][2] = norm_k[1] * spacing[2];
                matrix[2][2] = norm_k[2] * spacing[2];
                matrix[0][3] = origin[0];
                matrix[1][3] = origin[1];
                matrix[2][3] = origin[2];
        }

        /**
         *** To set Image matrix
         * @param origin array
         * @param normals matrix
         *** 
         */
        public void SetMatrix(double origin[], double normals[][]) {
                matrix[0][0] = normals[0][0];
                matrix[1][0] = normals[1][0];
                matrix[2][0] = normals[2][0];
                matrix[0][1] = normals[0][1];
                matrix[1][1] = normals[1][1];
                matrix[2][1] = normals[2][1];
                matrix[0][2] = normals[0][2];
                matrix[1][2] = normals[1][2];
                matrix[2][2] = normals[2][2];
                matrix[0][3] = origin[0];
                matrix[1][3] = origin[1];
                matrix[2][3] = origin[2];
                matrix[3][0] = 0.0;
                matrix[3][1] = 0.0;
                matrix[3][2] = 0.0;
                matrix[3][3] = 1.0;
                SetMatrix(matrix);
        }

        /**
         *** To set Image matrix
         * @param matrix
         *** 
         */
        public void SetMatrix(double matrix[][]) {
                this.matrix = matrix;
                double tx = matrix[0][0];
                double ty = matrix[1][0];
                double tz = matrix[2][0];
                double sx = matrix[0][1];
                double sy = matrix[1][1];
                double sz = matrix[2][1];
                double nx = matrix[0][2];
                double ny = matrix[1][2];
                double nz = matrix[2][2];

                spacing[0] = Math.sqrt(tx*tx + ty*ty + tz*tz);
                spacing[1] = Math.sqrt(sx*sx + sy*sy + sz*sz);
                spacing[2] = Math.sqrt(nx*nx + ny*ny + nz*nz);
                norm_i[0] = matrix[0][0]  / spacing[0];
                norm_i[1] = matrix[1][0]  / spacing[0];
                norm_i[2] = matrix[2][0]  / spacing[0];
                norm_j[0] = matrix[1][1] / spacing[1];
                norm_j[1] = matrix[2][1] / spacing[1];
                norm_j[2] = matrix[3][1] / spacing[1];
                norm_k[0] = matrix[0][2] / spacing[2];
                norm_k[1] = matrix[1][2] / spacing[2];
                norm_k[2] = matrix[2][2] / spacing[2];
                origin[0] = matrix[0][3];
                origin[1] = matrix[1][3];
                origin[2] = matrix[2][3];
        }

        /**
         *** To get Image matrix
         *** 
         * @return the image matrix
         */
        public double[][] GetMatrix() {
                return matrix;
        }

        /**
         *** To set Image scalar type
         * @param scalarType
         *** 
         */
        public void SetScalarType(long scalarType) {
                this.scalarType = scalarType;
        }

        /**
         *** To set Image scalar type to Int8
         *** 
         */
        public void SetScalarTypeToInt8() {
                scalarType = TYPE_INT8;
        }

        /**
         *** To set Image scalar type to Uint8
         *** 
         */
        public void SetScalarTypeToUint8() {
                scalarType = TYPE_UINT8;
        }

        /**
         *** To set Image scalar type to Int16
         *** 
         */
        public void SetScalarTypeToInt16() {
                scalarType = TYPE_INT16;
        }

        /**
         *** To set Image scalar type to Uint16
         *** 
         */
        public void SetScalarTypeToUint16() {
                scalarType = TYPE_UINT16;
        }

        /**
         *** To set Image scalar type to Int32
         *** 
         */
        public void SetScalarTypeToInt32() {
                scalarType = TYPE_INT32;
        }

        /**
         *** To set Image scalar type to Uint32
         *** 
         */
        public void SetScalarTypeToUint32() {
                scalarType = TYPE_UINT32;
        }

        /**
         *** To get Image scalar type
         *** 
         * @return the scalar type
         */
        public long GetScalarType() {
                return scalarType;
        }

        /**
         *** To set Image image type
         * @param imageType
         *** 
         */
        public void SetImageType(long imageType) {
                this.imageType = imageType;
        }

        /**
         *** To get Image image type
         *** 
         * @return the image type
         */
        public long GetImageType() {
                return imageType;
        }

        /**
         *** To set Image Endian of image scalar (default is ENDIAN_BIG)
         * @param endian
         *** 
         */
        public void SetEndian(long endian) {
                this.endian = endian;
        }

        /**
         *** To get Image Endian of image scalar
         *** 
         * @return the endian
         */
        public long GetEndian() {
                return endian;
        }

        /**
         *** To get Image Size
         *** 
         * @return the Size
         */
        public long GetImageSize() {
                return dimensions[0] * dimensions[1] * dimensions[2] * GetScalarSize();
        }

        /**
         *** To get SubVolume Size
         *** 
         * @return Subvolume Size
         */
        public long GetSubVolumeImageSize() {
                return subDimensions[0] * subDimensions[1] * subDimensions[2]
                                * GetScalarSize();
        }

        /**
         *** To get size of image scalar
         *** 
         * @return the scalar size
         */
        private long GetScalarSize() {
                switch ((int)scalarType) {
                case TYPE_INT8:
                        return 1;
                case TYPE_UINT8:
                        return 1;
                case TYPE_INT16:
                        return 2;
                case TYPE_UINT16:
                        return 2;
                case TYPE_INT32:
                        return 4;
                case TYPE_UINT32:
                        return 4;
                case TYPE_FLOAT32:
                        return 4;
                case TYPE_FLOAT64:
                        return 8;
                }
                return 0;
        }
}

