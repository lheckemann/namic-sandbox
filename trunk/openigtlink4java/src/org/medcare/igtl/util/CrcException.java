package org.medcare.igtl.util;

public class CrcException extends RuntimeException {
        /**
         * 
         */
        private static final long serialVersionUID = 8854925645976405597L;

        /**
         * Constructs a FormatException with no detail message.
         */
        public CrcException() {
                super();
        }

        /**
         * Constructs a FormatException with the specified detail message
         *
         * @param s the detail message
         */
        public CrcException(String s) {
                super(s);
        }
}

