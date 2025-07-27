#ifndef __MATCH_QUEUE_HPP__
#define __MATCH_QUEUE_HPP__

template<typename T>
class MatchQueue
{
public:
	typedef long long QID;
	typedef std::map<QID, T*> TQueue;
	typedef std::map<int, QID> IdHash;

public:
	MatchQueue();
	~MatchQueue();

	void Push(T* t);
	void Pop();
	T* Front();
	bool Empty();
	int Size();
	T* FindById(int id);
	void EraseById(int id);

private:
	TQueue m_queue;
	IdHash m_id_hash;

	long long m_qid_generator;
};

template<typename T>
MatchQueue<T>::~MatchQueue()
{

}

template<typename T>
MatchQueue<T>::MatchQueue()
{

}

#endif