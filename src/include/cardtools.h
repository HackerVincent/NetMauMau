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

/**
 * @file
 * @brief Common functions to handle cards
 * @author Heiko Schäfer <heiko@rangun.de>
 */

#ifndef NETMAUMAU_CARDTOOLS_H
#define NETMAUMAU_CARDTOOLS_H

#include <cstdlib>

#include "icard.h"

/// @brief Main namespace of %NetMauMau
namespace NetMauMau {

/**
 * @brief %Common classes and functions used by clients and server as well
 *
 * @c #include @c "cardtools.h" to use the tool functions.
 */
namespace Common {

/**
 * @brief Get an array of the four @c SUIT symbols
 *
 * @return const std::string* the four @c SUIT symbols
 */
_EXPORT const std::string *getSuitSymbols() _CONST;

/**
 * @brief Converts a @c SUIT symbol to a ANSI color representation
 *
 * @param suit the @c SUIT symbol
 * @return std::string a @c SUIT symbol in ANSI color representation
 */
_EXPORT std::string ansiSuit(const std::string &suit);

/**
 * @brief Converts a symbol to a NetMauMau::Common::ICard::SUIT
 *
 * @param symbol the symbol
 * @return NetMauMau::Common::ICard::SUIT the @c SUIT
 */
_EXPORT NetMauMau::Common::ICard::SUIT symbolToSuit(const std::string &symbol);

/**
 * @brief Converts a NetMauMau::Common::ICard::SUIT to a symbol
 *
 * @param suit the @c SUIT to convert
 * @param ansi if @c true create a ANSI color representation
 * @param endansi if @c false (default) don't end the ANSI color codes
 * @return std::string the @c SUIT symbol
 */
_EXPORT std::string suitToSymbol(NetMauMau::Common::ICard::SUIT suit, bool ansi,
								 bool endansi = false);

/**
 * @brief Parses a textual description
 *
 * Parses a textual description and stores the suit and the rank into the pointers,
 * which cannot be null.
 *
 * @param[in] desc the textual description of the card
 * @param[out] suit pointer to store the resulting @c SUIT
 * @param[out] rank pointer to store the resulting @c RANK
 * @return bool @c true if the parsing was successful, @c false otherwise
 */
_EXPORT _NOUNUSED bool parseCardDesc(const std::string &desc, NetMauMau::Common::ICard::SUIT *suit,
									 NetMauMau::Common::ICard::RANK *rank) _NONNULL(2, 3);

/**
 * @brief Creates a card description
 *
 * @param suite the @c SUIT
 * @param rank the @c RANK
 * @param ansi if @c true create a ANSI color representation
 * @return std::string the card description
 */
_EXPORT std::string createCardDesc(NetMauMau::Common::ICard::SUIT suite,
								   NetMauMau::Common::ICard::RANK rank, bool ansi);

/**
 * @brief Gets the points of a @c RANK
 *
 * @param rank the @c RANK
 * @return std::size_t the points of a @c RANK
 */
_EXPORT std::size_t getCardPoints(NetMauMau::Common::ICard::RANK rank) _CONST;

}

}

#endif /* NETMAUMAU_CARDTOOLS_H */

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 