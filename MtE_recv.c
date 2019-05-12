
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <unistd.h>
#include <math.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <sys/time.h>

#define DEST_MAC0	0xFF
#define DEST_MAC1	0xFF
#define DEST_MAC2	0xFF
#define DEST_MAC3	0xFF
#define DEST_MAC4	0xFF
#define DEST_MAC5	0xFF

#define ETHER_TYPE	0x88B8

#define DEFAULT_IF	"eno1"  /* your system interface name */
#define BUF_SIZ		1024



unsigned char s[256] = 
 {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
 };

 unsigned char inv_s[256] = 
 {
    0x52, 0x09, 0x6A, 0xD5, 0x30, 0x36, 0xA5, 0x38, 0xBF, 0x40, 0xA3, 0x9E, 0x81, 0xF3, 0xD7, 0xFB,
    0x7C, 0xE3, 0x39, 0x82, 0x9B, 0x2F, 0xFF, 0x87, 0x34, 0x8E, 0x43, 0x44, 0xC4, 0xDE, 0xE9, 0xCB,
    0x54, 0x7B, 0x94, 0x32, 0xA6, 0xC2, 0x23, 0x3D, 0xEE, 0x4C, 0x95, 0x0B, 0x42, 0xFA, 0xC3, 0x4E,
    0x08, 0x2E, 0xA1, 0x66, 0x28, 0xD9, 0x24, 0xB2, 0x76, 0x5B, 0xA2, 0x49, 0x6D, 0x8B, 0xD1, 0x25,
    0x72, 0xF8, 0xF6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xD4, 0xA4, 0x5C, 0xCC, 0x5D, 0x65, 0xB6, 0x92,
    0x6C, 0x70, 0x48, 0x50, 0xFD, 0xED, 0xB9, 0xDA, 0x5E, 0x15, 0x46, 0x57, 0xA7, 0x8D, 0x9D, 0x84,
    0x90, 0xD8, 0xAB, 0x00, 0x8C, 0xBC, 0xD3, 0x0A, 0xF7, 0xE4, 0x58, 0x05, 0xB8, 0xB3, 0x45, 0x06,
    0xD0, 0x2C, 0x1E, 0x8F, 0xCA, 0x3F, 0x0F, 0x02, 0xC1, 0xAF, 0xBD, 0x03, 0x01, 0x13, 0x8A, 0x6B,
    0x3A, 0x91, 0x11, 0x41, 0x4F, 0x67, 0xDC, 0xEA, 0x97, 0xF2, 0xCF, 0xCE, 0xF0, 0xB4, 0xE6, 0x73,
    0x96, 0xAC, 0x74, 0x22, 0xE7, 0xAD, 0x35, 0x85, 0xE2, 0xF9, 0x37, 0xE8, 0x1C, 0x75, 0xDF, 0x6E,
    0x47, 0xF1, 0x1A, 0x71, 0x1D, 0x29, 0xC5, 0x89, 0x6F, 0xB7, 0x62, 0x0E, 0xAA, 0x18, 0xBE, 0x1B,
    0xFC, 0x56, 0x3E, 0x4B, 0xC6, 0xD2, 0x79, 0x20, 0x9A, 0xDB, 0xC0, 0xFE, 0x78, 0xCD, 0x5A, 0xF4,
    0x1F, 0xDD, 0xA8, 0x33, 0x88, 0x07, 0xC7, 0x31, 0xB1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xEC, 0x5F,
    0x60, 0x51, 0x7F, 0xA9, 0x19, 0xB5, 0x4A, 0x0D, 0x2D, 0xE5, 0x7A, 0x9F, 0x93, 0xC9, 0x9C, 0xEF,
    0xA0, 0xE0, 0x3B, 0x4D, 0xAE, 0x2A, 0xF5, 0xB0, 0xC8, 0xEB, 0xBB, 0x3C, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2B, 0x04, 0x7E, 0xBA, 0x77, 0xD6, 0x26, 0xE1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0C, 0x7D
 };

unsigned char mul2[] =
{
    0x00,0x02,0x04,0x06,0x08,0x0a,0x0c,0x0e,0x10,0x12,0x14,0x16,0x18,0x1a,0x1c,0x1e,
    0x20,0x22,0x24,0x26,0x28,0x2a,0x2c,0x2e,0x30,0x32,0x34,0x36,0x38,0x3a,0x3c,0x3e,
    0x40,0x42,0x44,0x46,0x48,0x4a,0x4c,0x4e,0x50,0x52,0x54,0x56,0x58,0x5a,0x5c,0x5e,
    0x60,0x62,0x64,0x66,0x68,0x6a,0x6c,0x6e,0x70,0x72,0x74,0x76,0x78,0x7a,0x7c,0x7e,
    0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0x8e,0x90,0x92,0x94,0x96,0x98,0x9a,0x9c,0x9e,
    0xa0,0xa2,0xa4,0xa6,0xa8,0xaa,0xac,0xae,0xb0,0xb2,0xb4,0xb6,0xb8,0xba,0xbc,0xbe,
    0xc0,0xc2,0xc4,0xc6,0xc8,0xca,0xcc,0xce,0xd0,0xd2,0xd4,0xd6,0xd8,0xda,0xdc,0xde,
    0xe0,0xe2,0xe4,0xe6,0xe8,0xea,0xec,0xee,0xf0,0xf2,0xf4,0xf6,0xf8,0xfa,0xfc,0xfe,
    0x1b,0x19,0x1f,0x1d,0x13,0x11,0x17,0x15,0x0b,0x09,0x0f,0x0d,0x03,0x01,0x07,0x05,
    0x3b,0x39,0x3f,0x3d,0x33,0x31,0x37,0x35,0x2b,0x29,0x2f,0x2d,0x23,0x21,0x27,0x25,
    0x5b,0x59,0x5f,0x5d,0x53,0x51,0x57,0x55,0x4b,0x49,0x4f,0x4d,0x43,0x41,0x47,0x45,
    0x7b,0x79,0x7f,0x7d,0x73,0x71,0x77,0x75,0x6b,0x69,0x6f,0x6d,0x63,0x61,0x67,0x65,
    0x9b,0x99,0x9f,0x9d,0x93,0x91,0x97,0x95,0x8b,0x89,0x8f,0x8d,0x83,0x81,0x87,0x85,
    0xbb,0xb9,0xbf,0xbd,0xb3,0xb1,0xb7,0xb5,0xab,0xa9,0xaf,0xad,0xa3,0xa1,0xa7,0xa5,
    0xdb,0xd9,0xdf,0xdd,0xd3,0xd1,0xd7,0xd5,0xcb,0xc9,0xcf,0xcd,0xc3,0xc1,0xc7,0xc5,
    0xfb,0xf9,0xff,0xfd,0xf3,0xf1,0xf7,0xf5,0xeb,0xe9,0xef,0xed,0xe3,0xe1,0xe7,0xe5
};

unsigned char mul_3[] = 
{ 
    0x00,0x03,0x06,0x05,0x0c,0x0f,0x0a,0x09,0x18,0x1b,0x1e,0x1d,0x14,0x17,0x12,0x11,
    0x30,0x33,0x36,0x35,0x3c,0x3f,0x3a,0x39,0x28,0x2b,0x2e,0x2d,0x24,0x27,0x22,0x21,
    0x60,0x63,0x66,0x65,0x6c,0x6f,0x6a,0x69,0x78,0x7b,0x7e,0x7d,0x74,0x77,0x72,0x71,
    0x50,0x53,0x56,0x55,0x5c,0x5f,0x5a,0x59,0x48,0x4b,0x4e,0x4d,0x44,0x47,0x42,0x41,
    0xc0,0xc3,0xc6,0xc5,0xcc,0xcf,0xca,0xc9,0xd8,0xdb,0xde,0xdd,0xd4,0xd7,0xd2,0xd1,
    0xf0,0xf3,0xf6,0xf5,0xfc,0xff,0xfa,0xf9,0xe8,0xeb,0xee,0xed,0xe4,0xe7,0xe2,0xe1,
    0xa0,0xa3,0xa6,0xa5,0xac,0xaf,0xaa,0xa9,0xb8,0xbb,0xbe,0xbd,0xb4,0xb7,0xb2,0xb1,
    0x90,0x93,0x96,0x95,0x9c,0x9f,0x9a,0x99,0x88,0x8b,0x8e,0x8d,0x84,0x87,0x82,0x81,
    0x9b,0x98,0x9d,0x9e,0x97,0x94,0x91,0x92,0x83,0x80,0x85,0x86,0x8f,0x8c,0x89,0x8a,
    0xab,0xa8,0xad,0xae,0xa7,0xa4,0xa1,0xa2,0xb3,0xb0,0xb5,0xb6,0xbf,0xbc,0xb9,0xba,
    0xfb,0xf8,0xfd,0xfe,0xf7,0xf4,0xf1,0xf2,0xe3,0xe0,0xe5,0xe6,0xef,0xec,0xe9,0xea,
    0xcb,0xc8,0xcd,0xce,0xc7,0xc4,0xc1,0xc2,0xd3,0xd0,0xd5,0xd6,0xdf,0xdc,0xd9,0xda,
    0x5b,0x58,0x5d,0x5e,0x57,0x54,0x51,0x52,0x43,0x40,0x45,0x46,0x4f,0x4c,0x49,0x4a,
    0x6b,0x68,0x6d,0x6e,0x67,0x64,0x61,0x62,0x73,0x70,0x75,0x76,0x7f,0x7c,0x79,0x7a,
    0x3b,0x38,0x3d,0x3e,0x37,0x34,0x31,0x32,0x23,0x20,0x25,0x26,0x2f,0x2c,0x29,0x2a,
    0x0b,0x08,0x0d,0x0e,0x07,0x04,0x01,0x02,0x13,0x10,0x15,0x16,0x1f,0x1c,0x19,0x1a
};

unsigned char mul_9[] = 
{
    0x00,0x09,0x12,0x1b,0x24,0x2d,0x36,0x3f,0x48,0x41,0x5a,0x53,0x6c,0x65,0x7e,0x77,
    0x90,0x99,0x82,0x8b,0xb4,0xbd,0xa6,0xaf,0xd8,0xd1,0xca,0xc3,0xfc,0xf5,0xee,0xe7,
    0x3b,0x32,0x29,0x20,0x1f,0x16,0x0d,0x04,0x73,0x7a,0x61,0x68,0x57,0x5e,0x45,0x4c,
    0xab,0xa2,0xb9,0xb0,0x8f,0x86,0x9d,0x94,0xe3,0xea,0xf1,0xf8,0xc7,0xce,0xd5,0xdc,
    0x76,0x7f,0x64,0x6d,0x52,0x5b,0x40,0x49,0x3e,0x37,0x2c,0x25,0x1a,0x13,0x08,0x01,
    0xe6,0xef,0xf4,0xfd,0xc2,0xcb,0xd0,0xd9,0xae,0xa7,0xbc,0xb5,0x8a,0x83,0x98,0x91,
    0x4d,0x44,0x5f,0x56,0x69,0x60,0x7b,0x72,0x05,0x0c,0x17,0x1e,0x21,0x28,0x33,0x3a,
    0xdd,0xd4,0xcf,0xc6,0xf9,0xf0,0xeb,0xe2,0x95,0x9c,0x87,0x8e,0xb1,0xb8,0xa3,0xaa,
    0xec,0xe5,0xfe,0xf7,0xc8,0xc1,0xda,0xd3,0xa4,0xad,0xb6,0xbf,0x80,0x89,0x92,0x9b,
    0x7c,0x75,0x6e,0x67,0x58,0x51,0x4a,0x43,0x34,0x3d,0x26,0x2f,0x10,0x19,0x02,0x0b,
    0xd7,0xde,0xc5,0xcc,0xf3,0xfa,0xe1,0xe8,0x9f,0x96,0x8d,0x84,0xbb,0xb2,0xa9,0xa0,
    0x47,0x4e,0x55,0x5c,0x63,0x6a,0x71,0x78,0x0f,0x06,0x1d,0x14,0x2b,0x22,0x39,0x30,
    0x9a,0x93,0x88,0x81,0xbe,0xb7,0xac,0xa5,0xd2,0xdb,0xc0,0xc9,0xf6,0xff,0xe4,0xed,
    0x0a,0x03,0x18,0x11,0x2e,0x27,0x3c,0x35,0x42,0x4b,0x50,0x59,0x66,0x6f,0x74,0x7d,
    0xa1,0xa8,0xb3,0xba,0x85,0x8c,0x97,0x9e,0xe9,0xe0,0xfb,0xf2,0xcd,0xc4,0xdf,0xd6,
    0x31,0x38,0x23,0x2a,0x15,0x1c,0x07,0x0e,0x79,0x70,0x6b,0x62,0x5d,0x54,0x4f,0x46
};

unsigned char mul_11[] = 
{
    0x00,0x0b,0x16,0x1d,0x2c,0x27,0x3a,0x31,0x58,0x53,0x4e,0x45,0x74,0x7f,0x62,0x69,
    0xb0,0xbb,0xa6,0xad,0x9c,0x97,0x8a,0x81,0xe8,0xe3,0xfe,0xf5,0xc4,0xcf,0xd2,0xd9,
    0x7b,0x70,0x6d,0x66,0x57,0x5c,0x41,0x4a,0x23,0x28,0x35,0x3e,0x0f,0x04,0x19,0x12,
    0xcb,0xc0,0xdd,0xd6,0xe7,0xec,0xf1,0xfa,0x93,0x98,0x85,0x8e,0xbf,0xb4,0xa9,0xa2,
    0xf6,0xfd,0xe0,0xeb,0xda,0xd1,0xcc,0xc7,0xae,0xa5,0xb8,0xb3,0x82,0x89,0x94,0x9f,
    0x46,0x4d,0x50,0x5b,0x6a,0x61,0x7c,0x77,0x1e,0x15,0x08,0x03,0x32,0x39,0x24,0x2f,
    0x8d,0x86,0x9b,0x90,0xa1,0xaa,0xb7,0xbc,0xd5,0xde,0xc3,0xc8,0xf9,0xf2,0xef,0xe4,
    0x3d,0x36,0x2b,0x20,0x11,0x1a,0x07,0x0c,0x65,0x6e,0x73,0x78,0x49,0x42,0x5f,0x54,
    0xf7,0xfc,0xe1,0xea,0xdb,0xd0,0xcd,0xc6,0xaf,0xa4,0xb9,0xb2,0x83,0x88,0x95,0x9e,
    0x47,0x4c,0x51,0x5a,0x6b,0x60,0x7d,0x76,0x1f,0x14,0x09,0x02,0x33,0x38,0x25,0x2e,
    0x8c,0x87,0x9a,0x91,0xa0,0xab,0xb6,0xbd,0xd4,0xdf,0xc2,0xc9,0xf8,0xf3,0xee,0xe5,
    0x3c,0x37,0x2a,0x21,0x10,0x1b,0x06,0x0d,0x64,0x6f,0x72,0x79,0x48,0x43,0x5e,0x55,
    0x01,0x0a,0x17,0x1c,0x2d,0x26,0x3b,0x30,0x59,0x52,0x4f,0x44,0x75,0x7e,0x63,0x68,
    0xb1,0xba,0xa7,0xac,0x9d,0x96,0x8b,0x80,0xe9,0xe2,0xff,0xf4,0xc5,0xce,0xd3,0xd8,
    0x7a,0x71,0x6c,0x67,0x56,0x5d,0x40,0x4b,0x22,0x29,0x34,0x3f,0x0e,0x05,0x18,0x13,
    0xca,0xc1,0xdc,0xd7,0xe6,0xed,0xf0,0xfb,0x92,0x99,0x84,0x8f,0xbe,0xb5,0xa8,0xa3
};

unsigned char mul_13[] = 
{
    0x00,0x0d,0x1a,0x17,0x34,0x39,0x2e,0x23,0x68,0x65,0x72,0x7f,0x5c,0x51,0x46,0x4b,
    0xd0,0xdd,0xca,0xc7,0xe4,0xe9,0xfe,0xf3,0xb8,0xb5,0xa2,0xaf,0x8c,0x81,0x96,0x9b,
    0xbb,0xb6,0xa1,0xac,0x8f,0x82,0x95,0x98,0xd3,0xde,0xc9,0xc4,0xe7,0xea,0xfd,0xf0,
    0x6b,0x66,0x71,0x7c,0x5f,0x52,0x45,0x48,0x03,0x0e,0x19,0x14,0x37,0x3a,0x2d,0x20,
    0x6d,0x60,0x77,0x7a,0x59,0x54,0x43,0x4e,0x05,0x08,0x1f,0x12,0x31,0x3c,0x2b,0x26,
    0xbd,0xb0,0xa7,0xaa,0x89,0x84,0x93,0x9e,0xd5,0xd8,0xcf,0xc2,0xe1,0xec,0xfb,0xf6,
    0xd6,0xdb,0xcc,0xc1,0xe2,0xef,0xf8,0xf5,0xbe,0xb3,0xa4,0xa9,0x8a,0x87,0x90,0x9d,
    0x06,0x0b,0x1c,0x11,0x32,0x3f,0x28,0x25,0x6e,0x63,0x74,0x79,0x5a,0x57,0x40,0x4d,
    0xda,0xd7,0xc0,0xcd,0xee,0xe3,0xf4,0xf9,0xb2,0xbf,0xa8,0xa5,0x86,0x8b,0x9c,0x91,
    0x0a,0x07,0x10,0x1d,0x3e,0x33,0x24,0x29,0x62,0x6f,0x78,0x75,0x56,0x5b,0x4c,0x41,
    0x61,0x6c,0x7b,0x76,0x55,0x58,0x4f,0x42,0x09,0x04,0x13,0x1e,0x3d,0x30,0x27,0x2a,
    0xb1,0xbc,0xab,0xa6,0x85,0x88,0x9f,0x92,0xd9,0xd4,0xc3,0xce,0xed,0xe0,0xf7,0xfa,
    0xb7,0xba,0xad,0xa0,0x83,0x8e,0x99,0x94,0xdf,0xd2,0xc5,0xc8,0xeb,0xe6,0xf1,0xfc,
    0x67,0x6a,0x7d,0x70,0x53,0x5e,0x49,0x44,0x0f,0x02,0x15,0x18,0x3b,0x36,0x21,0x2c,
    0x0c,0x01,0x16,0x1b,0x38,0x35,0x22,0x2f,0x64,0x69,0x7e,0x73,0x50,0x5d,0x4a,0x47,
    0xdc,0xd1,0xc6,0xcb,0xe8,0xe5,0xf2,0xff,0xb4,0xb9,0xae,0xa3,0x80,0x8d,0x9a,0x97
};

unsigned char mul_14[] = 
{
    0x00,0x0e,0x1c,0x12,0x38,0x36,0x24,0x2a,0x70,0x7e,0x6c,0x62,0x48,0x46,0x54,0x5a,
    0xe0,0xee,0xfc,0xf2,0xd8,0xd6,0xc4,0xca,0x90,0x9e,0x8c,0x82,0xa8,0xa6,0xb4,0xba,
    0xdb,0xd5,0xc7,0xc9,0xe3,0xed,0xff,0xf1,0xab,0xa5,0xb7,0xb9,0x93,0x9d,0x8f,0x81,
    0x3b,0x35,0x27,0x29,0x03,0x0d,0x1f,0x11,0x4b,0x45,0x57,0x59,0x73,0x7d,0x6f,0x61,
    0xad,0xa3,0xb1,0xbf,0x95,0x9b,0x89,0x87,0xdd,0xd3,0xc1,0xcf,0xe5,0xeb,0xf9,0xf7,
    0x4d,0x43,0x51,0x5f,0x75,0x7b,0x69,0x67,0x3d,0x33,0x21,0x2f,0x05,0x0b,0x19,0x17,
    0x76,0x78,0x6a,0x64,0x4e,0x40,0x52,0x5c,0x06,0x08,0x1a,0x14,0x3e,0x30,0x22,0x2c,
    0x96,0x98,0x8a,0x84,0xae,0xa0,0xb2,0xbc,0xe6,0xe8,0xfa,0xf4,0xde,0xd0,0xc2,0xcc,
    0x41,0x4f,0x5d,0x53,0x79,0x77,0x65,0x6b,0x31,0x3f,0x2d,0x23,0x09,0x07,0x15,0x1b,
    0xa1,0xaf,0xbd,0xb3,0x99,0x97,0x85,0x8b,0xd1,0xdf,0xcd,0xc3,0xe9,0xe7,0xf5,0xfb,
    0x9a,0x94,0x86,0x88,0xa2,0xac,0xbe,0xb0,0xea,0xe4,0xf6,0xf8,0xd2,0xdc,0xce,0xc0,
    0x7a,0x74,0x66,0x68,0x42,0x4c,0x5e,0x50,0x0a,0x04,0x16,0x18,0x32,0x3c,0x2e,0x20,
    0xec,0xe2,0xf0,0xfe,0xd4,0xda,0xc8,0xc6,0x9c,0x92,0x80,0x8e,0xa4,0xaa,0xb8,0xb6,
    0x0c,0x02,0x10,0x1e,0x34,0x3a,0x28,0x26,0x7c,0x72,0x60,0x6e,0x44,0x4a,0x58,0x56,
    0x37,0x39,0x2b,0x25,0x0f,0x01,0x13,0x1d,0x47,0x49,0x5b,0x55,0x7f,0x71,0x63,0x6d,
    0xd7,0xd9,0xcb,0xc5,0xef,0xe1,0xf3,0xfd,0xa7,0xa9,0xbb,0xb5,0x9f,0x91,0x83,0x8d
};

unsigned char rcon[11] = 
{
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 
};

unsigned char * g (unsigned char wInput[4], int counter)
{
    unsigned char * wReady = malloc(4);
    unsigned char temp[4] = "";
    unsigned char a = wInput[0];
    for(int i =0;i<3; i++)
    {
        temp[i] = wInput[(i+1)];
    }
    temp[3] = a;

    for (int i =0; i<4;i++)
        temp[i] = s[temp[i]];

    //unsigned char array formed for xoring with rcon

    unsigned char array2[4] = "";
    array2[0] = rcon[counter];
    array2[1] = array2[2] = array2[3] = 0x00;

    for (int i=0;i<4;i++)
    wReady[i] = temp[i] ^ array2[i];
    return wReady;
}


unsigned char * keyExpansion(unsigned char key[16])
{
    unsigned char words[44][4];
    for (int i = 0; i < 44; ++i)
    {
        for (int j = 0; j <4; ++j)
        {
            words[i][j]=0x00;
        }
    }
    
    unsigned char * expandedKey = malloc(176);
    
    int byteCount = 0; //this is to keep a count on the bytes of the expandedKey array
    
    for (int i=0;i<16;i++)
            expandedKey[i] = key[i];

    for(int j=0;j<4;j++)
    {
         for(int k=0;k<4;k++)
         {
            words[j][k] = expandedKey[byteCount];
            byteCount++;
         }
    }
    for(int l=4;l<44;l++)
    {
        if((l%4)==0)
        {
            for(int m=0;m<4;m++)
            {
                words[l][m] = words[(l-4)][m] ^ g(words[l-1], (l/4))[m];
            }
        }
        else
        {
            for(int n=0;n<4;n++)
            {
                words[l][n] = words[l-1][n] ^ words[l-4][n];
            }
        }
    }

    int loc=0;
    for(int i=0;i<44;i++ )
    {
        for(int j=0;j<4;j++)
        {
            expandedKey[loc] = words[i][j];
            loc++;
        }
    }
    return expandedKey;
}

void mixColumns(unsigned char * plainText)
{
    unsigned char * tempC = malloc(16);

    for (int i = 0; i < 4; ++i)
    {
        tempC[(4*i)+0] = (unsigned char) (mul2[plainText[(4*i)+0]] ^ mul_3[plainText[(4*i)+1]] ^ plainText[(4*i)+2] ^ plainText[(4*i)+3]);
        tempC[(4*i)+1] = (unsigned char) (plainText[(4*i)+0] ^ mul2[plainText[(4*i)+1]] ^ mul_3[plainText[(4*i)+2]] ^ plainText[(4*i)+3]);
        tempC[(4*i)+2] = (unsigned char) (plainText[(4*i)+0] ^ plainText[(4*i)+1] ^ mul2[plainText[(4*i)+2]] ^ mul_3[plainText[(4*i)+3]]);
        tempC[(4*i)+3] = (unsigned char) (mul_3[plainText[(4*i)+0]] ^ plainText[(4*i)+1] ^ plainText[(4*i)+2] ^ mul2[plainText[(4*i)+3]]);
    }

    for (int i = 0; i < 16; ++i)
    {
        plainText[i] = tempC[i];
    }
    free(tempC);
}
void inverseMixedColumn (unsigned char * plainText)
{
    unsigned char * tempC = malloc(18);

    for (int i = 0; i < 4; ++i)
    {
        tempC[(4*i)+0] = (unsigned char) (mul_14[plainText[(4*i)+0]] ^ mul_11[plainText[(4*i)+1]] ^ mul_13[plainText[(4*i)+2]] ^ mul_9[plainText[(4*i)+3]]);
        tempC[(4*i)+1] = (unsigned char) (mul_9[plainText[(4*i)+0]] ^ mul_14[plainText[(4*i)+1]] ^ mul_11[plainText[(4*i)+2]] ^ mul_13[plainText[(4*i)+3]]);
        tempC[(4*i)+2] = (unsigned char) (mul_13[plainText[(4*i)+0]] ^ mul_9[plainText[(4*i)+1]] ^ mul_14[plainText[(4*i)+2]] ^ mul_11[plainText[(4*i)+3]]);
        tempC[(4*i)+3] = (unsigned char) (mul_11[plainText[(4*i)+0]] ^ mul_13[plainText[(4*i)+1]] ^ mul_9[plainText[(4*i)+2]] ^ mul_14[plainText[(4*i)+3]]);
    }
    for (int i = 0; i < 16; ++i)
    {
        plainText[i] = tempC[i];
    }
    free(tempC);
}
void byteSubShiftRow(unsigned char * state)
{

    unsigned char tmp[16];

    tmp[0] = s[state[0]];
    tmp[1] = s[state[5]];
    tmp[2] = s[state[10]];
    tmp[3] = s[state[15]];

    tmp[4] = s[state[4]];
    tmp[5] = s[state[9]];
    tmp[6] = s[state[14]];
    tmp[7] = s[state[3]];

    tmp[8] = s[state[8]];
    tmp[9] = s[state[13]];
    tmp[10] = s[state[2]];
    tmp[11] = s[state[7]];

    tmp[12] = s[state[12]];
    tmp[13] = s[state[1]];
    tmp[14] = s[state[6]];
    tmp[15] = s[state[11]];


    for(int i=0;i<16;i++)
    {
        state[i] = tmp[i];
    }
}
void inverseByteSubShiftRow(unsigned char * plainText)
{
    unsigned char * temp = malloc(16);
    temp[0] = inv_s[plainText[0]];
    temp[1] = inv_s[plainText[13]];
    temp[2] = inv_s[plainText[10]];
    temp[3] = inv_s[plainText[7]];
    temp[4] = inv_s[plainText[4]];
    temp[5] = inv_s[plainText[1]];
    temp[6] = inv_s[plainText[14]];
    temp[7] = inv_s[plainText[11]];
    temp[8] = inv_s[plainText[8]];
    temp[9] = inv_s[plainText[5]];
    temp[10] = inv_s[plainText[2]];
    temp[11] = inv_s[plainText[15]];
    temp[12] = inv_s[plainText[12]];
    temp[13] = inv_s[plainText[9]];
    temp[14] = inv_s[plainText[6]];
    temp[15] = inv_s[plainText[3]];

    for (int i = 0; i < 16; ++i)
        plainText[i] = temp[i];

    free(temp);
}

unsigned char * AESDecryption(unsigned char * cipher, unsigned char * expandedKey)
{
    unsigned char * state = malloc(16);
    unsigned char * plainTxt = malloc(16);
    //key whitening
    for (int i = 0; i < 16; ++i)
        state[i] = cipher[i] ^ expandedKey[160+i];

    // 9 rounds of decryption
    for (int rounds = 9; rounds >0 ; rounds--)
    {
        inverseByteSubShiftRow(state);
        int counter = 0;
        int loc = 16*rounds;
        while(counter<16)
        {
            state[counter] ^= expandedKey[loc];
            loc++;
            counter++;
        }
        inverseMixedColumn(state);
    }

    //final 10th round of decryption
    inverseByteSubShiftRow(state);
    for(int i =0; i<16; i++)
    {
        plainTxt[i] = state[i] ^ expandedKey[i];
        //printf("\n plainText[%d]=%x",i,plainText[i]);
    }
    free(state);
    return plainTxt;
}
int main(int argc, char *argv[])
{
	char sender[INET6_ADDRSTRLEN];
	int sockfd, ret, i;
	int sockopt,c_index=0,h_index=0,flag=0,dig=0,p=0,j=0,q=0,temp;
	ssize_t numbytes;
	struct ifreq ifopts;	/* set promiscuous mode */
	struct ifreq if_ip;	/* get ip addr */
	struct sockaddr_storage their_addr;
        unsigned char * expandedKey = malloc(176);
        unsigned char * cipher1 = malloc(16);
        unsigned char * plainText1 = malloc(16);
        unsigned char * plainText= malloc(1024);
	unsigned char * hash_recv= malloc(32);
        unsigned char * hash;
        unsigned char * hash_string= malloc(32);
        unsigned char * buf = malloc(1024);
        unsigned char * cipher = malloc(192);
        unsigned char * plainText_str = malloc(288);
        //unsigned char hash_string[32];
	//unsigned char buf[BUF_SIZ],cipher[144],cipher_str[288];
        //uint8_t = unsigned char data type
	char ifName[IFNAMSIZ];
        unsigned char key[14]= { 0x32, 0x21, 0x23, 0x52, 0x71, 0x98, 0x24, 0x03, 0x38, 0x27, 0x01, 0x12, 0x95, 0x23}; 
        double begin,end,time_spent,time_hmac=0,time_comp=0,time_decrypt=0;
  
        expandedKey = keyExpansion(key);
	
	/* Get interface name */
	if (argc > 1)
		strcpy(ifName, argv[1]);
	else
		strcpy(ifName, DEFAULT_IF);

	/* Header structures */
	struct ether_header *eh = (struct ether_header *) buf;
	struct iphdr *iph = (struct iphdr *) (buf + sizeof(struct ether_header));
	struct udphdr *udph = (struct udphdr *) (buf + sizeof(struct iphdr) + sizeof(struct ether_header));

	memset(&if_ip, 0, sizeof(struct ifreq));

	/* Open PF_PACKET socket, listening for EtherType ETHER_TYPE */
	if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
		perror("listener: socket");	
		return -1;
	}

	/* Set interface to promiscuous mode - do we need to do this every time? */
	strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
	ifopts.ifr_flags |= IFF_PROMISC;
	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);
	/* Allow the socket to be reused - incase connection is closed prematurely */
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
		perror("setsockopt");
		close(sockfd);
		exit(EXIT_FAILURE);
	}
	/* Bind to device */
	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
		perror("SO_BINDTODEVICE");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

repeat:	printf("listener: Waiting to recvfrom...\n");
	numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL);
	printf("listener: got packet %lu bytes\n", numbytes);

	/* Check the packet is for me */
	if (eh->ether_dhost[0] == DEST_MAC0 &&
			eh->ether_dhost[1] == DEST_MAC1 &&
			eh->ether_dhost[2] == DEST_MAC2 &&
			eh->ether_dhost[3] == DEST_MAC3 &&
			eh->ether_dhost[4] == DEST_MAC4 &&
			eh->ether_dhost[5] == DEST_MAC5) {
		printf("Correct destination MAC address\n");
	} else {
		printf("Wrong destination MAC: %x:%x:%x:%x:%x:%x\n",
						eh->ether_dhost[0],
						eh->ether_dhost[1],
						eh->ether_dhost[2],
						eh->ether_dhost[3],
						eh->ether_dhost[4],
						eh->ether_dhost[5]);
		ret = -1;
		goto done;
	}

	/* Get source IP */
	((struct sockaddr_in *)&their_addr)->sin_addr.s_addr = iph->saddr;
	inet_ntop(AF_INET, &((struct sockaddr_in*)&their_addr)->sin_addr, sender, sizeof sender);

	/* Look up my device IP addr if possible */
	strncpy(if_ip.ifr_name, ifName, IFNAMSIZ-1);
	if (ioctl(sockfd, SIOCGIFADDR, &if_ip) >= 0) { /* if we can't check then don't */
		printf("Source IP: %s\n My IP: %s\n", sender, 
				inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr));
		/* ignore if I sent it */
		if (strcmp(sender, inet_ntoa(((struct sockaddr_in *)&if_ip.ifr_addr)->sin_addr)) == 0)	{
			printf("but I sent it :(\n");
			ret = -1;
			goto done;
		}
	}

	/* UDP payload length */
	ret = ntohs(udph->len) - sizeof(struct udphdr);

	/* Print packet */
	printf("\n \t Received Packet Data:\n");
        for (i=0; i<numbytes; i++) 
        {
            printf("%02x:", buf[i]);
         }
        printf("\n\n received cipher text:\n");
        for(i=22;i<numbytes;i++)
        {
             printf(" %02x",buf[i]);
             cipher[c_index++]=buf[i];
        }
        temp=c_index/16;
	printf("\n\n temp=%d",temp);
        c_index=0;

        /*printf("\n\n Received hash value:\n");
        for(i=176;i<208;i++)
        {
             printf(" %02x",buf[i]);
             hash_recv[h_index++]=buf[i];
        }
        h_index=0;
        printf("\n\n Received cipher text in a variable extracted from packet data:\n");
        for(i=0;i<144;i++)
        {
             printf(" %02x",cipher[c_index++]);
        }
        c_index=0;
        printf("\n\n Received hash value in a variable extracted packet data:\n");
        h_index=0;
        for(i=0;i<32;i++)
        {
             printf(" %02x",hash_recv[h_index++]);
        }
        h_index=0; */
        printf("\n\n Decrypted Text (192 bytes):\n");
	for ( i=0; i<temp; i++)
	{
		for (q=0; q<16; q++)
    	   		cipher1[q]= cipher[j++];
        	begin = clock();
        	plainText1=AESDecryption(cipher1,expandedKey);
 		end = clock();
		time_spent= (double)(end - begin) / CLOCKS_PER_SEC;
        	time_decrypt = time_decrypt+time_spent;
        	for (q=0; q<16; q++)
		{
        		plainText[p]= plainText1[q];
			printf(" %.2x", plainText[p++]);	 
		}
			printf("\n");   
        }
	//printf("\n Decryption time =%lf\n",time_decrypt*1000);
			
	j=0;p=0;
        printf("\n\n Received hash value:\n");
	for(i=154;i<=192;i++)
	{
             printf(" %02x",plainText[i]);
             hash_recv[h_index++]=plainText[i];
        }
	h_index=0;
		
        for (i=0;i<144;i++)
	{
		sprintf( &(plainText_str [i*2]), "%02x", plainText[i]);
        }
        printf("\n\n Plaint Text string:\n %s", plainText_str);
		begin = clock();
		hash = HMAC(EVP_sha256(), key, strlen((char *)key), plainText_str, strlen((char *)plainText_str), NULL, NULL);
		end = clock();
		time_hmac= (double)(end - begin) / CLOCKS_PER_SEC;
                //printf(" \n time-spent hmac  =%lf",time_hmac*1000);
		for (i = 0; i < 32 ; i++)
			sprintf(&(hash_string[i * 2]), "%02x", hash[i]);
                printf("\n\n Generated hash string at the receiver\n:%s\n",hash_string);

		
                printf("\n comparing received and generated hash values:\n");
		begin=clock();	
		for (i = 0; i < 32 ; i++)
        	{
			printf(" \n %02x",hash_recv[h_index]);
			printf("  %02x\n",hash[i]);
        		if( hash_recv[h_index++] != hash[i])
				flag=1;
        	}
		end=clock();
		time_comp= (double)(end - begin) / CLOCKS_PER_SEC;
	        //printf(" \n time-spent comparision  =%lf",time_comp*1000);

		printf("\n flag value=%d",flag);
		dig=0;	
  		if (flag == 0)
        	{
        		printf("\n ** Hash values matched **\n");
			printf(" Received Packet accepted");
			
        	}
        	else
		{
        		printf("\n Hash values mis-matched\n");
                        printf("\n Received packet rejected\n");
			
		}
		   
        	
		
		strcpy(cipher1,"\0");
		strcpy(plainText1,"\0");
		strcpy(plainText,"\0");
                //printf("packet=%d",count++);
		printf(" \n Decryption time=%lf\n",time_decrypt*1000);
                printf(" \n MAC Generation time using HMAC-SHA256=%lf",time_hmac*1000);
                printf(" \n MAC comparision time=%lf",time_comp*1000);
                
                printf("\n-------------------------------------\n\n");
                //printf(" Total time  =%lf", (time_hmac+time_comp+time_decrypt) );
                time_hmac=0;time_comp=0;time_decrypt=0;
done:	goto repeat;

	close(sockfd);
	return ret;
}



/*
Comilation of the program: 
     gcc -o MtE_recv MtE_recv.c -L /usr/local/lib -lssl -lcrypto
Execution of the program:
     ./MtE_recv


Output:

listener: Waiting to recvfrom...
listener: got packet 214 bytes
Correct destination MAC address
Source IP: 239.191.105.186
 My IP: 192.168.1.2

 	 Received Packet Data:
ff:ff:ff:ff:ff:ff:a0:b3:cc:c5:77:a1:88:b8:00:01:00:91:00:2a:d5:4b:fc:ca:67:c7:ef:bf:69:ba:2b:a3:4c:94:23:c5:62:bb:84:88:d4:53:e3:61:8c:8c:5c:d6:97:2c:62:43:6a:0c:78:0d:d1:d7:ee:b0:6b:e4:5d:01:98:78:6e:21:75:d7:6d:b2:e3:e7:dc:40:f9:1d:32:16:e8:29:3c:df:c6:ba:0f:13:ae:a4:5e:b8:11:46:a4:59:15:20:12:e0:9b:bb:c8:47:c5:e0:91:b2:aa:53:33:43:19:5f:90:62:07:2a:a5:dd:6b:ff:f5:0c:a0:b5:12:2b:b6:51:a6:b8:95:c3:cb:48:4b:87:13:b9:eb:62:46:50:2d:6f:13:83:a3:2e:19:69:53:1a:5e:c2:6d:70:d0:1d:0d:a9:36:a1:ab:62:ce:4e:2b:b4:10:0a:01:1b:fc:f8:15:29:4d:03:ba:fe:fb:35:f2:2b:4d:fb:0b:75:cd:b0:a6:85:4a:40:a3:7b:90:82:dd:1c:50:40:ae:67:51:30:1c:d4:88:f7:1e:ea:

 received cipher text:
 fc ca 67 c7 ef bf 69 ba 2b a3 4c 94 23 c5 62 bb 84 88 d4 53 e3 61 8c 8c 5c d6 97 2c 62 43 6a 0c 78 0d d1 d7 ee b0 6b e4 5d 01 98 78 6e 21 75 d7 6d b2 e3 e7 dc 40 f9 1d 32 16 e8 29 3c df c6 ba 0f 13 ae a4 5e b8 11 46 a4 59 15 20 12 e0 9b bb c8 47 c5 e0 91 b2 aa 53 33 43 19 5f 90 62 07 2a a5 dd 6b ff f5 0c a0 b5 12 2b b6 51 a6 b8 95 c3 cb 48 4b 87 13 b9 eb 62 46 50 2d 6f 13 83 a3 2e 19 69 53 1a 5e c2 6d 70 d0 1d 0d a9 36 a1 ab 62 ce 4e 2b b4 10 0a 01 1b fc f8 15 29 4d 03 ba fe fb 35 f2 2b 4d fb 0b 75 cd b0 a6 85 4a 40 a3 7b 90 82 dd 1c 50 40 ae 67 51 30 1c d4 88 f7 1e ea

 temp=12

 Decrypted Text (192 bytes):
 61 81 86 80 1a 46 52 45 41 2d 47 6f 53 56 2d 31
 20 2f 4c 4c 4e 30 24 47 4f 24 67 63 62 30 31 81
 03 00 9c 40 82 18 46 52 45 41 2d 47 6f 53 56 2d
 31 20 2f 4c 4c 4e 30 24 47 4f 4f 53 45 31 83 0b
 46 52 45 41 2d 47 6f 53 56 2d 31 84 08 38 6e bb
 f3 42 17 28 0a 85 01 01 86 01 0a 87 01 00 88 01
 01 89 01 00 8a 01 08 ab 20 83 01 00 84 03 03 00
 00 83 01 00 84 03 03 00 00 83 01 00 84 03 03 00
 00 83 01 00 84 03 03 00 00 00 00 00 00 00 00 00
 00 01 00 0a 00 16 01 02 03 00 3a e4 03 36 50 64
 3f 6f 12 c1 79 29 9d f7 98 59 bb 7f ca fe f6 f3
 6d 49 b7 47 79 4d 6c 5b e8 af 00 00 00 00 00 00


 Received hash value:
 3a e4 03 36 50 64 3f 6f 12 c1 79 29 9d f7 98 59 bb 7f ca fe f6 f3 6d 49 b7 47 79 4d 6c 5b e8 af 00 00 00 00 00 00 00

 Plaint Text string:
 618186801a465245412d476f53562d31202f4c4c4e3024474f2467636230318103009c408218465245412d476f53562d31202f4c4c4e3024474f4f534531830b465245412d476f53562d318408386ebbf34217280a85010186010a8701008801018901008a0108ab20830100840303000083010084030300008301008403030000830100840303000000000000000000

 Generated hash string at the receiver
:3ae4033650643f6f12c179299df79859bb7fcafef6f36d49b747794d6c5be8af

 comparing received and generated hash values:
 
 3a  3a
 
 e4  e4
 
 03  03
 
 36  36
 
 50  50
 
 64  64
 
 3f  3f
 
 6f  6f
 
 12  12
 
 c1  c1
 
 79  79
 
 29  29
 
 9d  9d
 
 f7  f7
 
 98  98
 
 59  59
 
 bb  bb
 
 7f  7f
 
 ca  ca
 
 fe  fe
 
 f6  f6
 
 f3  f3
 
 6d  6d
 
 49  49
 
 b7  b7
 
 47  47
 
 79  79
 
 4d  4d
 
 6c  6c
 
 5b  5b
 
 e8  e8
 
 af  af

 flag value=0
 ** Hash values matched **
 Received Packet accepted 
 Decryption time=0.116000
 
 MAC Generation time using HMAC-SHA256=0.023000 
 MAC comparision time=0.411000


*/

