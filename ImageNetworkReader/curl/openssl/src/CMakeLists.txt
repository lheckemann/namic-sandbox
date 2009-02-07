PROJECT(OpenSSL)

IF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)
  SET(THIRTY_TWO_BIT 1)
ENDIF(${CMAKE_SIZEOF_VOID_P} EQUAL 4)

IF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
  SET(SIXTY_FOUR_BIT 1)
ENDIF(${CMAKE_SIZEOF_VOID_P} EQUAL 8)

CONFIGURE_FILE(${OpenSSL_SOURCE_DIR}/opensslconf.h.in
  ${OpenSSL_BINARY_DIR}/../opensslconf.h)

INCLUDE_DIRECTORIES(${OpenSSL_BINARY_DIR}/../)
INCLUDE_DIRECTORIES(${OpenSSL_BINARY_DIR}/../)
INCLUDE_DIRECTORIES(${OpenSSL_BINARY_DIR}/../../)
INCLUDE_DIRECTORIES(${OpenSSL_BINARY_DIR}/openssl)

AUX_SOURCE_DIRECTORY(. OpenSSL_SRCS)

INCLUDE_DIRECTORIES(
${OpenSSL_SOURCE_DIR}/crypto
${OpenSSL_SOURCE_DIR}/../
${OpenSSL_SOURCE_DIR}/../../
)

AUX_SOURCE_DIRECTORY(crypto/evp Crypto_SRCS)
AUX_SOURCE_DIRECTORY(crypto Crypto_evp_SRCS)
AUX_SOURCE_DIRECTORY(crypto/x509 Crypto_x509_SRCS)
AUX_SOURCE_DIRECTORY(crypto/x509v3 Crypto_x509v3_SRCS)
AUX_SOURCE_DIRECTORY(crypto/stack Crypto_stack_SRCS)
AUX_SOURCE_DIRECTORY(crypto/asn1 Crypto_asn1_SRCS)
AUX_SOURCE_DIRECTORY(crypto/err Crypto_err_SRCS)
AUX_SOURCE_DIRECTORY(crypto/rand Crypto_rand_SRCS)
AUX_SOURCE_DIRECTORY(crypto/md4 Crypto_md4_SRCS)
AUX_SOURCE_DIRECTORY(crypto/md5 Crypto_md5_SRCS)
AUX_SOURCE_DIRECTORY(crypto/buffer Crypto_buffer_SRCS)
AUX_SOURCE_DIRECTORY(crypto/bio Crypto_bio_SRCS)
AUX_SOURCE_DIRECTORY(crypto/objects Crypto_objects_SRCS)
AUX_SOURCE_DIRECTORY(crypto/comp Crypto_comp_SRCS)
AUX_SOURCE_DIRECTORY(crypto/dh Crypto_dh_SRCS)
AUX_SOURCE_DIRECTORY(crypto/rsa Crypto_rsa_SRCS)
AUX_SOURCE_DIRECTORY(crypto/ec Crypto_ec_SRCS)
AUX_SOURCE_DIRECTORY(crypto/bn Crypto_bn_SRCS)
AUX_SOURCE_DIRECTORY(crypto/lhash Crypto_lhash_SRCS)
AUX_SOURCE_DIRECTORY(crypto/engine Crypto_engine_SRCS)
AUX_SOURCE_DIRECTORY(crypto/aes Crypto_aes_SRCS)
AUX_SOURCE_DIRECTORY(crypto/idea Crypto_idea_SRCS)
AUX_SOURCE_DIRECTORY(crypto/hmac Crypto_hmac_SRCS)
AUX_SOURCE_DIRECTORY(crypto/pem Crypto_pem_SRCS)
AUX_SOURCE_DIRECTORY(crypto/dsa Crypto_dsa_SRCS)
AUX_SOURCE_DIRECTORY(crypto/ecdh Crypto_ecdh_SRCS)
AUX_SOURCE_DIRECTORY(crypto/ecdsa Crypto_ecdsa_SRCS)
AUX_SOURCE_DIRECTORY(crypto/md2 Crypto_md2_SRCS)
AUX_SOURCE_DIRECTORY(crypto/rc2 Crypto_rc2_SRCS)
AUX_SOURCE_DIRECTORY(crypto/ocsp Crypto_ocsp_SRCS)
AUX_SOURCE_DIRECTORY(crypto/conf Crypto_conf_SRCS)
AUX_SOURCE_DIRECTORY(crypto/sha Crypto_sha_SRCS)
AUX_SOURCE_DIRECTORY(crypto/des Crypto_des_SRCS)
IF(WIN32)
  IF(NOT CYGWIN)
  AUX_SOURCE_DIRECTORY(crypto/pkcs7 Crypto_pkcs7_SRCS)
  AUX_SOURCE_DIRECTORY(crypto/pkcs12 Crypto_pkcs12_SRCS)
  ENDIF(NOT CYGWIN)
ENDIF(WIN32)
AUX_SOURCE_DIRECTORY(crypto/ui Crypto_ui_SRCS)
AUX_SOURCE_DIRECTORY(crypto/dso Crypto_dso_SRCS)

  AUX_SOURCE_DIRECTORY(crypto/pkcs7 Crypto_pkcs7_SRCS)
  AUX_SOURCE_DIRECTORY(crypto/pkcs12 Crypto_pkcs12_SRCS)
  AUX_SOURCE_DIRECTORY(crypto/rc4 Crypto_rc4_SRCS)


ADD_LIBRARY(OpenSSL 
${OpenSSL_SRCS}
${Crypto_SRCS}
${Crypto_evp_SRCS}
${Crypto_x509_SRCS}
${Crypto_x509v3_SRCS}
${Crypto_stack_SRCS}
${Crypto_asn1_SRCS}
${Crypto_err_SRCS}
${Crypto_rand_SRCS}
${Crypto_md4_SRCS}
${Crypto_md5_SRCS}
${Crypto_buffer_SRCS}
${Crypto_bio_SRCS}
${Crypto_objects_SRCS}
${Crypto_ocsp_SRCS}
${Crypto_comp_SRCS}
${Crypto_des_SRCS}
${Crypto_dh_SRCS}
${Crypto_rsa_SRCS}
${Crypto_ec_SRCS}
${Crypto_bn_SRCS}
${Crypto_lhash_SRCS}
${Crypto_engine_SRCS}
${Crypto_aes_SRCS}
${Crypto_idea_SRCS}
${Crypto_hmac_SRCS}
${Crypto_pem_SRCS}
${Crypto_sha_SRCS}
${Crypto_dsa_SRCS}
${Crypto_ecdh_SRCS}
${Crypto_ecdsa_SRCS}
${Crypto_md2_SRCS}
${Crypto_rc2_SRCS}
${Crypto_rc4_SRCS}
${Crypto_conf_SRCS}
${Crypto_pkcs7_SRCS}
${Crypto_pkcs12_SRCS}
${Crypto_ui_SRCS}
${Crypto_dso_SRCS}
)

