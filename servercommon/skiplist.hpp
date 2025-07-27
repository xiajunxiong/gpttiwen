#ifndef __SKIP_LIST_HPP__
#define __SKIP_LIST_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <assert.h>
#include<math.h>

static const int MAX_SKIP_LIST_LEVEL_COUNT = 32;
static const unsigned int MAX_UINT = 0xffffffff;

// �����ݽṹ�����߼�����Ϊ�����а���������
// �����а�������Ҫ�õ������ݽṹ��ʱ�� , ���Ȳ鿴��Ӧ�߼��Ա��ⲻ��Ҫ�ķ���

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

		t_name m_data;				// ��Ҫ����

		unsigned int m_span;		// ��ǰ�㼶�ýڵ㵽ǰ�ڵ��֮��Ŀ��

		Node * m_up_node;			// ��ָ��
		Node * m_down_node;			// ��ָ��
		Node * m_left_node;			// ��ָ��
		Node * m_right_node;		// ��ָ��
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
	// ���������
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
	// ��ȡ��ʼ������
	Iterator Begin(void)
	{
		return Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_head)->m_right_node);
	}
	// ��ȡ��ֹ������
	Iterator End(void)
	{
		return Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_tail));
	}
	// ��ȡ������ʼ������
	Reverse_Iterator RBegin(void)
	{
		return  Reverse_Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_tail)->m_left_node);
	}
	// ��ȡ������ֹ������
	Reverse_Iterator REnd(void)
	{
		return Reverse_Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), GetGroundNode(m_head));
	}

	// ����ĳ��Ԫ��
	Iterator Find(t_name const & _data)
	{
		Node * p = this->Search(_data);
		if (NULL == p)
		{
			return this->End();
		}

		return Iterator(GetGroundNode(m_head), GetGroundNode(m_tail), p);
	}
	// ����������ȡ��ӦԪ�����ڵĵײ�ڵ������
	Iterator operator[] (unsigned int index)
	{
		if (index >= m_size || this->IsEmpty())
		{
			return this->End();
		}

		// span �Ǵ�1��ʼ��
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

	// ����ĳ��Ԫ��
	std::pair<Iterator, bool> Insert(const t_name  & _data)
	{
		// 0 == m_default_max_size ���ʾ�����������ֵ , ����Բ������2^64�������� 
		if ((m_size >= m_default_max_size && 0 != m_default_max_size )|| m_size >= MAX_UINT)
		{
			return std::pair<Iterator, bool>(this->End(), false);
		}

		Node * left_node = this->FindNode(_data);
		//���data��ͬ��ֱ�ӷ��� false
		if (!left_node->IsHead() && left_node->m_data == _data)
		{
			return std::pair<Iterator, bool>(Iterator(m_head, m_tail, left_node), false);
		}

		Node * new_node = new Node(_data);
		++m_size;
		this->AppendNode(left_node, new_node);
		new_node->m_span = 1;
		Node * return_node = new_node;

		//�����������Ƿ񡰽����� 
		int curr_level = 0;
		int temp_span = 1;
		while (rand() % 2 == 1 && curr_level < m_max_limit_level) // ���32�㹻 2^64�������� ,
		{
			//�����ǰ���Ѿ�����߲㣬��Ҫ����һ��
			if (curr_level == m_max_level)
			{
				this->AddLevel();
			}
			//�ҵ���һ���ǰ�ýڵ�
			while (NULL == left_node->m_up_node)
			{
				temp_span += left_node->m_span;
				left_node = left_node->m_left_node;
			}
			assert(NULL != left_node);

			left_node = left_node->m_up_node;
			//�ѡ����������½����뵽��һ��
			Node * up_node = new Node(_data);
			up_node->m_span = temp_span;
			this->AppendNode(left_node, up_node);
			up_node->m_down_node = new_node;
			new_node->m_up_node = up_node;
			new_node = up_node;
			curr_level++;
		}
		// ά������ڵ���ҽڵ�span , ����Ϊ����ڵ�ĵײ�ڵ�
		this->AddRightNodeSpan(return_node);
		return std::pair<Iterator, bool>(Iterator(m_head, m_tail, return_node), true);
	}
	// ɾ��һ���ڵ� , ���ر�ɾ���ڵ���ҽڵ� �� ɾ�����
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
			//���ֻʣͷ��β�ڵ� �� ɾ���ò� , ��ײ㲻ɾ
			if (0 != curr_level && removed_node->m_right_node->IsTail() && removed_node->m_left_node->IsHead())
			{
				// ��Ϊɾ���㼶�� ,ԭ��ָ��ָ���ʧЧ,������ǰ����һ��ȡֵ
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
	// ɾ��һ���ڵ� , ���ر�ɾ���ڵ���ҽڵ� �� ɾ�����
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
		// ��ճ��ײ�ͷβ�ڵ�������нڵ�
		Node * node = m_head;
		// ��ճ��ײ�������нڵ�
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
		//����ײ��ͷβ�ڵ�������нڵ� 
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
	// ��ȡ�������б��д���ʲôλ��
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
			return total_span - 1; // ������Ҫ��0��ʼ
		}
		return -1;
	}
	// ��ȡ���ݽ�����뵽�б��д���ʲôλ��[ֻ�ǲ�ѯ,���������������]
	int GetSpan(const t_name & _data)
	{
		int total_span = 0;
		this->FindNode(_data, &total_span);
		return total_span;
	}
	// ���±������� , ���ظ��º���������ڵĽڵ�λ��(�ײ�) �� ���½��
	std::pair<Iterator, bool> UpdateData(const t_name & _old_data, const t_name & _new_data)
	{
		Node * want_update_node = this->Search(_old_data);
		if (NULL == want_update_node)
		{
			return std::pair<Iterator, bool>(this->End(), false);
		}
		// �ҵ�������
		// 2����� :
		// 1. ֱ�Ӹ���ԭ�ڵ� , ����Ķ��ڵ�λ�� 
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

		// 2. ��Ҫ�����ڵ�λ�� , ɾ�ɽڵ� , ����½ڵ�
		std::pair<Iterator, bool> ret = this->Erase(_old_data);
		if (!ret.second)
		{
			return ret;
		}
		return this->Insert(_new_data);
	}

	// �����ô�ӡ�������в㼶����������
	// ֻ������������ֻ����int
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
	// ����ĳ��Ԫ��
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
	// ���ز���ֵ����Ӧ�Ľڵ� ����ǰ�ýڵ�
	// �ձ���ͷ���
	// �ǿ� �� �ҵ���Ӧ��ֵ�򷵻�ֵ���ڵĽڵ�
	// �ǿ� , û�ҵ���Ӧ��ֵ�򷵻ظ�ֵ��ǰ�ڵ�
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
	// ����node , ��new_node���ӵ�_left_node֮��
	void AppendNode(Node * _left_node, Node * _new_node)
	{
		assert(NULL != _left_node && NULL != _new_node);

		_new_node->m_left_node = _left_node;
		_new_node->m_right_node = _left_node->m_right_node;
		_left_node->m_right_node->m_left_node = _new_node;
		_left_node->m_right_node = _new_node;

	}
	// ���һ���յ��²㼶
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
	// ɾ��һ���㼶
	void RemoveLevel(Node ** _left_node)
	{
		Node * right_node = (*_left_node)->m_right_node;
		//���ɾ��������߲�
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
	// ��ȡ�ײ�ڵ�
	Node * GetGroundNode(Node * _node)
	{
		Node * node = _node;
		while (node->m_down_node)
		{
			node = node->m_down_node;
		}
		return node;
	}
	// �ԱȺ���
	static bool Compare(const t_name & _a, const t_name &  _b)
	{
		if (_a <= _b)
		{
			return true;
		}
		return false;
	}
	// ����ҽڵ���
	void AddRightNodeSpan(Node * _new_node)
	{
		// �ҽڵ�Ϊβ�ڵ��� , ��˵���½ڵ������һ���ڵ�
		if (_new_node->m_right_node->IsTail())
		{
			return;
		}

		// �ҽڵ���2�����
		// 1.��ǰ�ҽڵ�����߲�
		// 2.��ǰ�ҽڵ���ұ��б��Լ����ߵĲ㼶
		Node * node = _new_node;
		// answer_1 : ֱ�������Ҿͺ���
		while (NULL != node && !node->m_right_node->IsTail())
		{
			node->m_right_node->m_span = node->m_right_node->m_span + 1 - node->m_span;
			if (NULL == node->m_up_node)
			{
				break;
			}
			node = node->m_up_node;
		}
		// �ҽڵ�Ϊβ�ڵ��� , ��˵����ǰ�㼶��ǰ�ڵ������һ���ڵ���
		if (node->m_right_node->IsTail())
		{
			return;
		}

		// answer_2 : ����answer_1 ,��ǰ�ڵ��Ѿ������span�� ,
		// ��鵱ǰ�ڵ��ұ��Ƿ��б��Լ��㼶�� , ���Լ��㼶�ߵ� , ���Ӧ�ڵ�span+1
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
	// ����ɾ������ calc_node �㼶�� �ҽڵ�span
	// ��ǰ_calc_node��ԭ��Ҫɾ���Ľڵ���߼�ƽ�� , ���Ѿ������span��
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
	//��ֹ�������ƣ��벻Ҫ���л����������������� �������������
	SkipList(const SkipList&);
	SkipList& operator=(const SkipList &);
};


#endif	// __SKIP_LIST_HPP__

