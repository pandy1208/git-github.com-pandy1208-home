#include <stdio.h>
#include <string.h>
/* POINTER defines a generic pointer type */ 
typedef unsigned char *POINTER; 

#define PROTO_LIST(list) list 

/* MD5 context. */ 
typedef struct _MD5_CTX 
{ 
    unsigned long int state[4]; /* state (ABCD) */ 
    unsigned long int count[2]; /* number of bits, modulo 2^64 (lsb first) */ 
    unsigned char buffer[64]; /* input buffer */ 
} MD5_CTX; 

/* Constants for MD5_Transform routine. 
*/ 
#define MD5_S11 7 
#define MD5_S12 12 
#define MD5_S13 17 
#define MD5_S14 22 
#define MD5_S21 5 
#define MD5_S22 9 
#define MD5_S23 14 
#define MD5_S24 20 
#define MD5_S31 4 
#define MD5_S32 11 
#define MD5_S33 16 
#define MD5_S34 23 
#define MD5_S41 6 
#define MD5_S42 10 
#define MD5_S43 15 
#define MD5_S44 21 

static unsigned char MD5_PADDING[64] = { 
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 
}; 

/* MD5_F, MD5_G, MD5_H and MD5_I are basic MD5 functions. 
*/ 
#define MD5_F(x, y, z) (((x) & (y)) | ((~x) & (z))) 
#define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z))) 
#define MD5_H(x, y, z) ((x) ^ (y) ^ (z)) 
#define MD5_I(x, y, z) ((y) ^ ((x) | (~z))) 

/* MD5_ROTATE_LEFT rotates x left n bits. 
*/ 
#define MD5_ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n)))) 
#define MD5_PP(x) (x<<24)|((x<<8)&0xff0000)|((x>>8)&0xff00)|(x>>24)  //将x高低位互换,例如PP(aabbccdd)=ddccbbaa

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4. 
Rotation is separate from addition to prevent recomputation. 
*/ 
#define MD5_FF(a, b, c, d, x, s, ac) { \
    (a) += MD5_F ((b), (c), (d)) + (x) + (unsigned long int)(ac);\
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    } 
#define MD5_GG(a, b, c, d, x, s, ac) { \
    (a) += MD5_G ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    } 
#define MD5_HH(a, b, c, d, x, s, ac) { \
    (a) += MD5_H ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    } 
#define MD5_II(a, b, c, d, x, s, ac) { \
    (a) += MD5_I ((b), (c), (d)) + (x) + (unsigned long int)(ac); \
    (a) = MD5_ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    } 

 
 void MD5_Transform PROTO_LIST ((unsigned long int [4], unsigned char [64])); 
 void MD5_Encode PROTO_LIST ((unsigned char *, unsigned long int *, unsigned int)); 
 void MD5_Decode PROTO_LIST ((unsigned long int *, unsigned char *, unsigned int)); 
 void MD5_MemCpy PROTO_LIST ((POINTER, POINTER, unsigned int)); 
 void MD5_memset PROTO_LIST ((POINTER, int, unsigned int)); 
 void MD5_Init PROTO_LIST ((MD5_CTX *)); 
 void MD5_Update PROTO_LIST ((MD5_CTX *, unsigned char *, unsigned int)); 
 void MD5_Final PROTO_LIST ((unsigned char [16], MD5_CTX *)); 
 void MD5_Encode PROTO_LIST ((unsigned char *, unsigned long int *, unsigned int)); 
 void MD5_Decode PROTO_LIST ((unsigned long int *, unsigned char *, unsigned int)); 
void MD5_String(unsigned char *string);
void MD5_CalculateMD5Value(unsigned char *input,unsigned char *output);

 void MD5_ChangeMD5KEYByIV(void); 
 void MD5_SaveIVConfigToFlash(void); 
void MD5_SaveKeyConfigToFlash(void);
void MD5_GetMD5KeyFromFlash(void);
void MD5_GetMD5IVFromFlash(void);

 
const unsigned char *MD5_Request_Head1="</id><type>request</type></common><id_validata operation=\"request\"></id_validata></root>"; 

const unsigned char *MD5_Head1="</id><type>md5</type></common><id_validata operation=\"md5\"><md5>"; 
const unsigned char *MD5_Head2="</md5></id_validata></root>";
extern const unsigned char *Common_Head;



//md5的默认密钥
const unsigned char MD5_KeyDefault[4]="123";
const unsigned char MD5_IVDefault[4]="123";

//uint8_t MD5_Key[4]="123";
unsigned char MD5_IV[4]="123";
   
/*定义中间变量、MD5计算值*/
unsigned int MD5_A,MD5_B,MD5_C,MD5_D;
unsigned int MD5[4];

/*----------------------------把数据变成16进制的字符放在arr数组中----------------------------*/
void data_to_array_16(unsigned int data,int num, unsigned char *arr)		
{
	unsigned char i;
	unsigned int s1=16,s2=1;

	for(i=0;i<num;i++)
	{
		if(s1 == 0)
			arr[num-i-1]=data/s2;
		else
			arr[num-i-1]=data%s1/s2;
//printf("\r\n s1=%d s2=%d i=%d num=%d data=%d arr[%d]=%d",s1,s2,i,num,data,num-i-1,arr[num-i-1]);
		if(arr[num-i-1]<10)
			arr[num-i-1]=arr[num-i-1]+48;
		else
			arr[num-i-1]=arr[num-i-1]+48+7;

		s1=s1*16,s2=s2*16;
//printf("\r\n s1:%d\r\n",s1);		
	} 
	arr[num]='\0';
//printf("\r\n end of data_to_array_16\r\n");
}



//MD5身份验证请求

  

//输入任意长度的字符串算出其32字节的MD5值。 
void MD5_CalculateMD5Value(unsigned char *input,unsigned char *output) 	
{
 	unsigned char md5_data1[9],md5_data2[9],md5_data3[9],md5_data4[9] ;

//   ClearBuffer(output,sizeof(output));

	MD5_String(input); //引号里面为收到的字符串代码
//printf("\r\n MD5_CalculateMD5Value1 \r\n");
	MD5[0]=MD5_PP(MD5_A);
    MD5[1]=MD5_PP(MD5_B);
    MD5[2]=MD5_PP(MD5_C);
    MD5[3]=MD5_PP(MD5_D); 
//printf("\r\n MD5_CalculateMD5Value2 \r\n");
	data_to_array_16(MD5[0],8, md5_data1);			  //16进制是2的4次。 u32是2的32次。所以需要8次。
	data_to_array_16(MD5[1],8, md5_data2);
	data_to_array_16(MD5[2],8, md5_data3);
	data_to_array_16(MD5[3],8, md5_data4);
//printf("\r\n MD5_CalculateMD5Value3 \r\n");
	strcpy((char *)output,(char *)md5_data1);
	strcat((char *)output,(char *)md5_data2);
	strcat((char *)output,(char *)md5_data3);
	strcat((char *)output,(char *)md5_data4);

 }











//
 void MD5_String(unsigned char *string)
{
	MD5_CTX context;
	unsigned char digest[16];  
//	char output1[34];  
//	static char output[33]={0};  
	unsigned int len = strlen((char *)string); //strlen函数获取字符串长度 
//	int i; 

	MD5_Init(&context);  
	MD5_Update(&context, (unsigned char*)string, len);  
	MD5_Final(digest, &context);  

}

 




/* MD5 initialization. Begins an MD5 operation, writing a new context. 
*/ 
 void MD5_Init( MD5_CTX *context ) 
{ 
    context->count[0] = context->count[1] = 0; 
    /* Load magic initialization constants. 
    */ 
    context->state[0] = 0x67452301; 
    context->state[1] = 0xefcdab89; 
    context->state[2] = 0x98badcfe; 
    context->state[3] = 0x10325476; 
} 

/* MD5 block update operation. Continues an MD5 message-digest 
operation, processing another message block, and updating the 
context. 
*/ 
	void MD5_Update( 
                      MD5_CTX *context, /* context */ 
                      unsigned char *input, /* input block */ 
                      unsigned int inputLen /* length of input block */ 
                      ) 
{ 
    unsigned int i, index, partLen; 
    
    /* Compute number of bytes mod 64 */ 
    index = (unsigned int)((context->count[0] >> 3) & 0x3F); 
    
    /* Update number of bits */ 
    if ((context->count[0] += ((unsigned long int)inputLen << 3)) < ((unsigned long int)inputLen << 3)) 
        context->count[1]++; 
    context->count[1] += ((unsigned long int)inputLen >> 29); 
    
    partLen = 64 - index; 
    
    /* Transform as many times as possible. 
    */ 
    if (inputLen >= partLen) 
	{ 
        MD5_MemCpy ((POINTER)&context->buffer[index], (POINTER)input, partLen); 
        MD5_Transform (context->state, context->buffer); 
        
        for (i = partLen; i + 63 < inputLen; i += 64) 
            MD5_Transform (context->state, &input[i]); 
        
        index = 0; 
    } 
    else 
        i = 0; 
    
    /* Buffer remaining input */ 
    MD5_MemCpy ((POINTER)&context->buffer[index], (POINTER)&input[i], inputLen-i); 
} 

/* MD5 finalization. Ends an MD5 message-digest operation, writing the 
the message digest and zeroizing the context. 
*/ 
	void MD5_Final(unsigned char digest[16], /* message digest */ MD5_CTX *context /* context */) 
{ 
    unsigned char bits[8]; 
    unsigned int index, padLen; 
    
    /* Save number of bits */ 
    MD5_Encode (bits, context->count, 8); 
    
    /* Pad out to 56 mod 64. 
    */ 
    index = (unsigned int)((context->count[0] >> 3) & 0x3f); 
    padLen = (index < 56) ? (56 - index) : (120 - index); 
    MD5_Update (context, MD5_PADDING, padLen); 
    
    /* Append length (before padding) */ 
    MD5_Update (context, bits, 8); 
    
    /* Store state in digest */ 
    MD5_Encode (digest, context->state, 16); 
    
    /* Zeroize sensitive information. 
    */ 
    MD5_memset ((POINTER)context, 0, sizeof (*context)); 
} 


/* MD5 basic transformation. Transforms state based on block.   */  
 void MD5_Transform(unsigned long int state[4], unsigned char block[64])  
{
	unsigned long int a = state[0], b = state[1], c = state[2], d = state[3], x[16]; 
	MD5_Decode (x, block, 64);

	/* Round 1 */  
	MD5_FF (a, b, c, d, x[0], MD5_S11, 0xd76aa478); /* 1 */ 
	MD5_FF (d, a, b, c, x[1], MD5_S12, 0xe8c7b756); /* 2 */ 
	MD5_FF (c, d, a, b, x[2], MD5_S13, 0x242070db); /* 3 */
	MD5_FF (b, c, d, a, x[3], MD5_S14, 0xc1bdceee); /* 4 */
	MD5_FF (a, b, c, d, x[4], MD5_S11, 0xf57c0faf); /* 5 */
	MD5_FF (d, a, b, c, x[5], MD5_S12, 0x4787c62a); /* 6 */
	MD5_FF (c, d, a, b, x[6], MD5_S13, 0xa8304613); /* 7 */
	MD5_FF (b, c, d, a, x[7], MD5_S14, 0xfd469501); /* 8 */  
	MD5_FF (a, b, c, d, x[8], MD5_S11, 0x698098d8); /* 9 */  
	MD5_FF (d, a, b, c, x[9], MD5_S12, 0x8b44f7af); /* 10 */  
	MD5_FF (c, d, a, b, x[10], MD5_S13, 0xffff5bb1); /* 11 */  
	MD5_FF (b, c, d, a, x[11], MD5_S14, 0x895cd7be); /* 12 */  
	MD5_FF (a, b, c, d, x[12], MD5_S11, 0x6b901122); /* 13 */  
	MD5_FF (d, a, b, c, x[13], MD5_S12, 0xfd987193); /* 14 */  
	MD5_FF (c, d, a, b, x[14], MD5_S13, 0xa679438e); /* 15 */  
	MD5_FF (b, c, d, a, x[15], MD5_S14, 0x49b40821); /* 16 */  

	/* Round 2 */ 
	MD5_GG (a, b, c, d, x[1], MD5_S21, 0xf61e2562); /* 17 */  
	MD5_GG (d, a, b, c, x[6], MD5_S22, 0xc040b340); /* 18 */  
	MD5_GG (c, d, a, b, x[11], MD5_S23, 0x265e5a51); /* 19 */
	MD5_GG (b, c, d, a, x[0], MD5_S24, 0xe9b6c7aa); /* 20 */
	MD5_GG (a, b, c, d, x[5], MD5_S21, 0xd62f105d); /* 21 */  
	MD5_GG (d, a, b, c, x[10], MD5_S22, 0x2441453); /* 22 */  
	MD5_GG (c, d, a, b, x[15], MD5_S23, 0xd8a1e681); /* 23 */  
	MD5_GG (b, c, d, a, x[4], MD5_S24, 0xe7d3fbc8); /* 24 */  
	MD5_GG (a, b, c, d, x[9], MD5_S21, 0x21e1cde6); /* 25 */ 
	MD5_GG (d, a, b, c, x[14], MD5_S22, 0xc33707d6); /* 26 */  
	MD5_GG (c, d, a, b, x[3], MD5_S23, 0xf4d50d87); /* 27 */
	MD5_GG (b, c, d, a, x[8], MD5_S24, 0x455a14ed); /* 28 */  
	MD5_GG (a, b, c, d, x[13], MD5_S21, 0xa9e3e905); /* 29 */  
	MD5_GG (d, a, b, c, x[2], MD5_S22, 0xfcefa3f8); /* 30 */  
	MD5_GG (c, d, a, b, x[7], MD5_S23, 0x676f02d9); /* 31 */  
	MD5_GG (b, c, d, a, x[12], MD5_S24, 0x8d2a4c8a); /* 32 */ 

	/* Round 3 */  
	MD5_HH (a, b, c, d, x[5], MD5_S31, 0xfffa3942); /* 33 */  
	MD5_HH (d, a, b, c, x[8], MD5_S32, 0x8771f681); /* 34 */  
	MD5_HH (c, d, a, b, x[11], MD5_S33, 0x6d9d6122); /* 35 */  
	MD5_HH (b, c, d, a, x[14], MD5_S34, 0xfde5380c); /* 36 */  
	MD5_HH (a, b, c, d, x[1], MD5_S31, 0xa4beea44); /* 37 */  
	MD5_HH (d, a, b, c, x[4], MD5_S32, 0x4bdecfa9); /* 38 */  
	MD5_HH (c, d, a, b, x[7], MD5_S33, 0xf6bb4b60); /* 39 */  
	MD5_HH (b, c, d, a, x[10], MD5_S34, 0xbebfbc70); /* 40 */  
	MD5_HH (a, b, c, d, x[13], MD5_S31, 0x289b7ec6); /* 41 */  
	MD5_HH (d, a, b, c, x[0], MD5_S32, 0xeaa127fa); /* 42 */  
	MD5_HH (c, d, a, b, x[3], MD5_S33, 0xd4ef3085); /* 43 */  
	MD5_HH (b, c, d, a, x[6], MD5_S34, 0x4881d05); /* 44 */  
	MD5_HH (a, b, c, d, x[9], MD5_S31, 0xd9d4d039); /* 45 */  
	MD5_HH (d, a, b, c, x[12], MD5_S32, 0xe6db99e5); /* 46 */  
	MD5_HH (c, d, a, b, x[15], MD5_S33, 0x1fa27cf8); /* 47 */  
	MD5_HH (b, c, d, a, x[2], MD5_S34, 0xc4ac5665); /* 48 */  

	/* Round 4 */  
	MD5_II (a, b, c, d, x[0], MD5_S41, 0xf4292244); /* 49 */  
	MD5_II (d, a, b, c, x[7], MD5_S42, 0x432aff97); /* 50 */  
	MD5_II (c, d, a, b, x[14], MD5_S43, 0xab9423a7); /* 51 */  
	MD5_II (b, c, d, a, x[5], MD5_S44, 0xfc93a039); /* 52 */  
	MD5_II (a, b, c, d, x[12], MD5_S41, 0x655b59c3); /* 53 */  
	MD5_II (d, a, b, c, x[3], MD5_S42, 0x8f0ccc92); /* 54 */  
	MD5_II (c, d, a, b, x[10], MD5_S43, 0xffeff47d); /* 55 */  
	MD5_II (b, c, d, a, x[1], MD5_S44, 0x85845dd1); /* 56 */  
	MD5_II (a, b, c, d, x[8], MD5_S41, 0x6fa87e4f); /* 57 */  
	MD5_II (d, a, b, c, x[15], MD5_S42, 0xfe2ce6e0); /* 58 */  
	MD5_II (c, d, a, b, x[6], MD5_S43, 0xa3014314); /* 59 */  
	MD5_II (b, c, d, a, x[13], MD5_S44, 0x4e0811a1); /* 60 */  
	MD5_II (a, b, c, d, x[4], MD5_S41, 0xf7537e82); /* 61 */  
	MD5_II (d, a, b, c, x[11], MD5_S42, 0xbd3af235); /* 62 */  
	MD5_II (c, d, a, b, x[2], MD5_S43, 0x2ad7d2bb); /* 63 */  
	MD5_II (b, c, d, a, x[9], MD5_S44, 0xeb86d391); /* 64 */  

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	MD5_A=state[0];
	MD5_B=state[1];
	MD5_C=state[2];
	MD5_D=state[3];

	/* Zeroize sensitive information.   	*/
//	MD5_memset ((POINTER)x, 0, sizeof (x)); 
 } 

 /* Encodes input (UINT4) into output (unsigned char). Assumes len is 
  a multiple of 4.   */
 void MD5_Encode(unsigned char *output, unsigned long int *input, unsigned int len)
{
	unsigned int i, j;
	for (i = 0, j = 0; j < len; i++, j += 4) 
	{
		output[j] = (unsigned char)(input[i] & 0xff);  
		output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);  
		output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);  
		output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);  
	}
 } 

/* Decodes input (unsigned char) into output (UINT4). Assumes len is 
  a multiple of 4.   */ 

 void MD5_Decode(unsigned long int *output,unsigned char *input,unsigned int len)
{
	unsigned int i, j;
	for (i = 0, j = 0; j < len; i++, j += 4)
	output[i] = ((unsigned long int)input[j]) | (((unsigned long int)input[j+1]) << 8) | 
	(((unsigned long int)input[j+2]) << 16) | (((unsigned long int)input[j+3]) << 24);  
 }

 /* Note: Replace "for loop" with standard memcpy if possible.  
 */  
 void MD5_MemCpy(POINTER output,POINTER input,unsigned int len)
{
	unsigned int i;
	for (i = 0; i < len; i++)  
		output[i] = input[i];  
}

/* Note: Replace "for loop" with standard memset if possible.  
 */

 void MD5_memset(POINTER output, int value,unsigned int len)
{
	unsigned int i;
	for (i = 0; i < len; i++)
		((char *)output)[i] = (char)value; 
}

