/**
 * @file qtree.cpp
 * @description student implementation of QTree class used for storing image data
 *              CPSC 221 PA3
 *
 *              SUBMIT THIS FILE
 */

#include "qtree.h"

/**
 * Constructor that builds a QTree out of the given PNG.
 * Every leaf in the tree corresponds to a pixel in the PNG.
 * Every non-leaf node corresponds to a rectangle of pixels
 * in the original PNG, represented by an (x,y) pair for the
 * upper left corner of the rectangle and an (x,y) pair for
 * lower right corner of the rectangle. In addition, the Node
 * stores a pixel representing the average color over the
 * rectangle.
 *
 * The average color for each node in your implementation MUST
 * be determined in constant time. HINT: this will lead to nodes
 * at shallower levels of the tree to accumulate some error in their
 * average color value, but we will accept this consequence in
 * exchange for faster tree construction.
 * Note that we will be looking for specific color values in our
 * autograder, so if you instead perform a slow but accurate
 * average color computation, you will likely fail the test cases!
 *
 * Every node's children correspond to a partition of the
 * node's rectangle into (up to) four smaller rectangles. The node's
 * rectangle is split evenly (or as close to evenly as possible)
 * along both horizontal and vertical axes. If an even split along
 * the vertical axis is not possible, the extra line will be included
 * in the left side; If an even split along the horizontal axis is not
 * possible, the extra line will be included in the upper side.
 * If a single-pixel-wide rectangle needs to be split, the NE and SE children
 * will be null; likewise if a single-pixel-tall rectangle needs to be split,
 * the SW and SE children will be null.
 *
 * In this way, each of the children's rectangles together will have coordinates
 * that when combined, completely cover the original rectangle's image
 * region and do not overlap.
 */
QTree::QTree(const PNG &imIn)
{
	// ADD YOUR IMPLEMENTATION BELOW

	// Initialize private member variables
	height = imIn.height();
	width = imIn.width();
	root = BuildNode(imIn, pair<unsigned int, unsigned int>(0, 0),
					 pair<unsigned int, unsigned int>(width - 1, height - 1));
}

/**
 * Overloaded assignment operator for QTrees.
 * Part of the Big Three that we must define because the class
 * allocates dynamic memory. This depends on your implementation
 * of the copy and clear funtions.
 *
 * @param rhs The right hand side of the assignment statement.
 */
QTree &QTree::operator=(const QTree &rhs)
{
	// ADD YOUR IMPLEMENTATION BELOW
	// Free data associated with current tree first
	if (this != &rhs)
	{
		Clear();
		Copy(rhs);
	}
	return *this;
}

/**
 * Render returns a PNG image consisting of the pixels
 * stored in the tree. may be used on pruned trees. Draws
 * every leaf node's rectangle onto a PNG canvas using the
 * average color stored in the node.
 *
 * For up-scaled images, no color interpolation will be done;
 * each rectangle is fully rendered into a larger rectangular region.
 *
 * @param scale multiplier for each horizontal/vertical dimension
 * @pre scale > 0
 */
PNG QTree::Render(unsigned int scale) const
{
	// Replace the line below with your implementation
	PNG output = PNG(width * scale, height * scale);
	RenderNode(output, root, scale);
	return output;
}

/**
 *  Prune function trims subtrees as high as possible in the tree.
 *  A subtree is pruned (cleared) if all of the subtree's leaves are within
 *  tolerance of the average color stored in the root of the subtree.
 *  NOTE - you may use the distanceTo function found in RGBAPixel.h
 *  Pruning criteria should be evaluated on the original tree, not
 *  on any pruned subtree. (we only expect that trees would be pruned once.)
 *
 * You may want a recursive helper function for this one.
 *
 * @param tolerance maximum RGBA distance to qualify for pruning
 * @pre this tree has not previously been pruned, nor is copied from a previously pruned tree.
 */
void QTree::Prune(double tolerance)
{
	// ADD YOUR IMPLEMENTATION BELOW
	PruneNode(root, tolerance, root->avg);
}

/**
 *  FlipHorizontal rearranges the contents of the tree, so that
 *  its rendered image will appear mirrored across a vertical axis.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  After flipping, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel wide rectangles have
 *  null eastern children
 *  (i.e. after flipping, a node's NW and SW pointers may be null, but
 *  have non-null NE and SE)
 *
 *  You may want a recursive helper function for this one.
 */
void QTree::FlipHorizontal()
{
	// ADD YOUR IMPLEMENTATION BELOW
	root = FlipHorizontalNode(root, root->upLeft, root->lowRight);
}

/**
 *  RotateCCW rearranges the contents of the tree, so that its
 *  rendered image will appear rotated by 90 degrees counter-clockwise.
 *  This may be called on a previously pruned/flipped/rotated tree.
 *
 *  Note that this may alter the dimensions of the rendered image, relative
 *  to its original dimensions.
 *
 *  After rotation, the NW/NE/SW/SE pointers must map to what will be
 *  physically rendered in the respective NW/NE/SW/SE corners, but it
 *  is no longer necessary to ensure that 1-pixel tall or wide rectangles
 *  have null eastern or southern children
 *  (i.e. after rotation, a node's NW and NE pointers may be null, but have
 *  non-null SW and SE, or it may have null NW/SW but non-null NE/SE)
 *
 *  You may want a recursive helper function for this one.
 */
void QTree::RotateCCW()
{
	// Update width and height of QTree
	

	// ADD YOUR IMPLEMENTATION BELOW
	RotateCCWNode(root,width,height);
	unsigned int temp_height = height;
	height = width;
	width = temp_height;

}

/**
 * Destroys all dynamically allocated memory associated with the
 * current QTree object. Complete for PA3.
 * You may want a recursive helper function for this one.
 */
void QTree::Clear()
{
	// ADD YOUR IMPLEMENTATION BELOW
	ClearNode(root);
}

/**
 * Copies the parameter other QTree into the current QTree.
 * Does not free any memory. Called by copy constructor and operator=.
 * You may want a recursive helper function for this one.
 * @param other The QTree to be copied.
 */
void QTree::Copy(const QTree &other)
{
	// ADD YOUR IMPLEMENTATION BELOW
	Node *other_root = other.root;
	root = new Node(other_root->upLeft, other_root->lowRight, other_root->avg);
	root->NW = CopyNode(other_root->NW);
	root->NE = CopyNode(other_root->NE);
	root->SW = CopyNode(other_root->SW);
	root->SE = CopyNode(other_root->SE);
}

/**
 * Private helper function for the constructor. Recursively builds
 * the tree according to the specification of the constructor.
 * @param img reference to the original input image.
 * @param ul upper left point of current node's rectangle.
 * @param lr lower right point of current node's rectangle.
 */
Node *QTree::BuildNode(const PNG &img, pair<unsigned int, unsigned int> ul, pair<unsigned int, unsigned int> lr)
{
	int width_img = lr.first - ul.first + 1;	// number of pixels in the image (width)
	int height_img = lr.second - ul.second + 1; // number of pixles in the image (height)

	RGBAPixel *pixel = img.getPixel(ul.first, ul.second);
	Node *subroot = new Node(ul, lr, *pixel);
	pair<unsigned int, unsigned int> ul_nw, lr_nw, ul_ne, lr_ne, ul_sw, lr_sw, ul_se, lr_se;

	if (width_img == 1 && height_img == 1)
	{
		// Case 1
		// leaf node is a single pixel
		// when ul and lr passed in build node is the same point
		// ul = lr
		RGBAPixel *pixel = img.getPixel(ul.first, ul.second);
		subroot->avg = *pixel;
		subroot->NW = NULL;
		subroot->NE = NULL;
		subroot->SW = NULL;
		subroot->SE = NULL;
		return subroot;
	}
	else if (width_img == 1)
	{
		if (height_img % 2 == 1)
		{
			// Case 2.1
			// width = 1 and height is odd
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair(ul.first, (ul.second + (height_img - 1) / 2));

			ul_sw = make_pair(ul.first, (ul.second + (height_img - 1) / 2 + 1));
			lr_sw = make_pair(lr.first, lr.second);
		}
		else if (height_img % 2 == 0)
		{
			// Case 2.2
			// width = 1 and height is even
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair(ul.first, (ul.second + (height_img) / 2 - 1));

			ul_sw = make_pair(ul.first, (ul.second + (height_img) / 2));
			lr_sw = make_pair(lr.first, lr.second);
		}
		subroot->NW = BuildNode(img, ul_nw, lr_nw);
		subroot->NE = NULL;
		subroot->SW = BuildNode(img, ul_sw, lr_sw);
		subroot->SE = NULL;
	}
	else if (height_img == 1)
	{
		if (width_img % 2 == 1)
		{
			// Case 3.1
			// height = 1 and width is odd
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair((ul.first + ((width_img - 1) / 2)), ul.second);

			ul_ne = make_pair(ul.first + 1 + ((width_img - 1) / 2), ul.second);
			lr_ne = make_pair(lr.first, lr.second);
		}
		else if (width_img % 2 == 0)
		{
			// Case 3.2
			// height = 1 and width is even
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair(ul.first + (width_img / 2) - 1, ul.second);

			ul_ne = make_pair(ul.first + (width_img / 2), ul.second);
			lr_ne = make_pair(lr.first, lr.second);
		}
		subroot->NW = BuildNode(img, ul_nw, lr_nw);
		subroot->NE = BuildNode(img, ul_ne, lr_ne);
		subroot->SW = NULL;
		subroot->SE = NULL;
	}
	else if (width_img % 2 == 0)
	{
		if (height_img % 2 == 0)
		{
			// Case 4.1
			// width is even, height is even
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair((ul.first + width_img / 2 - 1), (ul.second + height_img / 2 - 1));

			ul_ne = make_pair((ul.first + width_img / 2), ul.second);
			lr_ne = make_pair(lr.first, ul.second + height_img / 2 - 1);

			ul_sw = make_pair(ul.first, ul.second + height_img / 2);
			lr_sw = make_pair(ul.first + width_img / 2 - 1, lr.second);

			ul_se = make_pair(ul.first + width_img / 2, ul.second + height_img / 2);
			lr_se = make_pair(lr.first, lr.second);
		}
		else if (height_img % 2 == 1)
		{
			// Case 4.2
			// width is even, height is odd
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair(ul.first + width_img / 2 - 1, ul.second + (height_img - 1) / 2);

			ul_ne = make_pair(ul.first + width_img / 2, ul.second);
			lr_ne = make_pair(lr.first, ul.second + (height_img - 1) / 2);

			ul_sw = make_pair(ul.first, ul.second + (height_img + 1) / 2);
			lr_sw = make_pair(ul.first + width_img / 2 - 1, lr.second);

			ul_se = make_pair(ul.first + width_img / 2, ul.second + (height_img + 1) / 2);
			lr_se = make_pair(lr.first, lr.second);
		}
		subroot->NW = BuildNode(img, ul_nw, lr_nw);
		subroot->NE = BuildNode(img, ul_ne, lr_ne);
		subroot->SW = BuildNode(img, ul_sw, lr_sw);
		subroot->SE = BuildNode(img, ul_se, lr_se);
	}
	else if (width_img % 2 == 1)
	{
		if (height_img % 2 == 0)
		{
			// Case 5.1
			// width is odd, height is even
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair(ul.first + (width_img - 1) / 2, ul.second + height_img / 2 - 1);

			ul_ne = make_pair(ul.first + (width_img + 1) / 2, ul.second);
			lr_ne = make_pair(lr.first, ul.second + height_img / 2 - 1);

			ul_sw = make_pair(ul.first, ul.second + height_img / 2);
			lr_sw = make_pair(ul.first + (width_img - 1) / 2, lr.second);

			ul_se = make_pair(ul.first + (width_img + 1) / 2, ul.second + height_img / 2);
			lr_se = make_pair(lr.first, lr.second);
		}
		else if (height_img % 2 == 1)
		{
			// Case 5.2
			// width is odd, height is odd
			ul_nw = make_pair(ul.first, ul.second);
			lr_nw = make_pair(ul.first + (width_img - 1) / 2, ul.second + (height_img - 1) / 2);

			ul_ne = make_pair(ul.first + (width_img + 1) / 2, ul.second);
			lr_ne = make_pair(lr.first, ul.second + (height_img - 1) / 2);

			ul_sw = make_pair(ul.first, ul.second + (height_img + 1) / 2);
			lr_sw = make_pair(ul.first + (width_img - 1) / 2, lr.second);

			ul_se = make_pair(ul.first + (width_img + 1) / 2, ul.second + (height_img + 1) / 2);
			lr_se = make_pair(lr.first, lr.second);
		}
		subroot->NW = BuildNode(img, ul_nw, lr_nw);
		subroot->NE = BuildNode(img, ul_ne, lr_ne);
		subroot->SW = BuildNode(img, ul_sw, lr_sw);
		subroot->SE = BuildNode(img, ul_se, lr_se);
	}
	subroot->avg = calculateAvg(subroot->NW, subroot->NE, subroot->SW, subroot->SE);
	return subroot;
}

/*********************************************************/
/*** IMPLEMENT YOUR OWN PRIVATE MEMBER FUNCTIONS BELOW ***/
/*********************************************************/

RGBAPixel QTree::calculateAvg(Node *NW, Node *NE, Node *SW, Node *SE)
{
	// Need to include alpha (opacity) of pixel
	// Add up avg colors, find dimension of area covered by doing ul * lr, and divide by it
	double NW_Area = 0, NE_Area = 0, SW_Area = 0, SE_Area = 0;
	double nw_avgr = 0, nw_avgg = 0, nw_avgb = 0, nw_avga = 0;
	double ne_avgr = 0, ne_avgg = 0, ne_avgb = 0, ne_avga = 0;
	double sw_avgr = 0, sw_avgg = 0, sw_avgb = 0, sw_avga = 0;
	double se_avgr = 0, se_avgg = 0, se_avgb = 0, se_avga = 0;

	if (NW != NULL)
	{
		NW_Area = ((NW->lowRight.first - NW->upLeft.first) + 1) * ((NW->lowRight.second - NW->upLeft.second) + 1);
		nw_avgr = NW->avg.r * NW_Area;
		nw_avgg = NW->avg.g * NW_Area;
		nw_avgb = NW->avg.b * NW_Area;
		nw_avga = NW->avg.a * NW_Area;
	}
	if (NE != NULL)
	{
		NE_Area = ((NE->lowRight.first - NE->upLeft.first) + 1) * ((NE->lowRight.second - NE->upLeft.second) + 1);
		ne_avgr = NE->avg.r * NE_Area;
		ne_avgg = NE->avg.g * NE_Area;
		ne_avgb = NE->avg.b * NE_Area;
		ne_avga = NE->avg.a * NE_Area;
	}
	if (SW != NULL)
	{
		SW_Area = ((SW->lowRight.first - SW->upLeft.first) + 1) * ((SW->lowRight.second - SW->upLeft.second) + 1);
		sw_avgr = SW->avg.r * SW_Area;
		sw_avgg = SW->avg.g * SW_Area;
		sw_avgb = SW->avg.b * SW_Area;
		sw_avga = SW->avg.a * SW_Area;
	}
	if (SE != NULL)
	{
		SE_Area = ((SE->lowRight.first - SE->upLeft.first) + 1) * ((SE->lowRight.second - SE->upLeft.second) + 1);
		se_avgr = SE->avg.r * SE_Area;
		se_avgg = SE->avg.g * SE_Area;
		se_avgb = SE->avg.b * SE_Area;
		se_avga = SE->avg.a * SE_Area;
	}

	unsigned int totalArea = NW_Area + NE_Area + SW_Area + SE_Area;

	double average_r = ((nw_avgr + ne_avgr + sw_avgr + se_avgr) / totalArea);

	double average_g = ((nw_avgg + ne_avgg + sw_avgg + se_avgg) / totalArea);

	double average_b = ((nw_avgb + ne_avgb + sw_avgb + se_avgb) / totalArea);

	double average_a = ((nw_avga + ne_avga + sw_avga + se_avga) / totalArea);

	RGBAPixel pixel(average_r, average_g, average_b, average_a);

	return pixel;
}

void QTree::RenderNode(PNG &img, Node *subroot, unsigned int scale) const
{
	if (subroot == NULL)
	{
		return;
	}
	if (subroot->NW == NULL && subroot->NE == NULL && subroot->SW == NULL && subroot->SE == NULL)
	{
		for (unsigned int x = subroot->upLeft.first * scale; x <= ((subroot->lowRight.first * scale) + scale) - 1; x++)
		{
			for (unsigned int y = subroot->upLeft.second * scale; y <= ((subroot->lowRight.second * scale) + scale) - 1; y++)
			{
				RGBAPixel *pixel = img.getPixel(x, y);
				*pixel = subroot->avg;
			}
		}
	}

	RenderNode(img, subroot->NW, scale);
	RenderNode(img, subroot->NE, scale);
	RenderNode(img, subroot->SW, scale);
	RenderNode(img, subroot->SE, scale);
}

void QTree::ClearNode(Node *subroot)
{
	if (subroot == NULL)
	{
		return;
	}
	if (subroot->NW == NULL && subroot->NE == NULL && subroot->SW == NULL && subroot->SE == NULL)
	{
		delete subroot;
		return;
	}

	ClearNode(subroot->NW);
	ClearNode(subroot->NE);
	ClearNode(subroot->SW);
	ClearNode(subroot->SE);

	delete subroot;
	subroot = 0;
}



Node *QTree::CopyNode(Node *toCopy)
{
	if (toCopy == NULL)
	{
		return NULL;
	}

	Node *subroot = new Node(toCopy->upLeft, toCopy->lowRight, toCopy->avg);
	subroot->NW = CopyNode(toCopy->NW);
	subroot->NE = CopyNode(toCopy->NE);
	subroot->SW = CopyNode(toCopy->SW);
	subroot->SE = CopyNode(toCopy->SE);

	return subroot;
}

Node *QTree::FlipHorizontalNode(Node *subroot, pair<unsigned int, unsigned int> parent_ul, pair<unsigned int, unsigned int> parent_lr)
{

	// Step 1 : just change the coordinates(ul and lr) of nw, ne,sw and se according to the flipped image
	// Step 2 : find the new coordinates -  no chnage is made in the if statements
	// Step 3 : recursive calls to the children with the new coordinates
	if (subroot == NULL)
	{
		return NULL;
	}

	subroot->upLeft = parent_ul;
	subroot->lowRight = parent_lr;

	pair<unsigned int, unsigned int> nw_ul, nw_lr, ne_ul, ne_lr, sw_ul, sw_lr, se_ul, se_lr;

	if (subroot->NE != NULL)
	{
		// update the ul and lr for NW
		nw_ul.first = parent_ul.first;
		nw_ul.second = parent_ul.second;
		nw_lr.first = parent_ul.first + subroot->NE->lowRight.first - subroot->NE->upLeft.first;
		nw_lr.second = subroot->NE->lowRight.second;
	}
	if (subroot->NW != NULL)
	{
		// update the ul and lr for NE
		unsigned int ne_width = 0;
		if (subroot->NE != NULL)
		{
			ne_width = subroot->NE->lowRight.first - subroot->NE->upLeft.first + 1;
		}
		ne_ul.first = parent_ul.first + ne_width;
		ne_ul.second = parent_ul.second;
		ne_lr.first = parent_lr.first;
		ne_lr.second = subroot->NW->lowRight.second;
	}
	if (subroot->SE != NULL)
	{
		sw_ul.first = parent_ul.first;
		sw_ul.second = subroot->SE->upLeft.second;
		sw_lr.first = parent_ul.first + subroot->SE->lowRight.first - subroot->SE->upLeft.first;
		sw_lr.second = subroot->SE->lowRight.second;
	}
	if (subroot->SW != NULL)
	{
		unsigned int se_width = 0;
		if (subroot->SE != NULL)
		{
			se_width = subroot->SE->lowRight.first - subroot->SE->upLeft.first + 1;
		}
		se_ul.first = parent_ul.first + se_width;
		se_ul.second = subroot->SW->upLeft.second;
		se_lr.first = parent_lr.first;
		se_lr.second = parent_lr.second;
	}

	Node *old_nw = subroot->NW;
	Node *old_ne = subroot->NE;
	Node *old_sw = subroot->SW;
	Node *old_se = subroot->SE;

	subroot->NW = FlipHorizontalNode(old_ne, nw_ul, nw_lr);
	subroot->NE = FlipHorizontalNode(old_nw, ne_ul, ne_lr);
	subroot->SW = FlipHorizontalNode(old_se, sw_ul, sw_lr);
	subroot->SE = FlipHorizontalNode(old_sw, se_ul, se_lr);

	return subroot;
}

void QTree::PruneNode(Node *subroot, double tol, RGBAPixel p)
{
	if (subroot == NULL)
	{
		return;
	}
	else
	{
		if (toleranceLeaves(subroot, p, tol))
		{
			ClearNode(subroot->NW);
			ClearNode(subroot->NE);
			ClearNode(subroot->SW);
			ClearNode(subroot->SE);
			subroot->NW = nullptr;
			subroot->NE = nullptr;
			subroot->SW = nullptr;
			subroot->SE = nullptr;
			return;
		}
		PruneNode(subroot->NW, tol, subroot->avg);
		PruneNode(subroot->NE, tol, subroot->avg);
		PruneNode(subroot->SW, tol, subroot->avg);
		PruneNode(subroot->SE, tol, subroot->avg);
	}
}

bool QTree::toleranceLeaves(Node *subroot, RGBAPixel pixel, double tol)
{
	// Step 1 : find the leaves
	// Step 2 : find an array with checking the tol value
	// Step 3 : if the size of leaves is equal to the size of the leaves array then return true
	if (subroot != NULL)
	{
		vector<Node *> nodeArray = findLeaves(subroot);
		if (nodeArray.size() == 0)
		{
			return false;
		}
		vector<Node *> nn;
		for (int i = 0; i < nodeArray.size(); i++)
		{
			if (nodeArray[i]->avg.distanceTo(subroot->avg) <= tol)
			{
				nn.push_back(nodeArray[i]);
			}
		}
		return nn.size() == nodeArray.size();
	}
	return false;
}

vector<Node *> QTree::findLeaves(Node *subroot)
{
	vector<Node *> nodeArray;
	if (subroot != NULL)
	{
		if ((subroot->upLeft == subroot->lowRight) && (subroot->NW == NULL) && (subroot->NE == NULL) && (subroot->SW == NULL) && (subroot->SE == NULL))
		{
			nodeArray.push_back(subroot);
		}
		else
		{
			vector<Node *> children = {subroot->NW, subroot->NE, subroot->SW, subroot->SE};
			for (int i = 0; i < children.size(); i++)
			{
				vector<Node *> leaves = findLeaves(children[i]);
				nodeArray.insert(nodeArray.end(), leaves.begin(), leaves.end());
			}
		}
	}
	return nodeArray;
}
void QTree::RotateCCWNode(Node* node, int node_width, int node_height) {
    if (node == nullptr) {
        return; 
    }
    Node* temp = node->NW;
    node->NW = node->NE;
    node->NE = node->SE;
    node->SE = node->SW;
    node->SW = temp;

    int left_ul = node->upLeft.second;
    int left_lr = node_width - node->lowRight.first - 1;
    int right_lr = left_ul + (node->lowRight.second - node->upLeft.second);
    int right_ul = left_lr + (node->lowRight.first - node->upLeft.first);

    node->upLeft = make_pair(left_ul, left_lr);
    node->lowRight = make_pair(right_lr, right_ul);

    int new_width = node_height;
    int new_height = node_width;
    RotateCCWNode(node->NW, new_height, new_width);
    RotateCCWNode(node->NE, new_height, new_width);
    RotateCCWNode(node->SE, new_height, new_width);
    RotateCCWNode(node->SW, new_height, new_width);
}

