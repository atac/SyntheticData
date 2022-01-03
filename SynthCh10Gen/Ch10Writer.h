/* Ch10Writer - Base class for all Chapter 10 data packet writers

Some day this will be the base class for the Ch 10 writer objects. For now I 
just need a place to put Ch 10 TMATS and data headers versions.
*/

// IRIG 106-15
#define CH10_VER_TMATS          "15"
#define CH10_VER_RECORDER       0x0B
#define CH10_VER_HDR_TMATS      0x07
#define CH10_VER_HDR_TIME       0x06
#define CH10_VER_HDR_INDEX      0x06
#define CH10_VER_HDR_1553       0x06
#define CH10_VER_HDR_PCM        0x06
#define CH10_VER_HDR_VIDEO      0x06
#define CH10_VER_HDR_A429       0x06