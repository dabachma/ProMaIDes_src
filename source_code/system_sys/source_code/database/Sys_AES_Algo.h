#pragma once
/**\class Sys_AES_Algo
	\author Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2011 
*/

#ifndef SYS_AES_ALGO
#define SYS_AES_ALGO


#include <cstring>
#include <exception>

///The Electronic Code Book (ECB), Cipher Block Chaining (CBC) and Cipher Feedback Block (CFB) modes are implemented.
enum _sys_operation_mode_aes{
		///In ECB mode if the same block is encrypted twice with the same key, the resulting ciphertext blocks are the same.
		ECB=0, 
		///In CBC Mode a ciphertext block is obtained by first xoring the plaintext block with the previous ciphertext block, and encrypting the resulting value.
		CBC=1, 
		///In CFB mode a ciphertext block is obtained by encrypting the previous ciphertext block and xoring the resulting value with the plaintext.
		CFB=2 
};
///Default setting
enum _sys_default_settings_aes{ 
	///Default block size
	DEFAULT_BLOCK_SIZE=16,
	///Maximum block size
	MAX_BLOCK_SIZE=32, 
	///Maximum rounds
	MAX_ROUNDS=14, 
	///
	MAX_KC=8, 
	///
	MAX_BC=8 
};


///Class for encrypting/decrypting passwords. For further instructions  see below. \ingroup sys
/**
Rijndael (pronounced Reindaal) is a block cipher, designed by Joan Daemen and Vincent Rijmen as a candidate algorithm for the AES.
The cipher has a variable block length and key length. The authors currently specify how to use keys with a length
of 128, 192, or 256 bits to encrypt blocks with al length of 128, 192 or 256 bits (all nine combinations of
key length and block length are possible). Both block length and key length can be extended very easily to
multiples of 32 bits.
Rijndael can be implemented very efficiently on a wide range of processors and in hardware. 
This implementation is based on the Java Implementation used with the Cryptix toolkit found at:
http://www.esat.kuleuven.ac.be/~rijmen/rijndael/rijndael.zip
Java code authors: Raif S. Naffah, Paulo S. L. M. Barreto
This Implementation was tested against KAT test published by the authors of the method and the
results were identical.
*/
class Sys_AES_Algo
{
public:
	///Default constructor
	Sys_AES_Algo(void);
	///Default destructor
	virtual ~Sys_AES_Algo(void);

	//members
	///Null chain
	static char const* sm_chain0;


	//methods
	///Expand a user-supplied key material into a session key.
	void MakeKey(char const* key, char const* chain, int keylength=DEFAULT_BLOCK_SIZE, int blockSize=DEFAULT_BLOCK_SIZE);

	///Encrypt exactly one block of plaintext.
    void EncryptBlock(char const* in, char* result);
	
	///Decrypt exactly one block of ciphertext.
	void DecryptBlock(char const* in, char* result);

	///Encrypt exactly one block of plaintext.
	void Encrypt(char const* in, char* result, size_t n, int iMode=ECB);
	///Decrypt exactly one block of ciphertext.
	void Decrypt(char const* in, char* result, size_t n, int iMode=ECB);

	///Get Key Length
	int GetKeyLength(void);

	///Get the Block Size
	int	GetBlockSize(void);
	
	///Get number of Rounds
	int GetRounds(void);

	///Reset the chain
	void ResetChain(void);

	//Function to convert string of unsigned chars to string of chars
	void CharStr2HexStr(unsigned char const* pucCharStr, char* pszHexStr, int iSize);
	//Function to convert string of chars to string of unsigned chars
	void HexStr2CharStr(char const* pszHexStr, unsigned char* pucCharStr, int iSize);

private:

	//members
	static const int sm_alog[256];
	static const int sm_log[256];
	static const char sm_S[256];
    static const char sm_Si[256];
    static const int sm_T1[256];
    static const int sm_T2[256];
    static const int sm_T3[256];
    static const int sm_T4[256];
    static const int sm_T5[256];
    static const int sm_T6[256];
    static const int sm_T7[256];
    static const int sm_T8[256];
    static const int sm_U1[256];
    static const int sm_U2[256];
    static const int sm_U3[256];
    static const int sm_U4[256];
    static const char sm_rcon[30];
    static const int sm_shifts[3][4][2];
	///Error Messages 1
	static char const* sm_szErrorMsg1;
	///Error Messages 2
	static char const* sm_szErrorMsg2;
	///Key Initialization Flag
	bool m_bKeyInit;
	///Encryption (m_Ke) round key
	int m_Ke[MAX_ROUNDS+1][MAX_BC];
	///Decryption (m_Kd) round key
    int m_Kd[MAX_ROUNDS+1][MAX_BC];
	///Key Length
	int m_keylength;
	///Block Size
	int	m_blockSize;
	///Number of Rounds
	int m_iROUNDS;
	///Chain Block 
	char m_chain0[MAX_BLOCK_SIZE];
	char m_chain[MAX_BLOCK_SIZE];
	///Auxiliary private use buffer
	int tk[MAX_KC];
	///Auxiliary private use buffer
	int a[MAX_BC];
	///Auxiliary private use buffer
	int t[MAX_BC];


	//methods
	///Xor application
	void Xor(char* buff, char const* chain);
	//Convenience method to encrypt exactly one block of plaintext, assuming Rijndael's default block size (128-bit).
	void DefEncryptBlock(char const* in, char* result);
	//Convenience method to decrypt exactly one block of plaintext, assuming Rijndael's default block size (128-bit).
	void DefDecryptBlock(char const* in, char* result);

	///Function to convert unsigned char to string of length 2
	void Char2Hex(unsigned char ch, char* szHex);
	///Function to convert string of length 2 to unsigned char
	void Hex2Char(char const* szHex, unsigned char& rch);


};
#endif
