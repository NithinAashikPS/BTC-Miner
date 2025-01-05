//
// Created by aashik on 01/01/25.
//

#pragma once
#include <string>

inline std::string kernel_code = R"CLC(
#define SHA256_BLOCK_SIZE 32            // SHA256 outputs a 32 byte digest

#define ROT_LEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
#define ROT_RIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))

#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define EP0(x) (ROT_RIGHT(x,2) ^ ROT_RIGHT(x,13) ^ ROT_RIGHT(x,22))
#define EP1(x) (ROT_RIGHT(x,6) ^ ROT_RIGHT(x,11) ^ ROT_RIGHT(x,25))
#define SIG0(x) (ROT_RIGHT(x,7) ^ ROT_RIGHT(x,18) ^ ((x) >> 3))
#define SIG1(x) (ROT_RIGHT(x,17) ^ ROT_RIGHT(x,19) ^ ((x) >> 10))

typedef unsigned char BYTE;
typedef uint  WORD;

typedef struct {
	BYTE data[64];
	WORD dataLen;
	unsigned long long bitLen;
	WORD state[8];
} SHA256_CTX;

__constant WORD dev_k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len);
void sha256_final(SHA256_CTX *ctx, BYTE hash[]);

void sha256_transform(SHA256_CTX *ctx, const __private BYTE data[])
{
	WORD a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];
    WORD S[8];

    //mycpy32(S, ctx->state);

    #pragma unroll 16
	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);

    #pragma unroll 64
	for (; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

    #pragma unroll 64
	for (i = 0; i < 64; ++i) {
		t1 = h + EP1(e) + CH(e, f, g) + dev_k[i] + m[i];
		t2 = EP0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

void memset(void* ptr, int value, size_t num) {
    unsigned char* p = (unsigned char*)ptr;  // Cast the pointer to unsigned char* for byte-by-byte manipulation
    while (num--) {
        *p = (unsigned char)value;  // Set the byte to the given value
        p++;  // Move to the next byte
    }
	ptr = p;
}

void sha256_init(SHA256_CTX *ctx)
{
	ctx->dataLen = 0;
	ctx->bitLen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void sha256_update(SHA256_CTX *ctx, const BYTE data[], size_t len)
{
	WORD i;

	// for each byte in message
	for (i = 0; i < len; ++i) {
		// ctx->data == message 512 bit chunk
		ctx->data[ctx->dataLen] = data[i];
		ctx->dataLen++;
		if (ctx->dataLen == 64) {
			sha256_transform(ctx, (const __private BYTE*)ctx->data);
			ctx->bitLen += 512;
			ctx->dataLen = 0;
		}
	}
}

void sha256_final(SHA256_CTX *ctx, BYTE hash[])
{
	WORD i;

	i = ctx->dataLen;

	// Pad whatever data is left in the buffer.
	if (ctx->dataLen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, (const __private BYTE*)ctx->data);
		memset(ctx->data, 0, 56);
	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitLen += ctx->dataLen * 8;
	ctx->data[63] = ctx->bitLen;
	ctx->data[62] = ctx->bitLen >> 8;
	ctx->data[61] = ctx->bitLen >> 16;
	ctx->data[60] = ctx->bitLen >> 24;
	ctx->data[59] = ctx->bitLen >> 32;
	ctx->data[58] = ctx->bitLen >> 40;
	ctx->data[57] = ctx->bitLen >> 48;
	ctx->data[56] = ctx->bitLen >> 56;
	sha256_transform(ctx, (const __private BYTE*)ctx->data);

	// Since this implementation uses little endian byte ordering and SHA uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
	}
}

bool isValid(BYTE* blockHash, BYTE* targetHash) {

    for(int i = 0; i < 32; i++) {
        if(blockHash[i] > targetHash[i]) {
            return false;
        } else if(blockHash[i] < targetHash[i]) {
            return true;
        }
    }
    return false;
}

void toLittleEndian(long int num, BYTE *hex) {
    for (int i = 0; i < sizeof(int); i++) {
        hex[i] = (num >> (i * 8)) & 0xFF;
    }
}

void reverse(BYTE* arr, int size) {
    int start = 0;
    int end = size - 1;

    while (start < end) {
        BYTE temp = arr[start];
        arr[start] = arr[end];
        arr[end] = temp;
        start++; end--;
    }
}

void toPrivate(__global BYTE* src, BYTE* dest, int size) {
    for (int i = 0; i < size; ++i) {
        dest[i] = src[i];
    }
}

typedef struct {
    BYTE     VERSION[04];
    BYTE   PREV_HASH[32];
    BYTE MERKLE_ROOT[32];
    BYTE   TIMESTAMP[04];
    BYTE   BITS_DIFF[04];
    BYTE DIFF_TARGET[32];
    unsigned int x;
} BLOCK_TEMPLATE;

typedef struct {
    unsigned int start;
    unsigned int end;
} NONCE_RANGE;

__kernel void StartMining(__global BLOCK_TEMPLATE* blockTemplate, __global NONCE_RANGE* nonceRanges, __global unsigned int *nonceIdx, __global int *done) {
	NONCE_RANGE nonceRange = nonceRanges[atomic_add(nonceIdx, 1)];

    SHA256_CTX ctx;

    BYTE digest[32];
    BYTE nonce[sizeof(unsigned int)];

    BYTE     VERSION[04]; toPrivate(blockTemplate->VERSION, VERSION, sizeof(blockTemplate->VERSION));
    BYTE   PREV_HASH[32]; toPrivate(blockTemplate->PREV_HASH, PREV_HASH, sizeof(blockTemplate->PREV_HASH));
    BYTE MERKLE_ROOT[32]; toPrivate(blockTemplate->MERKLE_ROOT, MERKLE_ROOT, sizeof(blockTemplate->MERKLE_ROOT));
    BYTE   TIMESTAMP[04]; toPrivate(blockTemplate->TIMESTAMP, TIMESTAMP, sizeof(blockTemplate->TIMESTAMP));
    BYTE   BITS_DIFF[04]; toPrivate(blockTemplate->BITS_DIFF, BITS_DIFF, sizeof(blockTemplate->BITS_DIFF));
    BYTE DIFF_TARGET[32]; toPrivate(blockTemplate->DIFF_TARGET, DIFF_TARGET, sizeof(blockTemplate->DIFF_TARGET));

	for (unsigned int random=nonceRange.start; random<=nonceRange.end; random++) {
		if (*done)
			break;
        toLittleEndian(random, nonce);

        sha256_init(&ctx);
        sha256_update(&ctx, VERSION, sizeof(VERSION));
        sha256_update(&ctx, PREV_HASH, sizeof(PREV_HASH));
        sha256_update(&ctx, MERKLE_ROOT, sizeof(MERKLE_ROOT));
        sha256_update(&ctx, TIMESTAMP, sizeof(TIMESTAMP));
        sha256_update(&ctx, BITS_DIFF, sizeof(BITS_DIFF));
        sha256_update(&ctx, nonce, sizeof(nonce));
        sha256_final(&ctx, digest);

        sha256_init(&ctx);
        sha256_update(&ctx, digest, 32);
        sha256_final(&ctx, digest);
        reverse(digest, 32);

        if (isValid(digest, DIFF_TARGET)) {
            atomic_or(done, 1);
            blockTemplate->x = random;
			printf("Mined Hash : ");
			for (int i = 0; i < 32; i++) {
				printf("%02X", digest[i]);
			}
			printf("\n");
        }
	}
}
)CLC";