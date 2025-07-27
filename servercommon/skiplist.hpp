#ifndef __SKIP_LIST_HPP__
#define __SKIP_LIST_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <assert.h>
#include<math.h>

static const int MAX_SKIP_LIST_LEVEL_COUNT = 32;
static const unsigned int MAX_UINT = 0xffffffff;

// 该数据结构所有逻辑皆是为了排行榜需求所做
// 非排行榜需求需要用到该数据结构的时候 , 请先查看对应逻辑以避免不必要的风险

template<typename t_name>
class SkipList
{
private:
	struct Node
	{
		Node() : m_span(0), m_up_node(NULL),  m_down_node(NULL), m_left_node(NULL), m_right_node(NULL) {}
		Node(t_name _data) :m_data(_data), m_span(0), m_up_node(NULL), m_down_node(NULL), m_left_node(NULL), m_right_node(NULL) {}
		bool IsHead()
		{
			if (NULL == m_left_node && NULL != m_right_node)
				return true;
			return false;
		}
		bool IsTail()
		{
			if (NULL == m_right_node && NULL != m_left_node)
				return true;
			return false;
		}
		int GetSpan()
		{
			return m_span;
		}

		t_name m_data;				// 主要数据

		unsigned int m_span;		// 当前层级该节点到前节点的之间的跨度

		Node * m_up_node;			// 上指针
		Node * m_down_node;			// 下指针
		Node * m_left_node;			// 左指针
		Node * m_right_node;		// 右指针
	};
public:
	class Iterator
	{
	public:
		Iterator(Node* head = NULL, Node* tail = NULL, Node* node = NULL)
			:m_head(head), m_tail(tail), m_node(node) {}
		bool operator== (Iterator const& it) const
		{
			return m_node == it.m_node;
		}
		bool operator!= (Iterator const& it) const
		{
			return !(*this == it);
		}
		Iterator& operator++ ()
		{
			if (m_node)
				m_node = m_node->m_right_node;
			else
				m_node = m_head;
			return *this;
		}
		Iterator const operator++ (int)
		{
			Iterator it = *this;
			++*this;
			return it;
		}
		Iterator& operator-- ()
		{
			if (m_node)
				m_node = m_node->m_left_node;
			else
				m_node = m_tail;
			return *this;
		}
		Iterator const operator-- (int)
		{
			Iterator it = *this;
			--*this;
			return it;
		}
		t_name& operator* () const
		{
			return m_node->m_data;
		}
		t_name* operator-> () const
		{
			return &**this;
		}

	private:
		Node* m_head;
		Node* m_tail;
		Node* m_node;
		friend class SkipList;
	};
	// 反向迭代器
	class Reverse_Iterator
	{
	public:
		Reverse_Iterator(Node* head = NULL, Node* tail = NULL, Node* node = NULL)
			:m_head(head), m_tail(tail), m_node(node) {}
		bool operator== (Reverse_Iterator const& it) const
		{
			return m_node == it.m_node;
		}
		bool operator!= (Reverse_Iterator const& it) const
		{
			return !(*this == it);
		}
		Reverse_Iterator& operator++ ()
		{
			if (m_node)
				m_node = m_node->m_left_node;
			else
				m_node = m_tail;
			return *this;
		}
		Reverse_Iterator const operator++ (int)
		{
			Reverse_Iterator it = *this;
			++*this;
			return it;
		}
		Reverse_Iterator& operator-- ()
		{
			if (m_node)
				m_node = m_node->m_right_node;
			else
				m_node = m_head;
			return *this;
		}
		Reverse_Iterator const operator-- (int)
		{
			Iterator it = *this;
			--*this;
			return it;
		}
		t_name& operator* () const
		{
			return m_node->m_data;
		}
		t_name* operator-> () const
		{
			return &**this;
		}

	private:
		Node* m_head;
		Node* m_tail;
		Node* m_node;
		friend class SkipList;
	};
public:
	// 获取起始迭代器
	Iterator Begin(void)
	{
		return Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_head)->m_right_node);
	}
	// 获取终止迭代器
	Iterator End(void)
	{
		return Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_tail));
	}
	// 获取反向起始迭代器
	Reverse_Iterator RBegin(void)
	{
		return  Reverse_Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_tail)->m_left_node);
	}
	// 获取反向终止迭代器
	Reverse_Iterator REnd(void)
	{
		return Reverse_Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_head));
	}

	// 查找某个元素
	Iterator Find(t_name const & _data)
	{
		Node * p = this->Search(_data);
		if (NULL == p)
		{
			return this->End();
		}

		return Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), p);
	}
	// 根据索引获取对应元素所在的底层节点迭代器
	Iterator operator[] (unsigned int index)
	{
		if (index >= m_size || this->IsEmpty())
		{
			return this->End();
		}

		// span 是从1开始的
		unsigned int target_index = index + 1;

		int temp_total_span = 0;
		Node * node = m_head;
		while (true)
		{
			while (!node->m_right_node->IsTail() && temp_total_span + node->m_right_node->m_span <= target_index)
			{
				node = node->m_right_node;
				temp_total_span += node->m_span;
			}
			if (NULL == node->m_down_node)
			{
				break;
			}
			node = node->m_down_node;
		}
		return  Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), node);
	}

	// 插入某个元素
	std::pair<Iterator, bool> Insert(const t_name  & _data)
	{
		// 0 == m_default_max_size 则表示不限制最大数值 , 则可以插入最多2^64的数据量 
		if ((m_size >= m_default_max_size && 0 != m_default_max_size )|| m_size >= MAX_UINT)
		{
			return std::pair<Iterator, bool>(this->End(), false);
		}

		Node * left_node = this->FindNode(_data);
		//如果data相同，直接返回 false
		if (!left_node->IsHead() && left_node->m_data == _data)
		{
			return std::pair<Iterator, bool>(Iterator(m_head, m_tail, left_node), false);
		}

		Node * new_node = new Node(_data);
		++m_size;
		this->AppendNode(left_node, new_node);
		new_node->m_span = 1;
		Node * return_node = new_node;

		//随机决定结点是否“晋升” 
		int curr_level = 0;
		int temp_span = 1;
		while (rand() % 2 == 1 && curr_level < m_max_limit_level) // 最大32层够 2^64数据量了 ,
		{
			//如果当前层已经是最高层，需要增加一层
			if (curr_level == m_max_level)
			{
				this->AddLevel();
			}
			//找到上一层的前置节点
			while (NULL == left_node->m_up_node)
			{
				temp_span += left_node->m_span;
				left_node = left_node->m_left_node;
			}
			assert(NULL != left_node);

			left_node = left_node->m_up_node;
			//把“晋升”的新结点插入到上一层
			Node * up_node = new Node(_data);
			up_node->m_span = temp_span;
			this->AppendNode(left_node, up_node);
			up_node->m_down_node = new_node;
			new_node->m_up_node = up_node;
			new_node = up_node;
			curr_level++;
		}
		// 维护新入节点的右节点span , 参数为新入节点的底层节点
		this->AddRightNodeSpan(return_node);
		return std::pair<Iterator, bool>(Iterator(m_head, m_tail, return_node), true);
	}
	// 删除一个节点 , 返回被删除节点的右节点 ， 删除结果
	std::pair<Iterator, bool> Erase(const t_name & _data)
	{
		Node * removed_node = this->Search(_data);
		if (NULL == removed_node)
		{
			return std::pair<Iterator, bool>(this->End(), false);
		}

		Node * return_node = removed_node->m_right_node;
		Node * calc_span_node = removed_node->m_right_node;
		int curr_level = 0;
		while (NULL != removed_node)
		{
			removed_node->m_down_node = NULL;
			Node * tmp_node = removed_node;
			calc_span_node = removed_node->m_right_node;
			removed_node->m_right_node->m_left_node = removed_node->m_left_node;
			removed_node->m_left_node->m_right_node = removed_node->m_right_node;
			if (!removed_node->m_right_node->IsTail())
			{
				removed_node->m_right_node->m_span = removed_node->m_right_node->m_span - 1 + removed_node->m_span;
			}
			//如果只剩头和尾节点 ， 删除该层 , 最底层不删
			if (0 != curr_level && removed_node->m_right_node->IsTail() && removed_node->m_left_node->IsHead())
			{
				// 因为删除层级后 ,原有指向指针会失效,所以提前向下一层取值
				calc_span_node = removed_node->m_right_node->m_down_node;

				RemoveLevel(&removed_node->m_left_node);
			}
			removed_node = removed_node->m_up_node;
			if (NULL != removed_node)
			{
				removed_node->m_down_node = NULL;
			}
			delete tmp_node;
			tmp_node = NULL;
			++curr_level;
		}
		--m_size;
		this->CalcRightNodeSpan(calc_span_node);
		if (NULL == return_node)
		{
			return std::pair<Iterator, bool>(this->End(), true);
		}
		return std::pair<Iterator, bool>(return_node->IsTail() ? this->End() : Iterator(m_head, m_tail, return_node), true);
	}
	// 删除一个节点 , 返回被删除节点的右节点 ， 删除结果
	std::pair<Iterator, bool> Erase(Iterator iter)
	{
		if (this->End() == iter)
		{
			std::pair<Iterator, bool>(this->End(), false);
		}
		return this->Erase(*iter);
	}
	void Clear()
	{
		// 清空除底层头尾节点外的所有节点
		Node * node = m_head;
		// 清空除底层外的所有节点
		while (m_head->m_down_node)
		{
			Node * head_down_node = m_head->m_down_node;
			Node * tail_down_node = m_tail->m_down_node;
			while (m_head)
			{
				Node * delete_node = m_head;
				m_head = m_head->m_right_node;
				delete delete_node;
				delete_node = NULL;
			}
			m_head = head_down_node;
			m_tail = tail_down_node;
		}
		//清除底层除头尾节点外的所有节点 
		m_head->m_up_node = NULL;
		m_tail->m_up_node = NULL;
		node = m_head->m_right_node;
		while (node && !node->IsTail())
		{
			Node * delete_node = node;
			node = node->m_right_node;
			delete delete_node;
			delete_node = NULL;
		}
		m_head->m_right_node = m_tail;
		m_tail->m_left_node = m_head;
		m_size = 0;
		m_max_level = 0;
	}
	// 获取数据在列表中处于什么位置
	int GetIndex(const t_name & _data)
	{
		int total_span = 0;
		Node * left_node = this->FindNode(_data, &total_span);
		if (left_node->IsHead())
		{
			return -1;
		}
		if (_data == left_node->m_data)
		{
			return total_span - 1; // 索引需要从0开始
		}
		return -1;
	}
	// 获取数据将会插入到列表中处于什么位置[只是查询,不如插入或更新数据]
	int GetSpan(const t_name & _data)
	{
		int total_span = 0;
		this->FindNode(_data, &total_span);
		return total_span;
	}
	// 更新表内数据 , 返回更新后的数据所在的节点位置(底层) 和 更新结果
	std::pair<Iterator, bool> UpdateData(const t_name & _old_data, const t_name & _new_data)
	{
		Node * want_update_node = this->Search(_old_data);
		if (NULL == want_update_node)
		{
			return std::pair<Iterator, bool>(this->End(), false);
		}
		// 找到数据了
		// 2种情况 :
		// 1. 直接更新原节点 , 无需改动节点位置 
		if ((!want_update_node->m_left_node->IsHead() &&
			m_compare(want_update_node->m_left_node->m_data, _new_data) &&
			!want_update_node->m_right_node->IsTail() &&
			m_compare(_new_data, want_update_node->m_right_node->m_data))
			|| (want_update_node->m_left_node->IsHead() && m_compare(_new_data, want_update_node->m_right_node->m_data))
			|| (want_update_node->m_right_node->IsTail() && m_compare(want_update_node->m_left_node->m_data, _new_data)))
		{
			Node * return_node = want_update_node;
			while (NULL != want_update_node)
			{
				want_update_node->m_data = _new_data;
				want_update_node = want_update_node->m_up_node;
			}
			return std::pair<Iterator, bool>
				(Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(return_node)), true);
		}

		// 2. 需要调整节点位置 , 删旧节点 , 添加新节点
		std::pair<Iterator, bool> ret = this->Erase(_old_data);
		if (!ret.second)
		{
			return ret;
		}
		return this->Insert(_new_data);
	}

	// 测试用打印表内所有层级的所有数据
	// 只能用数据类型只能用int
	void TestPrintfAllData()
	{
		Node * node = m_head;
		while (node)
		{
			Node * temp_node = node->m_right_node;
			while (temp_node && !temp_node->IsTail())
			{
				printf("%d[%d] ", temp_node->m_data, temp_node->m_span);
				temp_node = temp_node->m_right_node;
			}
			printf("\n");
			node = node->m_down_node;
		}
	}
public:
	inline bool IsEmpty() const
	{
		return 0 == m_size;
	}
	inline bool IsFull() const 
	{
		return m_size == m_default_max_size;
	}
	inline unsigned int Size() const
	{
		return m_size;
	}

	void SetCompare(bool(*_compare)(const t_name & _a, const t_name & _b))
	{
		m_compare = _compare;
	}

	bool SetDefaultMaxSize(unsigned int _size)
	{
		if (_size < m_default_max_size)
		{
			return false;
		}

		m_default_max_size = _size;
		m_max_limit_level = (int)(log(m_default_max_size) / log(2));
		if (m_max_limit_level < m_max_level)
		{
			m_max_limit_level = m_max_level;
		}
		if (m_max_limit_level > MAX_SKIP_LIST_LEVEL_COUNT)
		{
			m_max_limit_level = MAX_SKIP_LIST_LEVEL_COUNT;
		}
		return true;
	}

public:
	SkipList(unsigned int _default_max_size = 0 , bool(*_compare)(const t_name & _a, const t_name & _b) = &SkipList::Compare)
		: m_max_level(0), m_size(0), m_default_max_size(_default_max_size), m_max_limit_level(1), m_head(NULL), m_tail(NULL), m_compare(_compare)
	{
		m_head = new Node();
		m_tail = new Node();

		m_head->m_right_node = m_tail;
		m_tail->m_left_node = m_head;

		if (0 == m_default_max_size)
		{
			m_max_limit_level = MAX_SKIP_LIST_LEVEL_COUNT;
		}
		else
		{
			m_max_limit_level = (int)(log(m_default_max_size) / log(2));
			if (m_max_limit_level > MAX_SKIP_LIST_LEVEL_COUNT)
			{
				m_max_limit_level = MAX_SKIP_LIST_LEVEL_COUNT;
			}
		}
	}

	~SkipList()
	{
		while (NULL != m_head)
		{
			Node * temp = m_head->m_down_node;

			while (NULL != m_head)
			{
				Node * tmp_node = m_head;
				m_head = m_head->m_right_node;
				delete tmp_node;
				tmp_node = NULL;
			}
			m_head = temp;
		}
	}
private:
	// 查找某个元素
	Node * Search(t_name const & _data)
	{
		Node * p = this->FindNode(_data);
		if (p->IsHead())
		{
			return NULL;
		}
		if (_data == p->m_data)
		{
			return p;
		}
		return NULL;
	}
	// 返回查找值所对应的节点 或者前置节点
	// 空表返回头结点
	// 非空 ， 找到对应的值则返回值所在的节点
	// 非空 , 没找到对应的值则返回该值得前节点
	Node * FindNode(t_name const & _data, int * _total_span = NULL)
	{
		if (this->IsEmpty())
			return m_head;
		int temp_total_span = 0;
		Node * node = m_head;
		while (true)
		{
			while (!node->m_right_node->IsTail() && m_compare(node->m_right_node->m_data, _data))
			{
				node = node->m_right_node;
				temp_total_span += node->m_span;
			}
			if (NULL == node->m_down_node)
			{
				break;
			}
			node = node->m_down_node;
		}
		if (NULL != _total_span)
			*_total_span = temp_total_span;
		return node;
	}
	// 附加node , 将new_node附加到_left_node之后
	void AppendNode(Node * _left_node, Node * _new_node)
	{
		assert(NULL != _left_node && NULL != _new_node);

		_new_node->m_left_node = _left_node;
		_new_node->m_right_node = _left_node->m_right_node;
		_left_node->m_right_node->m_left_node = _new_node;
		_left_node->m_right_node = _new_node;

	}
	// 添加一个空的新层级
	void AddLevel()
	{
		Node * _new_head = new Node();
		Node * _new_tail = new Node();
		m_head->m_up_node = _new_head;
		_new_head->m_down_node = m_head;
		m_tail->m_up_node = _new_tail;
		_new_tail->m_down_node = m_tail;
		_new_head->m_right_node = _new_tail;
		_new_tail->m_left_node = _new_head;
		m_head = _new_head;
		m_tail = _new_tail;

		++m_max_level;
	}
	// 删除一个层级
	void RemoveLevel(Node ** _left_node)
	{
		Node * right_node = (*_left_node)->m_right_node;
		//如果删除层是最高层
		if (NULL == (*_left_node)->m_up_node)
		{
			(*_left_node)->m_down_node->m_up_node = NULL;
			right_node->m_down_node->m_up_node = NULL;
			m_head = (*_left_node)->m_down_node;
			m_tail = right_node->m_down_node;
		}
		else
		{
			(*_left_node)->m_up_node->m_down_node = (*_left_node)->m_down_node;
			(*_left_node)->m_down_node->m_up_node = (*_left_node)->m_up_node;
			right_node->m_up_node->m_down_node = right_node->m_down_node;
			right_node->m_down_node->m_up_node = right_node->m_up_node;
		}

		delete (*_left_node); (*_left_node) = NULL;
		delete right_node; right_node = NULL;

		--m_max_level;
	}
	// 获取底层节点
	Node * GetGroundNode(Node * _node)
	{
		Node * node = _node;
		while (node->m_down_node)
		{
			node = node->m_down_node;
		}
		return node;
	}
	// 对比函数
	static bool Compare(const t_name & _a, const t_name &  _b)
	{
		if (_a <= _b)
		{
			return true;
		}
		return false;
	}
	// 添加右节点跨度
	void AddRightNodeSpan(Node * _new_node)
	{
		// 右节点为尾节点了 , 则说明新节点是最后一个节点
		if (_new_node->m_right_node->IsTail())
		{
			return;
		}

		// 右节点有2种情况
		// 1.当前右节点是最高层
		// 2.当前右节点的右边有比自己更高的层级
		Node * node = _new_node;
		// answer_1 : 直接往上找就好了
		while (NULL != node && !node->m_right_node->IsTail())
		{
			node->m_right_node->m_span = node->m_right_node->m_span + 1 - node->m_span;
			if (NULL == node->m_up_node)
			{
				break;
			}
			node = node->m_up_node;
		}
		// 右节点为尾节点了 , 则说明当前层级当前节点是最后一个节点了
		if (node->m_right_node->IsTail())
		{
			return;
		}

		// answer_2 : 由于answer_1 ,当前节点已经计算过span了 ,
		// 检查当前节点右边是否还有比自己层级高 , 比自己层级高的 , 则对应节点span+1
		while (!node->IsTail())
		{
			while (NULL == node->m_up_node && !node->IsTail())
			{
				node = node->m_right_node;
			}
			node = node->m_up_node;
			if (NULL == node || node->IsTail())
			{
				break;
			}
			++node->m_span;
		}
	}
	// 用于删除高于 calc_node 层级的 右节点span
	// 当前_calc_node与原需要删除的节点最高级平级 , 且已经处理过span了
	void CalcRightNodeSpan(Node * _calc_span_node)
	{
		Node * node = _calc_span_node;
		while (!node->IsTail())
		{
			while (NULL != node->m_up_node)
			{
				node = node->m_up_node;
				if (NULL != node)
				{
					--node->m_span;
				}
			}
			while (NULL == node->m_up_node && !node->IsTail())
			{
				node = node->m_right_node;
			}
			node = node->m_up_node;
			if (NULL == node || node->IsTail())
			{
				break;
			}
			--node->m_span;
		}
	}
private:
	int m_max_level;
	unsigned int m_size;
	unsigned int m_default_max_size;
	int m_max_limit_level;

	Node * m_head;
	Node * m_tail;

	bool(*m_compare)(const t_name & _a, const t_name & _b);
private:
	//禁止拷贝复制（请不要公有化以下这两个函数， 除非做好深拷贝）
	SkipList(const SkipList&);
	SkipList& operator=(const SkipList &);
};


#endif	// __SKIP_LIST_HPP__

