/*
 * PacketProviderDummy.h
 *
 *  Created on: May 14, 2013
 *      Author: kazu
 */

#ifndef PACKETPROVIDERDUMMY_H_
#define PACKETPROVIDERDUMMY_H_

#include "../PacketProvider.h"

// forward declarations
class PacketLisetner;

/**
 * empty PacketProvider that does nothing at all
 */
class PacketProviderDummy : public PacketProvider {

public:

	/** ctor */
	PacketProviderDummy();

	/** dtor */
	virtual ~PacketProviderDummy();

	void addListener(PacketListener* listener) override;

	void start() override;

	void stop() override;

};

#endif /* PACKETPROVIDERDUMMY_H_ */
