
#ifndef PORT
#include "../Enthalten/port.h"
#endif

#define _NODE_H

#ifdef _NODE_H

/** @struct Node
 *  @brief Represents a network node.
 *
 *  This structure represents a network node in the network simulation. Each
 * node has a unique identifier to distinguish it from other nodes.
 *
 *  @var Node::identifier
 *  Node identifier, a unique integer value assigned to each node.
 */
struct Node {
  unsigned int identifier;
};

// Create a node
/**
 * @brief Creates a new network node with a unique identifier.
 *
 * This function initializes a `Node` structure with a unique identifier. The
 * identifier is assigned based on the current value of `node_number`, which is
 * then incremented for the next node creation.
 *
 * @param node_number Pointer to the current node number, which will be
 * incremented after assigning the identifier to the new node.
 *
 * @return A `Node` structure initialized with the current node number as its
 * identifier.
 */
struct Node createANode(unsigned int *node_number) {
  struct Node node;

  node.identifier = *node_number;
  *node_number += 1;

  return node;
}
#endif