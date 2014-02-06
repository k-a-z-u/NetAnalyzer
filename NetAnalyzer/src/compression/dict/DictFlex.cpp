/*
 * DictFlex.cpp
 *
 *  Created on: May 7, 2013
 *      Author: kazu
 */

#include "DictFlex.h"
#include <cstring>
#include <cstdlib>

#include <iostream>

DictFlex::DictFlex() : data(nullptr), length(0), index(nullptr) {

	index = (uint32_t*) malloc(DICT_IDX_SIZE);

}

DictFlex::DictFlex(const DictFlex& src) : data(nullptr), length(0), index(nullptr) {

	index = (uint32_t*) malloc(DICT_IDX_SIZE);
	setDict(src.data, src.length);

}

DictFlex::~DictFlex() {
	if (this->data != nullptr) {free(this->data); this->data = nullptr;}
	if (this->index != nullptr) {free(this->index); this->index = nullptr;}
}

void DictFlex::setDict(const uint8_t* data, uint32_t length) {
	if (this->data != nullptr) {free(this->data); this->data = nullptr;}
	this->length = length;
	this->data = (uint8_t*) malloc(length);
	memcpy(this->data, data, length);
	createIndex();
}


void DictFlex::createIndex() {

	memset(index, 0xFF, DICT_IDX_SIZE);
	for (uint32_t i = 0; i < length-2; ++i) {

		uint32_t* offset = &DICT_IDX( &data[i] );
		if (*offset == 0xFFFFFFFF) {*offset = i+0;}


	}

}
