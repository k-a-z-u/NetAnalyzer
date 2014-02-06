/*
 * PnlContentType.cpp
 *
 *  Created on: Apr 14, 2013
 *      Author: kazu
 */

#include "PnlContentType.h"

#include "../../../analyzer/Summary.h"
#include "../elements/GTab.h"
#include "../helper/GuiHelper.h"



#include <algorithm>
#include <sstream>
#include <iomanip>


/** tab for huffman tree */
class TabHuffman : public GTab {
public:

	PnlContentType& parent;
	bool showPercent;

	TabHuffman(PnlContentType& parent) : GTab("Huffman"), parent(parent), showPercent(true) {;}
	bool onKey(int key) override {
		if (key == 'h') {showPercent = !showPercent; return true;}
		return false;
	}
	void refresh(GRender& render) override {

		// stats
		//uint64_t huffmanCompSize =;
		//uint32_t huffmanPercent = huffmanCompSize * 100l / ;

		GuiHelper::printIntPercent(render, 37, 0, parent.getContentType().response.sizePayload,  parent.getContentType().huffman.getCompressedSize(), " b");

		render.print(0,0, "compressed: ");
		render.print(40, 0, "entropy: " + floatToString(parent.getContentType().huffman.getEntropy(), 4));
		//render.print(40, 0, "press 'h' to toggle percent/absolute");

		// get current probability from huffman
		const uint32_t* counter = parent.getContentType().huffman.getCounterValues();
		const uint64_t counterTotal = parent.getContentType().huffman.getTotalValues();

		struct HuffEntry {
			uint8_t byte;
			uint32_t cnt;
		};

		// add them to a vector
		std::vector<HuffEntry> entries;
		for (uint32_t i = 0; i <= 0xFF; ++i) {
			entries.push_back({(uint8_t)i, counter[i]});
		}

		// sort them
		auto lambda = [] (const HuffEntry& a, const HuffEntry& b) {return a.cnt > b.cnt;};
		std::sort(entries.begin(), entries.end(), lambda);

		// output the sorted values in structured manner
		static std::stringstream ss;
		ss.str("");


		uint32_t x = 0;
		uint32_t y = 2;

		for (uint32_t i = 0; i < 256; ++i) {

			uint8_t byte = entries[i].byte;

			// output as ascii-char or it's ordinal
			if ((byte >= 32 && byte < 127)) {
				ss << "'" << byte << "'";;
			} else {
				ss << std::setfill('0') << std::setw(3) << (uint32_t) byte;
			}

			ss << ":";

			if (showPercent) {
				ss << std::setiosflags(std::ios::fixed);
				ss << std::setprecision(5);
				ss << " " << (entries[i].cnt / (float) counterTotal);
			} else {
				ss << std::setfill(' ') << std::setw(7) << entries[i].cnt;
			}

			ss << " | ";

			// display
			render.print(x, y, ss.str());
			ss.str("");

			// next line
			++y;
			if (y >= (b.h)) {y = 2; x += 15;}

			// window full?
			//x += 15;
			if (x >= (b.w - 4) - 15) {return;}

		}


	}
};

/** tab for word analysis */
class TabWords : public GTab {
public:
	PnlContentType& parent;
	TabWords(PnlContentType& parent) : GTab("Words"), parent(parent) {;}
	bool onKey(int key) override {(void) key; return false;}
	void refresh(GRender& render) override {

		// cfg
		const uint32_t sy = 2;
		const uint32_t colW = 25;

		// show number of words
		uint32_t numWords = parent.getContentType().words.getNumWords();
		render.print(0, 0, "words: " + intToString(numWords));

		// get current stats
		std::vector<Word> words;
		parent.getContentType().words.getStats(words);

		// show all words and their occurence
		uint32_t x = 0;
		uint32_t y = sy;
		for (uint32_t i = 0; i < words.size(); ++i) {

			render.print(x, y, words[i].word);
			render.printR(x+colW-2, y, intToString(words[i].cnt));

			// move down or right
			if (++y >= (b.h)) {y = sy; x += colW;}

			// done?
			if (x >= (b.w - 4) - colW) {return;}

		}

	}
};

class TabAnalysis : public GTab {
public:
	PnlContentType& parent;
	TabAnalysis(PnlContentType& parent) : GTab("Analysis"), parent(parent) {;}
	bool onKey(int key) override {
		if (key == 'h') {GuiHelper::toggleBytePercentUnit(); return true;}
		return false;
	}



	void printStats(GRender& render, const HTTPStats& global, const HTTPStats& stats, uint32_t x) {

		GuiHelper::printIntPercent(render, x, 2, global.latency.getCount(), stats.latency.getCount(), "");

		GuiHelper::printBytesPercent(render, x, 3, global.sizePayload, stats.sizePayload);

		GuiHelper::printIntPercent(render, x, 5, stats.numHeaders, stats.numHttpOK, "");
		GuiHelper::printIntPercent(render, x, 6, stats.numHeaders, stats.numHttpMoved, "");
		GuiHelper::printIntPercent(render, x, 7, stats.numHeaders, stats.numHttpNotModified, "");
		GuiHelper::printIntPercent(render, x, 8, stats.numHeaders, stats.numHttpNotFound, "");
		GuiHelper::printIntPercent(render, x, 9, stats.numHeaders, stats.numHttpIntError, "");
		GuiHelper::printIntPercent(render, x, 10, stats.numHeaders, stats.numHttpOther, "");

		render.printR(x, 12, floatToString(stats.latency.getAvg(), 1) + " ms");
		render.printR(x, 13, floatToString(stats.latency.getStdDev(), 1) + " ms");

		render.printR(x, 15, floatToString(stats.latency.getMin(), 0) + " ms");
		render.printR(x, 16, floatToString(stats.latency.getQuantile(0.25f), 0) + " ms");
		render.printR(x, 17, floatToString(stats.latency.getQuantile(0.50f), 0) + " ms");
		render.printR(x, 18, floatToString(stats.latency.getQuantile(0.75f), 0) + " ms");
		render.printR(x, 19, floatToString(stats.latency.getMax(), 0) + " ms");

	}

	void refresh(GRender& render) override {

		const int s = 24;

		// headers
		render.printR(13+s*1, 1, "Total");
		render.printR(13+s*2, 1, "Static Content");
		render.printR(13+s*3, 1, "Dynamic Content");

		// legend
		render.print(1, 2, "Values:");
		render.print(1, 3, "Traffic:");

		render.print(1, 5, "HTTP 200:");
		render.print(1, 6, "HTTP 302:");
		render.print(1, 7, "HTTP 304:");
		render.print(1, 8, "HTTP 404:");
		render.print(1, 9, "HTTP 500:");
		render.print(1, 10, "HTTP ???:");

		render.print(1, 12, "Average:");
		render.print(1, 13, "StdDev:");

		render.print(1, 15, "Min:");
		render.print(1, 16, "Q 0.25:");
		render.print(1, 17, "Q 0.50:");
		render.print(1, 18, "Q 0.75:");
		render.print(1, 19, "Max:");

		const HTTPContentType& ct = parent.getContentType();
		printStats(render, ct.response, ct.response, 13+s*1);
		printStats(render, ct.response, ct.responseStatic, 13+s*2);
		printStats(render, ct.response, ct.responseDynamic, 13+s*3);

	}
};

class CompressorStatsTab : public GTab {
public:
	CompressorStatsTab(PnlContentType& parent, const std::string& comprName)
: GTab(comprName), parent(parent), comprName(comprName) { }

	bool onKey(int key) override {
		if (key == 'h') {GuiHelper::toggleBytePercentUnit(); return true;}
		return false;
	}

//	void printInt(GRender& render, uint32_t x, uint32_t y, uint64_t total, uint64_t other, const std::string& unit) {
//		uint32_t percent = (total) ? (100 * other / (total)) : (0);
//		std::stringstream ss;
//		ss << " (" << percent << "%)";
//		render.printR(x, y, ss.str());
//		ss.str("");
//		ss << other << unit ;
//		render.printR(x-7, y, ss.str());
//	}

	void printStats(GRender& render, const HTTPStats& global, const HTTPStats& stats, uint32_t x) {

		const auto globalStatsIter = global.comprStats.find(comprName);
		if (globalStatsIter == global.comprStats.end()) {
			return; // no global stats available; should never be the case.
		}

		// fill the "Total" column.
		GuiHelper::printBytesPercent(render, x, 2, global.sizePayload, stats.sizePayload);

		const auto statsStatsIter = stats.comprStats.find(comprName);
		if (statsStatsIter != stats.comprStats.end()) {

			// compression side
			GuiHelper::printBytesPercent(render, x, 4, stats.sizePayload, statsStatsIter->second.compression.compressedPayloadSize);
			GuiHelper::printIntPercent(render, x, 5, globalStatsIter->second.compression.compressedPayloadUs / 1000, statsStatsIter->second.compression.compressedPayloadUs / 1000, " ms");

			render.printR(x, 7, floatToString( statsStatsIter->second.compression.getCompressionSpeed(stats.sizePayload) / 1024 / 1024.0, 1) + " MiB/s" );
			render.printR(x, 8, floatToString( statsStatsIter->second.compression.getReductionSpeed(stats.sizePayload) / 1024 / 1024.0, 1) + " MiB/s" );

			// decompression side
			GuiHelper::printBytesPercent(render, x, 10, stats.sizePayload, statsStatsIter->second.decompression.decompressedPayloadSize);
			GuiHelper::printIntPercent(render, x, 11, globalStatsIter->second.decompression.decompressionUs / 1000, statsStatsIter->second.decompression.decompressionUs / 1000, " ms");

			render.printR(x, 13, floatToString( statsStatsIter->second.decompression.getDecompressionSpeed() / 1024 / 1024.0, 1 ) + " MiB/s" );

		}

	}

	void refresh(GRender& render) override {

		const int s = 24;

		// headers
		render.printR(17+s, 1, "Total Payload");
		render.printR(17+s+s, 1, "Static Payload");
		render.printR(17+s+s+s, 1, "Dynamic Payload");

		// legend
		render.print(1, 2, "Traffic:");

		render.print(1, 4, "Compr. Size:");
		render.print(1, 5, "Compr. Time:");

		render.print(1, 7, "Compr. Speed:");
		render.print(1, 8, "Compr. RedSpeed:");

		render.print(1, 10, "Decompr. Size:");
		render.print(1, 11, "Decompr. Time:");

		render.print(1, 13, "Decompr. Speed:");


		// TODO: fix percent output values.

		const HTTPContentType& ct = parent.getContentType();
		printStats(render, ct.response, ct.response, 17+s);
		printStats(render, ct.response, ct.responseStatic, 17+s+s);
		printStats(render, ct.response, ct.responseDynamic, 17+s+s+s);

	}

private:
	PnlContentType& parent;
	const std::string comprName;

};




PnlContentType::PnlContentType() :
		GPanel(), type(nullptr), tabs() {
	recreateTabs();
}

PnlContentType::~PnlContentType() {
	;
}

const HTTPContentType& PnlContentType::getContentType() {
	return *type;
}

void PnlContentType::setContentType(const HTTPContentType& type) {
	this->type = &type;

	recreateTabs();
}

bool PnlContentType::onKey(int key) {

	if (tabs.onKey(key)) {return true;}

	switch (key) {
	case 27:		delete this; return true; // escape key
	default:		return false;
	}

}

void PnlContentType::refresh(GRender& render) {
	tabs.setBounds(b.x, b.y, b.w, b.h);
	tabs.render(render);
}

void PnlContentType::recreateTabs() {
	tabs.removeTabs();
	tabs.addTab(new TabAnalysis(*this));
	tabs.addTab(new TabWords(*this));
	tabs.addTab(new TabHuffman(*this));

	if (type) {
		// create a tab for each active compression algorithm
		for (const auto& stats : type->response.comprStats) {
			tabs.addTab(new CompressorStatsTab(*this, stats.first));
		}
	}
}
