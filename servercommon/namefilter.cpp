#include "namefilter.hpp"

#include <fstream>
#include <string.h>

NameNode::NameNode()
{

}

NameNode::~NameNode()
{
	for (NodeMapIt it = m_childs.begin(); it != m_childs.end(); ++it)
	{
		if (NULL != it->second)
		{
			delete it->second;
		}
	}

	m_childs.clear();
}

NameFilter::NameFilter()
{
	m_root = new NameNode();
}

NameFilter::~NameFilter()
{
	if (NULL != m_root)
	{
		delete m_root;
		m_root = NULL;
	}
}

NameFilter & NameFilter::Instance()
{
	static NameFilter instance;
	return instance;
}

bool NameFilter::Init(const char *filename, std::string *err)
{
	std::ifstream ifs(filename);
	if (ifs.fail())
	{
		if (NULL != err) *err = "File Not Exist";
		return false;
	}

	char buf[MAX_SENSITIVEWORD_SIZE + 1] = {0};
	int loop_count = 1000000;

	while (!ifs.eof())
	{
		if (loop_count-- <= 0)
		{
			if (NULL != err) *err = "Dead loop";
			return false;
		}

		ifs.getline(buf, MAX_SENSITIVEWORD_SIZE); buf[MAX_SENSITIVEWORD_SIZE - 1] = 0;

		size_t nLen = strlen(buf);
		while (nLen > 0 && ((unsigned int)buf[nLen - 1] <= 0x20))
		{
			buf[nLen - 1] = 0;
			nLen--;
		}

		if (nLen > 0) this->AddSensitiveWord(sizeof(buf), buf);
	}

	printf("NameFilter::Init Success.\n");

	return true;
}

void NameFilter::AddSensitiveWord(int max_len, const char *word)
{
	NameNode *node = m_root;
	
	int length = max_len < MAX_SENSITIVEWORD_SIZE ? max_len : MAX_SENSITIVEWORD_SIZE;

	for (int i = 0; i < length && '\0' != word[i]; ++i)
	{
		NameNode::NodeMapIt it = node->m_childs.find(word[i]);
		if (it == node->m_childs.end())
		{
			NameNode *temp = new NameNode();
			node->m_childs.insert(std::make_pair(word[i], temp));
			node = temp;
		}
		else
		{
			node = it->second;
		}
	}

	node->m_childs.insert(std::make_pair('\0', new NameNode()));
}

bool NameFilter::HasSensitiveWord(int max_len, const char *input_word)
{
	int length = max_len < MAX_SENSITIVEWORD_SIZE ? max_len : MAX_SENSITIVEWORD_SIZE;
	char match_array[MAX_SENSITIVEWORD_SIZE] = { 0 };
	int match_idx = 0;

	for (int i = 0; i < length && '\0' != input_word[i]; ++i)
	{
		NameNode *node = m_root;
		const char *cur_pos = &input_word[i];
		memset(match_array, 0, sizeof(match_array));
		match_idx = 0;

		int k = length - i;
		while ('\0' != *cur_pos && k-- > 0)
		{
			if (node->m_childs.find('\0') != node->m_childs.end())
			{
				printf("NameFilter::HasSensitiveWord input[%s] match[%s]\n", input_word, match_array);
				return true;
			}
			NameNode::NodeMapIt iter = node->m_childs.find(*cur_pos);
			if (iter == node->m_childs.end()) break;

			match_array[match_idx++] = *cur_pos;
			node = iter->second;
			++cur_pos;
		}

		if ('\0' == *cur_pos && node->m_childs.find('\0') != node->m_childs.end())
		{
			printf("NameFilter::HasSensitiveWord input[%s] match[%s]\n", input_word, match_array);
			return true;
		}
	}

	return false;
}

