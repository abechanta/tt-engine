#pragma once
#include <cassert>
#include <cstdint>
#include <boost/iterator_adaptors.hpp>

namespace tte {
	using namespace boost;
	using namespace std;

	template<typename Node>
	class MTree {
		//
		// member variables
		//
	private:
		Node *m_pParent;
		Node *m_pSibling;
		Node *m_pChildren;

		//
		// public methods
		//
	public:
		MTree() : m_pParent(nullptr), m_pSibling(nullptr), m_pChildren(nullptr) {
		}

		virtual ~MTree() {
			detach(self());
			while (m_pChildren) {
				delete m_pChildren;
			}
			assert(!m_pChildren);
		}

		Node * appendChild(Node *p) {
			assert(p);
			assert(!p->m_pParent && !p->m_pSibling);
			p->m_pParent = self();
			*findRight(&m_pChildren, nullptr) = p;
			return self();
		}

		Node * prependChild(Node *p) {
			assert(p);
			assert(!p->m_pParent && !p->m_pSibling);
			p->m_pParent = self();
			p->m_pSibling = m_pChildren;
			m_pChildren = p;
			return self();
		}

		//
		// downcast operators
		//
	private:
		Node * self() {
			return reinterpret_cast<Node *>(this);
		}

		const Node * self() const {
			return reinterpret_cast<const Node *>(this);
		}

		//
		// iterators
		//
	public:
		struct iterator : iterator_adaptor<iterator, Node *, Node, forward_traversal_tag> {
			iterator(Node *p, bool bSkipSelf = false) : iterator_adaptor<iterator, Node *, Node, forward_traversal_tag>(p) {
				if (bSkipSelf) {
					increment();
				}
			}

			void increment() {
				base_reference() = next(base());
			}
		};

		iterator begin(bool bSkipSelf = false) {
			return iterator(self(), bSkipSelf);
		}

		iterator end() {
			return iterator(nextUpward(self()));
		}

		struct const_iterator : iterator_adaptor<const_iterator, const Node *, Node, forward_traversal_tag> {
			const_iterator(const Node *p, bool bSkipSelf = false) : iterator_adaptor<const_iterator, const Node *, Node, forward_traversal_tag>(p) {
				if (bSkipSelf) {
					increment();
				}
			}

			void increment() {
				base_reference() = next(base());
			}
		};

		const_iterator begin(bool bSkipSelf = false) const {
			return const_iterator(self(), bSkipSelf);
		}

		const_iterator end() const {
			return const_iterator(nextUpward(self()));
		}

		struct children_iterator : iterator_adaptor<children_iterator, Node *, Node, forward_traversal_tag> {
			children_iterator(Node *p) : iterator_adaptor<children_iterator, Node *, Node, forward_traversal_tag>(p) {
			}

			void increment() {
				base_reference() = nextSibling(base());
			}
		};

		children_iterator begin_children() {
			return children_iterator(m_pChildren ? m_pChildren : self());
		}

		children_iterator end_children() {
			return children_iterator(self());
		}

		struct const_children_iterator : iterator_adaptor<const_children_iterator, const Node *, Node, forward_traversal_tag> {
			const_children_iterator(const Node *p) : iterator_adaptor<const_children_iterator, const Node *, Node, forward_traversal_tag>(p) {
			}

			void increment() {
				base_reference() = nextSibling(base());
			}
		};

		const_children_iterator begin_children() const {
			return const_children_iterator(m_pChildren ? m_pChildren : self());
		}

		const_children_iterator end_children() const {
			return const_children_iterator(self());
		}

		//
		// others
		//
	private:
		static Node * next(const Node *p) {
			assert(p);
			return p->m_pChildren ? p->m_pChildren : nextUpward(p);
		}

		static Node * nextUpward(const Node *p) {
			assert(p);
			for (; !p->m_pSibling; p = p->m_pParent) {
				if (!p->m_pParent) {
					return nullptr;
				}
			}
			return p->m_pSibling;
		}

		static Node ** findRight(Node **pp, Node *t) {
			assert(pp);
			for (; *pp != t; pp = &(*pp)->m_pSibling) {
				;
			}
			return pp;
		}

		static void detach(Node *p) {
			assert(p);
			if (p->m_pParent) {
				*findRight(&p->m_pParent->m_pChildren, p) = p->m_pSibling;
				p->m_pParent = nullptr;
			}
			p->m_pSibling = nullptr;
		}

		static Node * nextSibling(const Node *p) {
			return p->m_pSibling ? p->m_pSibling : p->m_pParent;
		}
	};
}
