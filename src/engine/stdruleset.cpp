/*
 * Copyright 2014-2015 by Heiko Schäfer <heiko@rangun.de>
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

#include <cassert>

#include "stdruleset.h"

#include "iaceroundlistener.h"
#include "random_gen.h"
#include "cardtools.h"
#include "iplayer.h"

using namespace NetMauMau::RuleSet;

StdRuleSet::StdRuleSet(bool dirChangePossible, std::size_t icc,
					   const NetMauMau::IAceRoundListener *l) : IRuleSet(), m_hasToSuspend(false),
	m_hasSuspended(false), m_takeCardCount(0), m_jackMode(false),
	m_jackSuit(NetMauMau::Common::ICard::SUIT_ILLEGAL), m_aceRound(l), m_aceRoundPlayer(0L),
	m_arl(l), m_curPlayers(0), m_dirChange(false), m_dirChangeIsSuspend(false),
	m_dirChangePossible(dirChangePossible), m_initialCardCount(icc) {}

StdRuleSet::~StdRuleSet() {}

std::size_t StdRuleSet::getMaxPlayers() const {
	return 5;
}

void StdRuleSet::setCurPlayers(std::size_t players) {
	m_curPlayers = players;
}

void StdRuleSet::checkInitial(const NetMauMau::Player::IPlayer *player,
							  const NetMauMau::Common::ICard *playedCard) {
	checkCard(player, 0L, playedCard, false);
}

bool StdRuleSet::checkCard(const NetMauMau::Common::ICard *uncoveredCard,
						   const NetMauMau::Common::ICard *playedCard) const {

	if(!uncoveredCard || !playedCard) return false;

	return (m_aceRound && m_aceRoundPlayer) ?
		   (playedCard->getRank() == m_arl->getAceRoundRank()) :
		   ((playedCard->getRank() == NetMauMau::Common::ICard::JACK
			 && uncoveredCard->getRank() != NetMauMau::Common::ICard::JACK) ||
			((((isJackMode() && getJackSuit() == playedCard->getSuit()) ||
			   (!isJackMode() && (uncoveredCard->getSuit() == playedCard->getSuit() ||
								  (uncoveredCard->getRank() == playedCard->getRank()))))) &&
			 !(playedCard->getRank() == NetMauMau::Common::ICard::JACK &&
			   uncoveredCard->getRank() == NetMauMau::Common::ICard::JACK)));
}

bool StdRuleSet::checkCard(const NetMauMau::Player::IPlayer *player,
						   const NetMauMau::Common::ICard *uncoveredCard,
						   const NetMauMau::Common::ICard *playedCard, bool /*ai*/) {

	const bool accepted = uncoveredCard ? checkCard(uncoveredCard, playedCard) : true;

	if(accepted && (m_aceRound && uncoveredCard && (!m_aceRoundPlayer ||
					m_aceRoundPlayer == player) && playedCard->getRank() ==
					m_arl->getAceRoundRank())) {

		const bool acrCont = m_aceRoundPlayer;

		if((m_aceRoundPlayer = player->getAceRoundChoice() ? player : 0L)) {
			m_arl->aceRoundStarted(player);
		} else if(acrCont) {
			m_arl->aceRoundEnded(player);
		}

	} else if(accepted && isDirChange(playedCard)) {
		m_dirChange = true;
	}

	m_hasToSuspend = accepted && (playedCard->getRank() == NetMauMau::Common::ICard::EIGHT ||
								  (isDirChange(playedCard) && m_dirChangeIsSuspend));

	m_hasSuspended = false;

	if(accepted && playedCard->getRank() == NetMauMau::Common::ICard::SEVEN) {
		m_takeCardCount += 2;
	} else if(accepted && playedCard->getRank() == NetMauMau::Common::ICard::JACK &&
			  (m_curPlayers > 2 || player->getCardCount() > 1)) {
		m_jackSuit = player->getJackChoice(uncoveredCard ? uncoveredCard : playedCard, playedCard);
		m_jackMode = true;
	}

	return accepted;
}

std::size_t StdRuleSet::lostPointFactor(const NetMauMau::Common::ICard *uc) const {
	return uc->getRank() == NetMauMau::Common::ICard::JACK ? 2 : 1;
}

bool StdRuleSet::hasToSuspend() const {
	return m_hasToSuspend && !m_hasSuspended;
}

void StdRuleSet::hasSuspended() {
	m_hasSuspended = true;
}

std::size_t StdRuleSet::takeCardCount() const {
	return m_takeCardCount;
}

std::size_t StdRuleSet::takeCards(const NetMauMau::Common::ICard *playedCard) const {
	return (playedCard && playedCard->getRank() == NetMauMau::Common::ICard::SEVEN) ? 0 :
		   takeCardCount();
}

void StdRuleSet::hasTakenCards() {
	m_takeCardCount = 0;
}

std::size_t StdRuleSet::initialCardCount() const {
	return m_initialCardCount;
}

bool StdRuleSet::suspendIfNoMatchingCard() const {
	return false;
}

bool StdRuleSet::takeIfLost() const {
	return true;
}

bool StdRuleSet::isAceRoundPossible() const {
	return m_aceRound;
}

NetMauMau::Common::ICard::RANK StdRuleSet::getAceRoundRank() const {
	return m_arl ? m_arl->getAceRoundRank() : NetMauMau::Common::ICard::RANK_ILLEGAL;
}

bool StdRuleSet::isAceRound() const {
	return m_aceRoundPlayer && isAceRoundPossible();
}

bool StdRuleSet::isJackMode() const {
	return m_jackMode;
}

void StdRuleSet::setJackModeOff() {
	m_jackMode = false;
}

bool StdRuleSet::isDirChange(const NetMauMau::Common::ICard *playedCard) const {
	return m_dirChangePossible && playedCard->getRank() == NetMauMau::Common::ICard::NINE;
}

bool StdRuleSet::hasDirChange() const {
	return m_dirChange;
}

void StdRuleSet::dirChanged() {
	m_dirChange = false;
}

bool StdRuleSet::getDirChangeIsSuspend() const {
	return m_dirChangeIsSuspend;
}

void StdRuleSet::setDirChangeIsSuspend(bool suspend) {
	m_dirChangeIsSuspend = suspend;
}

NetMauMau::Common::ICard::SUIT StdRuleSet::getJackSuit() const {
	return m_jackSuit == NetMauMau::Common::ICard::SUIT_ILLEGAL ?
		   NetMauMau::Common::symbolToSuit(NetMauMau::Common::getSuitSymbols()
										   [NetMauMau::Common::genRandom(4)]) : m_jackSuit;
}

void StdRuleSet::reset() throw() {
	m_hasToSuspend = false;
	m_hasSuspended = false;
	m_takeCardCount = 0;
	m_jackMode = false;
	m_jackSuit = NetMauMau::Common::ICard::SUIT_ILLEGAL;
	m_aceRoundPlayer = 0L;
	m_curPlayers = 0;
	m_dirChange = false;
	m_dirChangeIsSuspend = false;
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
