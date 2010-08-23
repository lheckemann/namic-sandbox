package org.medcare.igtl.util;

public class Status {

        public static int STATUS_INVALID = 0;
        public static int STATUS_OK = 1;
        public static int STATUS_UNKNOWN_ERROR = 2;
        public static int STATUS_PANICK_MODE = 3;  /* emergency */
        public static int STATUS_NOT_FOUND = 4;  /* file, configuration, device etc */
        public static int STATUS_ACCESS_DENIED = 5;
        public static int STATUS_BUSY = 6;
        public static int STATUS_TIME_OUT = 7;  /* Time out / Connection lost */
        public static int STATUS_OVERFLOW = 8;  /* Overflow / Can't be reached */
        public static int STATUS_CHECKSUM_ERROR = 9;  /* Checksum error */
        public static int STATUS_CONFIG_ERROR = 10; /* Configuration error */
        public static int STATUS_RESOURCE_ERROR = 11; /* Not enough resource (memory, storage etc) */
        public static int STATUS_UNKNOWN_INSTRUCTION = 12; /* Illegal/Unknown instruction */
        public static int STATUS_NOT_READY = 13; /* Device not ready (starting up)*/
        public static int STATUS_MANUAL_MODE = 14; /* Manual mode (device does not accept commands) */
        public static int STATUS_DISABLED = 15; /* Device disabled */
        public static int STATUS_NOT_PRESENT = 16; /* Device not present */
        public static int STATUS_UNKNOWN_VERSION = 17; /* Device version not known */
        public static int STATUS_HARDWARE_FAILURE = 18; /* Hardware failure */
        public static int STATUS_SHUT_DOWN = 19; /* Exiting / shut down in progress */
        public static int STATUS_NUM_TYPES = 20;
        private int code;
        private int subCode;
        private String errorName;
        private String statusString;

        /**
         * Constructs a status object with default value STATUS_OK
         */
        public Status() {
                this.code = STATUS_OK;
        }

        /**
         * Constructs a a status object with the specified status code
         *
         * @param code the status code
         */
        public Status(int code) {
                this.code = code;
        }

        /**
         * Constructs a a status object with the specified status code
         *
         * @param code
         * @param subCode
         * @param errorName
         * @param statusString
         */
        public Status(int code, int subCode, String errorName, String statusString) {
                this.code = code;
                this.subCode = subCode;
                this.errorName = errorName;
                this.statusString = statusString;
        }

        /**
         *** To set client or server status code
         * @param code
         *** 
         */
        public void setCode(int code) {
                this.code = code;
        }

        /**
         *** To get client or server status code
         *** 
         * @return the status code
         */
        public int getCode() {
                return this.code;
        }

        /**
         *** To set client or server status subCode
         * @param subCode
         *** 
         */
        public void setSubCode(int subCode) {
                this.subCode = subCode;
        }

        /**
         *** To get client or server status code
         *** 
         * @return the status subCode
         */
        public int getSubCode() {
                return this.subCode;
        }

        /**
         *** To set client or server status errorName
         * @param errorName
         *** 
         */
        public void setErrorNamee(String errorName) {
                this.errorName = errorName;
        }

        /**
         *** To get client or server status code
         *** 
         * @return the status code
         */
        public String getErrorName() {
                return this.errorName;
        }

        /**
         *** To set client or server status statusString
         * @param statusString
         *** 
         */
        public void setStatusString(String statusString) {
                this.statusString = statusString;
        }

        /**
         *** To get client or server status code
         *** 
         * @return the status code
         */
        public String getStatusString() {
                return this.statusString;
        }
}

