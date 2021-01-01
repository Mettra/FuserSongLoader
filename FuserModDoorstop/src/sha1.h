#pragma once
#include "core_types.h"

struct SHA1 {
	char digest[20];

	SHA1();

	void reset();
	void update(const u8 *data, u64 len);
	void finalize();

private:
	u32 m_state[5];
	u32 m_count[2];
	u32 __reserved1[1];
	u8  m_buffer[64];
	u32 __reserved2[3];

	u8 m_workspace[64];
	typedef union
	{
		u8  c[64];
		u32 l[16];
	} SHA1_WORKSPACE_BLOCK;
	SHA1_WORKSPACE_BLOCK *m_block;

	void transform(u32 *state, const u8 *buffer);
};