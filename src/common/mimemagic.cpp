/*
 * Copyright 2015 by Heiko Schäfer <heiko@rangun.de>
 *
 * This file is part of NetMauMau.
 *
 * NetMauMau is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * NetMauMau is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with NetMauMau.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>

#include "mimemagic.h"

using namespace NetMauMau::Common;

#if defined(HAVE_MAGIC_H) && defined(HAVE_LIBMAGIC)
MimeMagic::MimeMagic() : m_magic(magic_open(MAGIC_MIME_TYPE | MAGIC_NO_CHECK_ASCII |
									 MAGIC_NO_CHECK_COMPRESS | MAGIC_NO_CHECK_ELF |
									 MAGIC_NO_CHECK_FORTRAN | MAGIC_NO_CHECK_TAR |
									 MAGIC_NO_CHECK_TOKENS | MAGIC_NO_CHECK_TROFF)) {
	if(m_magic) {
		if(magic_load(m_magic, NULL)) m_magic = NULL;
	}
}
#else
MimeMagic::MimeMagic() {}
#endif

MimeMagic::~MimeMagic() {
#if defined(HAVE_MAGIC_H) && defined(HAVE_LIBMAGIC)

	if(m_magic) magic_close(m_magic);

#endif
}

MimeMagic &MimeMagic::getInstance() {
	static MimeMagic instance;
	return instance;
}

bool MimeMagic::checkMime(const unsigned char *data, std::size_t dataLen,
						  const char *mime) const {

#if defined(HAVE_MAGIC_H) && defined(HAVE_LIBMAGIC)
	const char *m = m_magic ? magic_buffer(m_magic, data, dataLen) : 0L;
	return m ? (std::strcmp(m, mime) == 0) : true;
#else
	return true;
#endif
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
