/*
 * Copyright 2014 by Heiko Schäfer <heiko@rangun.de>
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

#if defined(HAVE_CONFIG_H) || defined(IN_IDE_PARSER)
#include "config.h"
#endif

#include <algorithm>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <cstring>
#include <cerrno>

#include "abstractconnection.h"

#ifdef _WIN32
#include <mswsock.h>
#endif

namespace {

#pragma GCC diagnostic ignored "-Weffc++"
#pragma GCC diagnostic push
struct _isSocketFD :
	public std::binary_function < NetMauMau::Common::AbstractConnection::NAMESOCKFD, int, bool > {
	bool operator()(const NetMauMau::Common::AbstractConnection::NAMESOCKFD &nsd,
					int sockfd) const {
		return nsd.sockfd == sockfd;
	}
};

struct _isPlayer : public std::binary_function < NetMauMau::Common::AbstractConnection::NAMESOCKFD,
		std::string, bool > {
	bool operator()(const NetMauMau::Common::AbstractConnection::NAMESOCKFD &nsd,
					const std::string &player) const {
		return nsd.name == player;
	}
};
#pragma GCC diagnostic pop

}

using namespace NetMauMau::Common;

AbstractConnection::_info::_info() : sockfd(-1), name(), host(), port(0), maj(0), min(0) {}

AbstractConnection::_info::~_info() {}

AbstractConnection::AbstractConnection(const char *server, uint16_t port) :
	AbstractSocket(server, port), m_registeredPlayers() {}

AbstractConnection::~AbstractConnection() {}

void AbstractConnection::registerPlayer(const NAMESOCKFD &nfd) {
	m_registeredPlayers.push_back(nfd);
}

const AbstractConnection::PLAYERINFOS &AbstractConnection::getRegisteredPlayers() const {
	return m_registeredPlayers;
}

std::string AbstractConnection::getPlayerName(int sockfd) const {

	const PLAYERINFOS::const_iterator &f(std::find_if(m_registeredPlayers.begin(),
										 m_registeredPlayers.end(),
										 std::bind2nd(_isSocketFD(), sockfd)));

	return f != m_registeredPlayers.end() ? f->name : "";
}

void AbstractConnection::removePlayer(int sockfd) {

	const PLAYERINFOS::iterator &f(std::find_if(m_registeredPlayers.begin(),
								   m_registeredPlayers.end(),
								   std::bind2nd(_isSocketFD(), sockfd)));

	if(f != m_registeredPlayers.end()) m_registeredPlayers.erase(f);
}

void AbstractConnection::addAIPlayers(const std::vector<std::string> &aiPlayers) {
	m_aiPlayers.insert(m_aiPlayers.end(), aiPlayers.begin(), aiPlayers.end());
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
void AbstractConnection::wait(long ms) throw(Exception::SocketException) {

	fd_set rfds;
	int sret = 1;

	while(sret > 0) {

		FD_ZERO(&rfds);
		FD_SET(getSocketFD(), &rfds);

		timeval tv = { ms / 1000L, ms % 1000L };

		if((sret = ::select(getSocketFD() + 1, &rfds, NULL, NULL, &tv)) < 0) {
			throw Exception::SocketException(std::strerror(errno), getSocketFD(), errno);
		} else if(sret > 0) {
			intercept();
#if _POSIX_C_SOURCE >= 200112L && defined(__linux)
			ms = tv.tv_sec * 1000L + tv.tv_usec;
#endif
		}
	}
}
#pragma GCC diagnostic pop

const std::vector<std::string> &AbstractConnection::getAIPlayers() const {
	return m_aiPlayers;
}

void AbstractConnection::reset() throw() {

	for(PLAYERINFOS::const_iterator i(m_registeredPlayers.begin()); i != m_registeredPlayers.end();
			++i) {
		close(i->sockfd);
	}

	m_registeredPlayers.clear();
	m_aiPlayers.clear();
}

uint16_t AbstractConnection::getMajorFromHello(const std::string &hello,
		std::string::size_type dot, std::string::size_type spc) const {
	return (uint16_t)std::strtoul(hello.substr(spc + 1, dot).c_str(), NULL, 10);
}

uint16_t AbstractConnection::getMinorFromHello(const std::string &hello,
		std::string::size_type dot) const {
	return (uint16_t)std::strtoul(hello.substr(dot + 1).c_str(), NULL, 10);
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
