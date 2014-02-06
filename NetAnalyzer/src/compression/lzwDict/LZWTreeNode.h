/*
 * LZWTreeNode.h
 *
 *  Created on: May 26, 2013
 *      Author: kazu
 */

#ifndef LZWTREENODE_H_
#define LZWTREENODE_H_

struct LZWTreeNode {

	/** pointer for each possible char */
	LZWTreeNode* nodes[256];

	/** the dictionary (compressed) index of this node */
	uint32_t idx;

	/** ctor */
	LZWTreeNode() : nodes(), idx(0) {;}

	/** dtor */
	~LZWTreeNode() {
		free();
	}

private:

	/** hidden copy ctor */
	LZWTreeNode(const LZWTreeNode&);

	/** recursively delete all nodes */
	void free() {
		for (uint32_t i = 0; i < 256; ++i) {
			if (!nodes[i]) {continue;}
			nodes[i]->free();
			delete nodes[i];
			nodes[i] = nullptr;
		}
	}

};


#endif /* LZWTREENODE_H_ */
