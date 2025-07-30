#ifndef __XUN_BAO_HPP__
#define __XUN_BAO_HPP__

#include <set>
#include "protocol/msgother.h"

class Role;

class XunBao
{
public:
	static XunBao & Instance();

	void OnUserLogin(Role * role);
	int Draw(Role * role, int draw_type, const int draw_times, int num, XunBaoComsumeInfo * comsume_info,bool use_free_times);
	void SendOpenInfo(Role * role = NULL);

	void DrawGuidePet(Role * role, int column, int index);
	void DrawGuidePet2(Role * role, int column, int index);

	void DrawGuidePartner(Role * role);
	void DrawGuidePartner2(Role * role);

	int ExchangeItem(Role * role, int draw_type, int get_seq, int comsume_seq1, int comsume_seq2, const std::set<Protocol::XunBaoExchangeComsumeItem> & comsume1, const std::set<Protocol::XunBaoExchangeComsumeItem> & comsume2);
private:
	void SendDrawResult(Role * role, const std::vector<ItemConfigData> & rewards);

	XunBao();
	~XunBao();
};

#endif 