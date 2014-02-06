/*
 * GList.h
 *
 *  Created on: Apr 13, 2013
 *      Author: kazu
 */

#ifndef GLIST_H_
#define GLIST_H_

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <cstring>

#ifdef __CYGWIN__
#include <ncurses/ncurses.h>
#else
#include <ncurses.h>
#endif

#include "GListEntry.h"
#include "GListListener.h"
#include "GColors.h"
#include "GPanel.h"

/**
 * represents one column with the list.
 * it has a header and a width
 */
struct GListColumn {

	/** the name of the column */
	std::string name;

	/** the width of the column */
	uint32_t width;

	/** ctor */
	GListColumn(const std::string& name, uint32_t width) : name(name), width(width) {;}

};


/**
 * provides a list with selectable elements
 */
class GList : public GPanel {

public:

	/** ctor */
	GList() :
		GPanel(), curSelIdx(0) {
	}

	/** dtor */
	~GList() {
		for (const auto& it : entries) {delete it;}
	}



	/** add a new column to the list */
	void addColumn(const std::string& name, uint32_t width) {
		columns.push_back(GListColumn(name, width));
	}

	/** add one entry to the list */
	void addEntry(GListEntry* entry) {
		entries.push_back(entry);
	}

	/** remove all entries from the list */
	void clear() {
		entries.clear();
	}

	/** get the vector of all entries in this list */
	const std::vector<GListEntry*> getEntries() {return entries;}


	/** receive key events */
	bool onKey(int key) {
		switch(key) {
		case KEY_DOWN:	if (curSelIdx < entries.size() - 1) {++curSelIdx;}; return true;
		case KEY_UP:	if (curSelIdx > 0) {--curSelIdx;}; return true;
		case '\n':		informListeners(); return true;
		default:		return false;
		}
	}

	/** pass event to all listeners */
	void informListeners() {
		if (curSelIdx >= entries.size()) {return;}
		for (size_t i = 0; i < listeners.size(); ++i) {
			listeners[i]->onSelectedItem(entries[curSelIdx]);
		}
	}

	/** add one listener to the list */
	void addListener(GListListener* listener) {
		listeners.push_back(listener);
	}

	/** sort the entries in this list */
	void sort(std::function<int(GListEntry*, GListEntry*)> lambda) {
		std::sort(entries.begin(), entries.end(), lambda);
	}

	/** refresh the list */
	void refresh(GRender& render) override {

		// draw box
		render.setBorderSize(0);
		render.drawBox(0, 0, b.w, b.h);
		render.setBorderSize(1);

		// get some sizing information
		const uint32_t numCols = columns.size();
		const uint32_t numEntries = entries.size();

		// init temporal storages
		tmpWidths.resize(numCols);
		tmpValues.resize(numCols*numEntries);
		//std::fill(tmpWidths.begin(), tmpWidths.end(), 20);

		uint32_t cx = 0;

		// fetch all column values into a temporary array
		// (we use this step also to track column widths)
		for (uint32_t row = 0; row < numEntries; ++row) {
			for (uint32_t col = 0; col < numCols; ++col) {
				uint32_t idx = row*numCols+col;
				const std::string& text = entries[row]->getValue(col);
				tmpValues[idx] = text;
				if (text.length() > tmpWidths[col]) {tmpWidths[col] = text.length();}
			}
		}

		// now, print the list's header, and adjust colum's size
		// (if header is longer than any content in this column)
		render.setColors(CIDX_LST_HEADER);
		render.drawHLine(0, 0, b.w, ' ');
		for (uint32_t col = 0; col < numCols; ++col) {
			const std::string& text = columns[col].name;
			render.print(cx, 0, text);
			if (text.length() > tmpWidths[col]) {tmpWidths[col] = text.length();}
			cx += tmpWidths[col] + 1;
		}
		render.setColors(CIDX_WIN_CONTENT);



		// draw grid
		cx = 0;
		for (uint32_t col = 0; col < numCols; ++col) {
			cx += tmpWidths[col];
			render.drawVLine(cx, 1, b.h-1, ACS_VLINE);
			render.printChar(cx, b.h-2, ACS_BTEE);
			cx += 1;
		}

		// now print the (previously) stored values
		for (uint32_t curRow = 0; curRow < numEntries; ++curRow) {

			// draw background behind the selected row
			if (curRow == curSelIdx) {
				render.setColors(CIDX_LST_FOCUSED);
				render.drawHLine(0, curRow+1, b.w, ' ');
			} else {
				render.setColors(CIDX_WIN_CONTENT);
			}

			// display the entry
			cx = 0;
			for (uint32_t col = 0; col < numCols; ++col) {

				uint32_t colWidth = tmpWidths[col] + 1;

				// print each column's value
				uint32_t idx = curRow * numCols + col;
				const std::string& str = tmpValues[idx];
				if (str.length() >= colWidth) {
					render.print(cx, curRow+1, (str.substr(0, colWidth - 3) + ".."));
				} else {
					render.print(cx, curRow+1, str);
				}

				// next column
				cx += colWidth;

			}

			// end of table? -> stop
			// 2 = header + countingFromZero
			if (curRow+2 >= b.h) {break;}

		}


	}


	/** all columns for this list */
	std::vector<GListColumn> columns;

	/** all entries within the list */
	std::vector<GListEntry*> entries;

	/** listeners to inform */
	std::vector<GListListener*> listeners;

	/** index of the currently selected entry */
	uint32_t curSelIdx;


	/** temporal storage of (calculated) column widths */
	std::vector<uint32_t> tmpWidths;

	/** temporal storage of each (row,column) entry */
	std::vector<std::string> tmpValues;

};


#endif /* GLIST_H_ */
