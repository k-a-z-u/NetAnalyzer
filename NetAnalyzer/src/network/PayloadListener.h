/*
 * PayloadListener.h
 *
 *  Created on: Apr 7, 2013
 *      Author: kazu
 */

#ifndef PAYLOADLISTENER_H_
#define PAYLOADLISTENER_H_

struct Payload;

/**
 * interface to listen for incoming payload
 * (payload = content of a packet)
 */
class PayloadListener {

public:

	/** dtor */
	virtual ~PayloadListener() {;}

	/** event is called on sniffed packets */
	virtual void onPayload(const Payload& payload) = 0;

};



#endif /* PAYLOADLISTENER_H_ */
