#define LINK
#ifdef LINK


/** @struct Link
 * @brief Represents a network link.
 * 
 * @var Link::identifier
 * Link identifier.
 * @var Link::capacity
 * Link capacity (in kbps).
 * @var Link::head_node
 * Head node of the link.
 * @var Link::tail_node
 * Tail node of the link.
 */
struct Link{
    unsigned int identifier;

    double capacity;
    struct Node head_node;
    struct Node tail_node;
};

#endif