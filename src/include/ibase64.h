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

#ifndef NETMAUMAU_CLIENT_IBASE64_H
#define NETMAUMAU_CLIENT_IBASE64_H

#include <vector>
#include <string>

#include "linkercontrol.h"

namespace NetMauMau {

namespace Client {

class IBase64 {
	DISALLOW_COPY_AND_ASSIGN(IBase64)
public:
	virtual std::string encode(unsigned char const *buf, unsigned int bufLen) const = 0;
	virtual std::vector<unsigned char> decode(std::string const &base64) const = 0;

	virtual ~IBase64() {}

protected:
	IBase64() {}
};

}

}

#endif /* NETMAUMAU_CLIENT_IBASE64_H */

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
