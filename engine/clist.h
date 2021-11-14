#pragma once
#include <cassert>
#include <cstdint>

namespace tte {
	using namespace std;

	class CList {
		//
		// member variables
		//
	private:
		uint32_t m_tag;
		CList *m_pNext;
		CList *m_pPrev;

		//
		// public methods
		//
	public:
		explicit CList(
			uint32_t tag
		) : m_tag(tag), m_pNext(this), m_pPrev(this) {
		}

		virtual ~CList() {
			detach();
		}

		//CList * linkPeer(CList *rhs) {
		//	assert(rhs);
		//	CList *lhs = this;
		//	lhs->detach();
		//	rhs->detach();
		//	lhs->m_pNext = lhs->m_pPrev = rhs;
		//	rhs->m_pNext = rhs->m_pPrev = lhs;
		//	return lhs;
		//}

		CList * append(CList *rhs) {
			assert(rhs);
			CList *lhs = this;
			rhs->detach();
			rhs->m_pNext = lhs->m_pNext;
			rhs->m_pPrev = lhs;
			lhs->m_pNext->m_pPrev = rhs;
			lhs->m_pNext = rhs;
			return lhs;
		}

		template<typename V>
		V * get() {
			for (CList *p = next(); p != end(); p = p->next()) {
				if (p->tag() == V::tag) {
					return reinterpret_cast<V *>(p);
				}
			}
			return nullptr;
		}

		//
		// property methods
		//
	public:
		const uint32_t & tag() const {
			return m_tag;
		}

		//
		// utility methods
		//
	public:
		CList * next() const {
			return m_pNext;
		}

		const CList * end() const {
			return this;
		}

		//
		// others
		//
	protected:
		static constexpr uint32_t makeTag(const char tagName[5]) {
			return (tagName[0] << 0) | (tagName[1] << 8) | (tagName[2] << 16) | (tagName[3] << 24);
		}

	private:
		void detach() {
			m_pPrev->m_pNext = m_pNext;
			m_pNext->m_pPrev = m_pPrev;
			m_pPrev = m_pNext = this;
		}
	};
}
