/*
 * Copyright (c) 2007-2008 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#if !defined(NODE_H)
#define NODE_H

#include "dyn_detail/boost/shared_ptr.hpp"
#include <set>
#include <string>
#include "Annotatable.h"

#include "dyntypes.h"


class BPatch_function;
class BPatch_basicBlock;

namespace Dyninst {

class Edge;
class Graph;
class NodeIterator;
class EdgeIterator;

class Node : public AnnotatableSparse {
    friend class Edge;
    friend class Graph;
    
    typedef dyn_detail::boost::shared_ptr<Edge> EdgePtr;
    typedef std::set<EdgePtr> EdgeSet;

 public:
    typedef dyn_detail::boost::shared_ptr<Node> Ptr;

    void ins(EdgeIterator &begin, EdgeIterator &end);
    void outs(EdgeIterator &begin, EdgeIterator &end);

    void ins(NodeIterator &begin, NodeIterator &end);
    void outs(NodeIterator &begin, NodeIterator &end);

    bool hasInEdges(); 
    bool hasOutEdges();
    
    virtual Address addr() const { return INVALID_ADDR; }
    
    virtual std::string format() const = 0;

    //virtual Node::Ptr copyTo(GraphPtr graph) = 0;
    
    virtual bool isVirtual() const = 0;
    
    virtual ~Node() {};

 protected:
    Node() {};

    EdgeSet ins_;
    EdgeSet outs_;
    
    void addInEdge(const EdgePtr in) { ins_.insert(in); }
    void addOutEdge(const EdgePtr out) { outs_.insert(out); }

    static const Address INVALID_ADDR;
};
 
class PhysicalNode : public Node {
    typedef dyn_detail::boost::shared_ptr<PhysicalNode> Ptr;
    
 public:
    static Node::Ptr createNode(Address addr);
    
    virtual Address addr() const { return addr_; }
    
    virtual std::string format() const;
    
    virtual bool isVirtual() const { return false; }
    
    virtual ~PhysicalNode() {};
    
    // virtual Node::Ptr copyTo(GraphPtr graph);

 protected:
    PhysicalNode(Address addr) : addr_(addr) {};
    
    Address addr_; 
};

class VirtualNode : public Node {
    friend class Edge;
    friend class Graph;

 public:
    typedef dyn_detail::boost::shared_ptr<VirtualNode> Ptr;
    
    static Node::Ptr createNode();
    
    virtual std::string format() const;
    //Node::Ptr copyTo(GraphPtr graph);
    
    virtual bool isVirtual() const { return true; }
    
    virtual ~VirtualNode() {};

    VirtualNode() {};

 private:
};

 class NodeIteratorImpl;

class NodeIterator {
    friend class Node;
    friend class Graph;
    friend class Edge;

 public:

    NodeIterator &operator++();
    NodeIterator operator++(int);

    NodeIterator &operator--();
    NodeIterator operator--(int);

    Node::Ptr operator*() const;

    bool operator==(const NodeIterator &rhs) const;
    bool operator!=(const NodeIterator &rhs) const;

    NodeIterator &operator=(const NodeIterator &rhs);


    // For code such as:
    // NodeIterator begin, end;
    // graph->entryNodes(begin, end);
    NodeIterator();

    // Main constructor
    // The iter parameter becomes owned by the iterator and will be destroyed
    // when the iterator is destroyed.
    NodeIterator(NodeIteratorImpl *iter);

    // Aaaand let's _really_ not forget the copy constructor
    NodeIterator(const NodeIterator &rhs);

    ~NodeIterator();

 protected:

    // We hide the internal iteration behavior behind a pointer. 
    // This allows us to override (yay for virtual functions).
    NodeIteratorImpl *iter_;

    NodeIteratorImpl *copy() const;

};

}
#endif
