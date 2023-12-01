# Image-Compressor
### The QTree Class
An QTree is a 4-ary tree whose nodes represent rectangular regions of a PNG image. The project involves representation of images in form of quad trees. Operations such as flipping and roatating of image are implemented using the representation of quad trees.

###  Image compression using QTrees
Implementation of prune fucntion prunes the images with the given tolerance parameter. This function attempts, starting near the top of a freshly built tree, to remove all of the descendants of a node, if all of the leaf nodes below the current node have colour within tolerance of the node's average colour.
