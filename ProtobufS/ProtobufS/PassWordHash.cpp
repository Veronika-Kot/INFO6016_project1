#include <windows.h>
#include <Wincrypt.h>
#include <iostream>
#include <string>
#include <sstream> 
#include <openssl\sha.h>

SHA256_CTX ctx;

void createSalt(std::string &salt);
void stringToHash(std::string plainText, std::string &hashedPassword);

void passwordToHashNewSalt(std::string password, std::string& salt, std::string& hashedPassword)
{
	SHA256_Init(&ctx);
	
	createSalt(salt);
	password += salt;

	stringToHash(password, hashedPassword);
}

void passwordToHash(std::string password, std::string salt, std::string& hashedPassword)
{
	SHA256_Init(&ctx);

	password += salt;

	stringToHash(password, hashedPassword);
	//std::cout << salt << std::endl;
}

void stringToHash(std::string plainText, std::string &hashPassword)
{
	unsigned char digest[SHA256_DIGEST_LENGTH];
	SHA256_Update(&ctx, plainText.c_str(), strlen(plainText.c_str()));
	SHA256_Final(digest, &ctx);

	char hash[SHA256_DIGEST_LENGTH * 2 + 1];
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(&(hash[i * 2]), "%02x", (unsigned int)(digest[i]));
	}
	std::stringstream streamHash;
	streamHash << hash;
	hashPassword = streamHash.str();
}

void createSalt(std::string &salt)
{
	HCRYPTPROV hProvider = 0;
	const DWORD dwLength = 16;
	BYTE pbBuffer[dwLength] = {};
	if (!CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		return;
	}

	if (!CryptGenRandom(hProvider, dwLength, pbBuffer))
	{
		CryptReleaseContext(hProvider, 0);
	}
	std::stringstream ss;
	for (DWORD i = 0; i < dwLength; ++i)
	{
		ss << std::hex << static_cast<unsigned int>(pbBuffer[i]);
	}
	salt = ss.str();

	if (CryptReleaseContext(hProvider, 0))
	{

	}
}