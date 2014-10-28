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

#include <sstream>

#include <cerrno>
#include <cstring>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include <sys/types.h>

#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif

#include "serverconnection.h"

using namespace NetMauMau::Server;

Connection::Connection(uint16_t port, const char *server) : AbstractConnection(server, port),
	m_caps() {}

Connection::~Connection() {

	for(std::vector<NAMESOCKFD>::const_iterator i(getRegisteredPlayers().begin());
			i != getRegisteredPlayers().end(); ++i) {

		try {
			send("BYE", 3, i->sockfd);
		} catch(const NetMauMau::Common::Exception::SocketException &) {}

		close(i->sockfd);
	}
}

bool Connection::wire(int sockfd, const struct sockaddr *addr, socklen_t addrlen) const {

	int yes = 1;

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char *>(&yes), sizeof(int)) == -1) {
		return false;
	}

	return bind(sockfd, addr, addrlen) == 0;
}

std::string Connection::wireError(const std::string &err) const {
	return std::string("could not bind") + (!err.empty() ? ": " : "") + (!err.empty() ? err : "");
}

void Connection::connect() throw(NetMauMau::Common::Exception::SocketException) {

	AbstractConnection::connect();

	if(listen(getSocketFD(), SOMAXCONN)) {
		throw NetMauMau::Common::Exception::SocketException(std::strerror(errno), getSocketFD());
	}
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic push
int Connection::wait(timeval *tv) const {

	fd_set rfds;

	FD_ZERO(&rfds);
	FD_SET(getSocketFD(), &rfds);

	return ::select(getSocketFD() + 1, &rfds, NULL, NULL, tv);
}
#pragma GCC diagnostic pop

Connection::ACCEPT_STATE Connection::accept(INFO &info,
		bool refuse) throw(NetMauMau::Common::Exception::SocketException) {

	ACCEPT_STATE accepted = REFUSED;

	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

	int cfd = ::accept(getSocketFD(), reinterpret_cast<struct sockaddr *>(&peer_addr),
					   &peer_addr_len);

	if(cfd != -1) {

		info.sockfd = cfd;

		char host[NI_MAXHOST], service[NI_MAXSERV];

		int err = getnameinfo(reinterpret_cast<struct sockaddr *>(&peer_addr), peer_addr_len,
							  host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

		(std::istringstream(service)) >> info.port;
		info.host = host;

		if(!err) {

			const std::string hello(PACKAGE_NAME);

			std::ostringstream os;
			os << hello << ' ' << MIN_MAJOR << '.' << MIN_MINOR;

			send(os.str().c_str(), os.str().length(), cfd);

			const std::string rHello = read(cfd);

			if(rHello != "CAP") {

				const std::string::size_type spc = rHello.find(' ');
				const std::string::size_type dot = rHello.find('.');
				const bool isClientHello = spc != std::string::npos &&
										   dot != std::string::npos && spc < dot;

				if(isClientHello && rHello.substr(0, std::strlen(PACKAGE_NAME)) == hello) {

					(std::istringstream(rHello.substr(spc + 1, dot))) >> info.maj;
					(std::istringstream(rHello.substr(dot + 1))) >> info.min;

					uint32_t cver = (info.maj << 16u) | info.min;
					uint32_t minver = (static_cast<uint16_t>(MIN_MAJOR) << 16u) |
									  static_cast<uint16_t>(MIN_MINOR);
					uint32_t maxver = (static_cast<uint16_t>(SERVER_VERSION_MAJOR) << 16u) |
									  static_cast<uint16_t>(SERVER_VERSION_MINOR);

					send("NAME", 4, cfd);
					info.name = read(cfd);

					if(cver >= minver && cver <= maxver && !refuse) {
						NAMESOCKFD nsf = { info.name, cfd };
						registerPlayer(nsf);
						send("OK", 2, cfd);
						accepted = PLAY;
					} else {
						send(cver <= maxver ? "NO" : "VM", 2, cfd);
						shutdown(cfd, SHUT_RDWR);
						close(cfd);
						accepted = REFUSED;
					}
				} else {
					send("NO", 2, cfd);
					shutdown(cfd, SHUT_RDWR);
					close(cfd);
				}
			} else {

				std::ostringstream oscap;

				for(CAPABILITIES::const_iterator i(m_caps.begin()); i != m_caps.end(); ++i) {
					oscap << i->first << '=' << i->second << '\0';
				}

				oscap << "CAPEND" << '\0';

				send(oscap.str().c_str(), oscap.str().length(), cfd);

				shutdown(cfd, SHUT_RDWR);
				close(cfd);

				accepted = CAP;
			}

		} else {
			shutdown(cfd, SHUT_RDWR);
			close(cfd);

			throw NetMauMau::Common::Exception::SocketException(gai_strerror(err));
		}
	}

	return accepted;
}

Connection &Connection::operator<<(const std::string &msg)
throw(NetMauMau::Common::Exception::SocketException) {

	for(std::vector<NAMESOCKFD>::const_iterator i(getRegisteredPlayers().begin());
			i != getRegisteredPlayers().end(); ++i) {
		write(i->sockfd, msg);
	}

	return *this;
}

Connection &Connection::operator>>(std::string &msg)
throw(NetMauMau::Common::Exception::SocketException) {

	for(std::vector<NAMESOCKFD>::const_iterator i(getRegisteredPlayers().begin());
			i != getRegisteredPlayers().end(); ++i) {
		msg = read(i->sockfd);
	}

	return *this;
}

void Connection::intercept() throw(NetMauMau::Common::Exception::SocketException) {

	timeval tv = { 0, 600 };

	if(wait(&tv) > 0) {
		INFO info;
		accept(info, true);
	}
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
