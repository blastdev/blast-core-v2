// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The SYSCOIN developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_PUREHEADER_H
#define BITCOIN_PRIMITIVES_PUREHEADER_H

#include "auxpow/consensus.h"
#include "auxpow/serialize.h"
#include "versionbits.h"

/**
 * A block header without auxpow information.  This "intermediate step"
 * in constructing the full header is useful, because it breaks the cyclic
 * dependency between auxpow (referencing a parent block header) and
 * the block header (referencing an auxpow).  The parent block header
 * does not have auxpow itself, so it is a pure header.
 */
class CPureBlockHeader
{
public:
	// header
	int32_t nVersion;
	uint256 hashPrevBlock;
	uint256 hashMerkleRoot;
	uint32_t nTime;
	uint32_t nBits;
	uint32_t nNonce;

	CPureBlockHeader()
	{
		SetNull();
	}

	ADD_SERIALIZE_METHODS;

	template <typename Stream, typename Operation>
	inline void SerializationOp(Stream& s, Operation ser_action) {
		READWRITE(this->nVersion);
		READWRITE(hashPrevBlock);
		READWRITE(hashMerkleRoot);
		READWRITE(nTime);
		READWRITE(nBits);
		READWRITE(nNonce);
	}

	void SetNull();
	inline bool IsNull() const { return (nBits == 0); }
	uint256 GetHash() const;
	inline int64_t GetBlockTime() const { return (int64_t)nTime; }

	/* Below are methods to interpret the version with respect to
	   auxpow data and chain ID.  This used to be in the CBlockVersion
	   class, but was moved here when we switched back to nVersion being
	   a pure int member as preparation to undoing the "abuse" and
	   allowing BIP9 to work.  */

	   /**
		* Extract the base version (without modifiers and chain ID).
		* @return The base version./
		*/
	inline int32_t GetBaseVersion() const { return GetBaseVersion(nVersion); }
	static inline int32_t GetBaseVersion(int32_t ver) { return ver % AuxPow::BLOCK_VERSION_AUXPOW; }

	/**
	 * Set the base version (apart from chain ID and auxpow flag) to
	 * the one given.  This should only be called when auxpow is not yet
	 * set, to initialise a block!
	 * @param nBaseVersion The base version.
	 * @param nChainId The auxpow chain ID.
	 */
	void SetBaseVersion(int32_t nBaseVersion, int32_t nChainId);

	/**
	 * Extract the chain ID.
	 * @return The chain ID encoded in the version.
	 */
	int32_t GetChainID() const;

	/**
	 * Set the chain ID.  This is used for the test suite.
	 * @param ch The chain ID to set.
	 */
	inline void SetChainID(int32_t chainId)
	{
		nVersion %= AuxPow::BLOCK_VERSION_CHAIN_START;
		nVersion |= chainId * AuxPow::BLOCK_VERSION_CHAIN_START;
	}

	/**
	 * Check if the auxpow flag is set in the version.
	 * @return True iff this block version is marked as auxpow.
	 */
	inline bool IsAuxPow() const
	{
		return nVersion & AuxPow::BLOCK_VERSION_AUXPOW;
	}

	/**
	 * Set the auxpow flag.  This is used for testing.
	 * @param auxpow Whether to mark auxpow as true.
	 */
	inline void SetAuxpowVersion(bool auxpow)
	{
		if (auxpow)
			nVersion |= AuxPow::BLOCK_VERSION_AUXPOW;
		else
			nVersion &= AuxPow::BLOCK_VERSION_AUXPOW;
	}

	/**
	 * Check whether this is a "legacy" block without chain ID.
	 * @return True iff it is.
	 */
	inline bool IsLegacy() const { return nVersion == 1; }
};

#endif // BITCOIN_PRIMITIVES_PUREHEADER_H