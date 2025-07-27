#include "common/md5/md5cpp.h"
#include "servercommon/maildef.hpp"
#include "common/bintohex.h"
#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include <sstream>
#include <iostream>
#include <fstream>
#include <time.h>
#include <vector>

#ifdef __unix
	#include "hotfix/hotfix.h"
#endif

#ifdef WIN32
	#include <io.h>
	#include <direct.h>
#else
	#include <sys/stat.h>
	#include <unistd.h>
	#include <execinfo.h>
#endif

OLD_USE_INTERNAL_NETWORK(); 

const char * ItemDataWrapperLog(const ItemDataWrapper *item_wrapper, int index)
{
	if (index < 0 || index >= LOG_MAX_ITEM_NUM) return "NULL";

	static char item_wrapper_log[LOG_BASE_LENGTH * LOG_MAX_ITEM_NUM] = { 0 };
	char *p = item_wrapper_log + index * LOG_BASE_LENGTH;

	if (NULL == item_wrapper)
	{
		SNPRINTF(p, LOG_BASE_LENGTH, "NULL");
	}
	else
	{
		SNPRINTF(p, LOG_BASE_LENGTH,
				 "item_%d:{[%d,%d,%d] invalid_time:%d}",
				 (int)index,
				 (int)item_wrapper->item_id,
				 (int)item_wrapper->num,
				 (int)item_wrapper->is_bind,
				 (int)item_wrapper->invalid_time);
	}

	return p;
}

const char * MailVirtualItemLog(const MailVirtualItemList * param)
{
	static char mail_virtual_item_log[1024] = { 0 };

	if (NULL == param)
	{
		SNPRINTF(mail_virtual_item_log, sizeof(mail_virtual_item_log), "NULL");
	}
	else
	{
		UNSTD_STATIC_CHECK(15 == MAX_VIRTUAL_ITEM_NUM);

		SNPRINTF(mail_virtual_item_log, sizeof(mail_virtual_item_log),
				 "exp:%d, gongxian:%d, immortal_gold:%d, gold:%d, bind_coin:%d,colosseum_credits:%d, platform_battle_credits:%d",
				 (int)param->virtual_item[MAIL_VIRTUAL_ITEM_EXP],
				 (int)param->virtual_item[MAIL_VIRTUAL_ITEM_GUILDGONGXIAN],
				 (int)param->virtual_item[MAIL_VIRTUAL_ITEM_IMMORTAL_COIN],
				 (int)param->virtual_item[MAIL_VIRTUAL_ITEM_GOLD],
				 (int)param->virtual_item[MAIL_VIRTUAL_ITEM_BIND_COIN],
				(int)param->virtual_item[MAIL_VIRTUAL_COLOSSEUM_CREDITS],
				(int)param->virtual_item[MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS]
		);
	}

	return mail_virtual_item_log;
}


const char * MailParamLog(const MailParam *param)
{
	static char mail_param_log[4096] = { 0 };

	if (NULL == param)
	{
		SNPRINTF(mail_param_log, sizeof(mail_param_log), "NULL");
	}
	else
	{
		UNSTD_STATIC_CHECK(8 == MAX_ATTACHMENT_ITEM_NUM);

		SNPRINTF(mail_param_log, sizeof(mail_param_log),
			"sender[uid:%d, name:%s], mail_index[%d], serial_num[%s], recv_time[%d], kind[%d], is_read[%d], is_lock[%d], is_fetch[%d], reason[%d], subject[%s], content[%s]"
			"attachment[%s,%s,%s,%s,%s,%s,%s,%s, virtual_items[%s]]",
			(int)param->status.sender_uid,
			(const char*)param->status.sender_name,
			(int)param->mail_index,
			param->mail_serial,
			(int)param->status.recv_time,
			(int)param->status.kind,
			(int)param->status.is_read,
			(int)param->status.is_lock,
			(int)param->status.is_fetch,
			(int)param->status.reason,
			(const char*)param->contentparam.subject,
			(const char*)param->contentparam.contenttxt,
			ItemDataWrapperLog(&param->contentparam.item_list[0], 0),
			ItemDataWrapperLog(&param->contentparam.item_list[1], 1),
			ItemDataWrapperLog(&param->contentparam.item_list[2], 2),
			ItemDataWrapperLog(&param->contentparam.item_list[3], 3),
			ItemDataWrapperLog(&param->contentparam.item_list[4], 4),
			ItemDataWrapperLog(&param->contentparam.item_list[5], 5),
			ItemDataWrapperLog(&param->contentparam.item_list[6], 6),
			ItemDataWrapperLog(&param->contentparam.item_list[7], 7),
			MailVirtualItemLog(&param->contentparam.virtual_item_list));
	}

	return mail_param_log;
}

const char * MailParamSimpleLog(const MailParam *param)
{
	static char mail_param_log[4096] = { 0 };

	if (NULL == param)
	{
		SNPRINTF(mail_param_log, sizeof(mail_param_log), "NULL");
	}
	else
	{
		UNSTD_STATIC_CHECK(8 == MAX_ATTACHMENT_ITEM_NUM);

		SNPRINTF(mail_param_log, sizeof(mail_param_log),
			"sender[uid:%d, name:%s], mail_index[%d], serial_num[%s], recv_time[%d], kind[%d], is_read[%d], is_lock[%d], is_fetch[%d], reason[%d], subject[%s], content[%s]",
			(int)param->status.sender_uid,
			(const char*)param->status.sender_name,
			(int)param->mail_index,
			param->mail_serial,
			(int)param->status.recv_time,
			(int)param->status.kind,
			(int)param->status.is_read,
			(int)param->status.is_lock,
			(int)param->status.is_fetch,
			(int)param->status.reason,
			(const char*)param->contentparam.subject,
			(const char*)param->contentparam.contenttxt);
	}

	return mail_param_log;
}

void MergeItemConfigData(std::vector<ItemConfigData>ARG_IN_AND_OUT & list)
{
	for (int i = 0; i < (int)list.size(); ++i)
	{	
		ItemConfigData & curr = list[i];
		for (int k = 0; k < i; ++k)
		{
			ItemConfigData & zhu = list[k];
			if (zhu.item_id == curr.item_id && zhu.is_bind == curr.is_bind && curr.num > 0)
			{
				zhu.num += curr.num;
				curr.num = 0;
				curr.item_id = 0;
				break;
			}
		}
	}

	for (std::vector<ItemConfigData>::iterator it = list.begin(); it != list.end(); )
	{
		ItemConfigData & curr = *it;
		if (curr.item_id == 0 || curr.num <= 0)
		{
			it = list.erase(it);
		}
		else
		{
			++it; 
		}
	}
}

void MergeItemDataWrapper(std::vector<ItemDataWrapper>ARG_IN_AND_OUT & list)
{
	for (int i = 0; i < (int)list.size(); ++i)
	{
		ItemDataWrapper & curr = list[i];

		for (int k = 0; k < i; ++k)
		{
			ItemDataWrapper & zhu = list[k];
			if (zhu.Merge(curr))
			{
				break;
			}
		}
	}

	for (std::vector<ItemDataWrapper>::iterator it = list.begin(); it != list.end(); )
	{
		ItemDataWrapper & curr = *it;
		if (curr.item_id == 0 || curr.num <= 0)
		{
			it = list.erase(it);
		}
		else
		{
			++it;
		}
	}
}

const char* GetIPStr(unsigned int ip)
{
	static char ip_str[16];
	memset(ip_str, 0, sizeof(ip_str));
	SNPRINTF(ip_str, sizeof(ip_str), "%d.%d.%d.%d", (ip & 0xff000000) >> 24, (ip & 0xff0000) >> 16, (ip & 0xff00) >> 8, ip & 0xff);

	return ip_str;
}

unsigned int GetIPFromStr(std::string ip_str)
{
	int ip_arr[4] = { 0 };
	std::string origin_ip_str = ip_str;
	ip_str.push_back('.'); //为了方便编程

	for (int i = 0; i < 4; ++i)
	{
		size_t pos = ip_str.find_first_of('.');
		if (std::string::npos == pos)
		{
			return 0;
		}
		std::string temp = ip_str.substr(0, pos);
		int temp_int = atoi(temp.c_str());
		if (temp_int < 0 || temp_int >= 256)
		{
			return 0;
		}

		ip_str = ip_str.erase(0, pos + 1);
		ip_arr[i] = temp_int;
	}

	char buff[32] = { 0 };
	SNPRINTF(buff, 32, "%d.%d.%d.%d", ip_arr[0], ip_arr[1], ip_arr[2], ip_arr[3]);
	if (0 != origin_ip_str.compare(buff)) //验证
	{
		return 0;
	}

	return (ip_arr[0] << 24) + (ip_arr[1] << 16) + (ip_arr[2] << 8) + ip_arr[3];
}

int IntAddOk(int  a, int  b, int & out_sum)
{
	out_sum = a + b;
	bool neg_over = a < 0 && b < 0 && out_sum >= 0;
	bool pos_over = a >= 0 && b >= 0 && out_sum < 0;

	if (pos_over)
	{
		return ADD_OK_TYPE_POS_OVER;
	}

	if (neg_over)
	{
		return ADD_OK_TYPE_NEG_OVER;
	}

	return ADD_OK_TYPE_RIGHT;
}

void IntHelpAdd(int ARG_IN_AND_OUT & curr_value, int max_value, int add_value, int min_value)
{
	int sum = 0;
	int add_type = IntAddOk(curr_value, add_value, sum);

	if (add_type == ADD_OK_TYPE_POS_OVER)
	{
		sum = max_value;
	}

	if (add_type == ADD_OK_TYPE_NEG_OVER)
	{
		sum = min_value;
	}

	sum = sum > max_value ? max_value : sum;
	sum = sum < min_value ? min_value : sum;

	curr_value = sum;
}

bool WriteFile(const char *path, const char *data, int length, bool is_append, bool is_binary)
{
	std::ios_base::openmode mode = std::ios::out;
	if (is_binary)
	{
		mode |= std::ios::binary;
	}
	if (is_append)
	{
		mode |= std::ios::app;
	}
	std::ofstream ofs(path, mode);
	if (!ofs.is_open())
	{
		return false;
	}
	
	ofs.write(data, length);
	ofs.close();
	return true;
}

bool CreateDirectory(const std::string& folder)
{
	static const char DELIMITER_1 = '\\';
	static const char DELIMITER_2 = '/';

	std::string folder_builder;
	std::string sub;
	sub.reserve(folder.size());
	for (std::string::const_iterator it = folder.begin(); it != folder.end(); ++it)
	{
		const char c = *it;
		sub.push_back(c);
		if (c == DELIMITER_1 || c == DELIMITER_2 || it == folder.end() - 1)
		{
			folder_builder.append(sub);
#ifdef WIN32
			if (0 != ::_access(folder_builder.c_str(), 0))
			{
				// this folder not exist
				if (0 != ::_mkdir(folder_builder.c_str()))
				{
					// create failed
					return false;
				}
			}
#else
			if (0 != access(folder_builder.c_str(), NULL))
			{
				// this folder not exist
				if (0 != mkdir(folder_builder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
				{
					// create failed
					return false;
				}
			}
#endif

			sub.clear();
		}
	}
	return true;
}

#ifdef WIN32

	#include <windows.h>
	#include <dbghelp.h>
	#pragma comment(lib, "dbghelp.lib")

#endif

std::string PrintStackBackTrace(bool is_dump_core)
{
	std::string ret;
	
#ifdef WIN32
	auto k = 0;
	return ret;
	static const int MAX_STACK_FRAMES = 10;

	void *pStack[MAX_STACK_FRAMES];

	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);
	WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);

	std::ostringstream oss;
	oss << "stack traceback: " << std::endl;
	for (WORD i = 0; i < frames; ++i) {
		DWORD64 address = (DWORD64)(pStack[i]);

		DWORD64 displacementSym = 0;
		char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
		pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		pSymbol->MaxNameLen = MAX_SYM_NAME;

		DWORD displacementLine = 0;
		IMAGEHLP_LINE64 line;
		//SymSetOptions(SYMOPT_LOAD_LINES);
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		if (SymFromAddr(process, address, &displacementSym, pSymbol)
			&& SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
			oss << "\t" << pSymbol->Name << " at " << line.FileName << ":" << line.LineNumber << "(0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
		}
		else {
			oss << "\terror: " << GetLastError() << std::endl;
		}
	}
	return oss.str();
	
#else
	void* buffer[100000];
	char **strings = NULL;
	int nptrs = backtrace(buffer, 100000);
	strings = backtrace_symbols(buffer, nptrs);
	if (NULL == strings) 
	{
		assert(0);
	}
	for (int jj = 0; jj < nptrs; ++jj) 
	{
		//printf("%s\n", strings[jj]);
		ret += strings[jj];
		ret += "|";
	}
	free(strings);
#endif	

	if (is_dump_core)
	{
		assert(0);
	}

	return ret;
}

bool CheckNameHasServerSuffix(const std::string& name)
{
	return std::string::npos != name.find_last_of("_s", name.length());
}

std::vector<int> RandomPermutation(int start_number, int end_number)
{
	std::vector<int> permutation;
	if (end_number < start_number) return permutation;

	permutation.reserve(end_number - start_number);
	for (int i = start_number; i < end_number; ++i)
	{
		permutation.push_back(i);
	}

	std::random_shuffle(permutation.begin(), permutation.end());
	return permutation;
}

void GetSessionKey(const std::string &key_base, int scene_id, int scene_key, int last_scene_id, int role_id, GameName role_name, unsigned int time, char first_login, PlatName pname, SessionKey key)
{
	char str_tmp[256];
	int str_len = SNPRINTF(str_tmp, sizeof(str_tmp), "%s;%d;%s;%u;%d;%s", key_base.c_str(), (scene_id + scene_key + last_scene_id + role_id),
		(char *)role_name, time, (int)first_login, (char*)pname);

	MD5 md5;
	MD5_buffer(&md5, str_tmp, (unsigned long)str_len);
	BinToHex((const char*)md5, sizeof(MD5), key);
	key[sizeof(SessionKey) - 1] = 0;
}

unsigned int GetChatLoginKey(int uid)
{
	return (unsigned int)uid;
}

void LongLongToUInt(long long l, unsigned int(&out)[2])
{
	union
	{
		long long ll_value;
		struct
		{
			unsigned int param_0;
			unsigned int param_1;
		};
	} union_param;

	union_param.ll_value = l;

	out[0] = union_param.param_0;
	out[1] = union_param.param_1;
}

void LongLongToInt(long long l, int(&out)[2])
{
	union
	{
		long long ll_value;
		struct
		{
			int param_0;
			int param_1;
		};
	} union_param;

	union_param.ll_value = l;
	
	out[0] = union_param.param_0;
	out[1] = union_param.param_1;
}

long long ConvertParamToLongLong(int param_0, int param_1)
{
	union 
	{
		long long ll_value;
		struct  
		{
			int param_0;
			int param_1;
		};
	} union_param;

	union_param.param_0 = param_0;
	union_param.param_1 = param_1;

	return union_param.ll_value;
}

long long ConvertParamToLongLong(int param_0, short param_1, short param_2)
{
	union 
	{
		long long ll_value; 
		struct  
		{
			int param_0;
			short param_1;
			short param_2;
		};
	} union_param;

	union_param.param_0 = param_0;
	union_param.param_1 = param_1;
	union_param.param_2 = param_2;

	return union_param.ll_value;
}

bool HasInvalidChar(char *str)
{
	while (*str)
	{
		if (*str == ';' || *str == '|' || *str == '\n' || *str == '\t' || *str == ' ' || *str == '[' || *str == ']' || *str == '\'' || *str == '\"' || *str == ',')
		{
			return true;
		}

		str++;
	}

	return false;
}

void InvalidCharFilte(char *str)
{
	while (*str)
	{
		if (*str == ';' || *str == '|' || *str == '\n' || *str == '\t' || *str == ' ' || *str == '[' || *str == ']' || *str == '\'' || *str == '\"' || *str == ',') *str = '*';
		str++;
	}
}

void RoleNameCatPostfix(GameName rolename, short server)
{
	//GameName tmp;
	//F_STRNCPY(tmp, rolename, sizeof(GameName));
	//SNPRINTF(rolename, sizeof(GameName), "%s[%d]", tmp, (int)server);
}

void PlatNameCatPostfix(PlatName pname, short server)
{
	static PlatName tmp;
	F_STRNCPY(tmp, pname, sizeof(PlatName));
	SNPRINTF(pname, sizeof(PlatName), "%s_%d", tmp, (int)server);
}

void GuildNameCatPostfix(GuildName guildname, short server)
{
	//GuildName tmp;
	//F_STRNCPY(tmp, guildname, sizeof(GuildName));
	//SNPRINTF(guildname, sizeof(GuildName), "%s[%d]", tmp, (int)server); 
}

char * GetCurVersion(unsigned int unVer)
{
	char szVer[32] = { 0 };
	SNPRINTF(szVer, sizeof(szVer), "%u", unVer);	

	static char szVersion[128];
	const char *szVer2 = ('0' == szVer[2]) ? "trunk" : ('1' == szVer[2] ? "branch" : "");
	SNPRINTF(szVersion, sizeof(szVersion), " %c.%c %s %c%c build (%d)", szVer[0], szVer[1], szVer2, szVer[3], szVer[4], unVer);

	return &szVersion[0];
}

void PlatNameReCatPostfix(PlatName pname)
{
	int fix_pos = -1;

	for (int i = 0; i < static_cast<int>(sizeof(PlatName)); i++)
	{
		if (0 == pname[i])
		{
			break;
		}
		else if ('_' == pname[i])
		{
			fix_pos = i;
		}
	}

	if (fix_pos >= 0 && fix_pos < static_cast<int>(sizeof(PlatName)))
	{
		pname[fix_pos] = 0;
	}

	//for (int i = sizeof(PlatName) - 1; i >= 0; i--)   旧代码考虑不周，因为并没有memset整个结构，无法保证\0后续全部为0
	//{
	//	if ('_' == pname[i])
	//	{
	//		pname[i] = '\0';
	//		break;
	//	}
	//}
}

bool GetTimeTM(const time_t &cur_time, struct tm &time_tm)
{
#ifdef _WIN32
	struct tm *local_begin_time = ::localtime(&cur_time);
	if (NULL == local_begin_time)
	{
		return false;
	}
#else	
	struct tm local_time_item;
	struct tm *local_begin_time = &local_time_item;

	if (NULL == ::localtime_r(&cur_time, &local_time_item))
	{
		return false;
	}
#endif

	time_tm = *local_begin_time;
	return true;
}

int GetHourIndex(time_t begin_time, time_t t)
{
	bool is_past = (t < begin_time);
	if (is_past)
	{
		begin_time = begin_time ^ t;
		t = begin_time ^ t;
		begin_time = begin_time ^ t;
	}

	return static_cast<int>((t - begin_time) / (3600));
}

int GetDayIndex(time_t begin_time, time_t t)
{
	bool is_past = (t < begin_time);
	if (is_past)
	{
		time_t tmp_time = begin_time; begin_time = t; t = tmp_time;
	}

#ifdef _WIN32
	struct tm *local_begin_time = ::localtime(&begin_time);
	if (NULL == local_begin_time)
	{
		return 0;
	}
#else
	struct tm local_time_item;
	struct tm *local_begin_time = &local_time_item;

	if (NULL == ::localtime_r(&begin_time, &local_time_item))
	{
		return 0;
	}
#endif

	time_t zero_time = begin_time - (local_begin_time->tm_hour * 3600 + local_begin_time->tm_min * 60 + local_begin_time->tm_sec);
	int day_index = static_cast<int>((t - zero_time) / (24 * 3600));

	return (is_past ? -day_index : day_index);
}

int GetDayIndexByEndTime(time_t end_time, time_t t)
{
	bool is_past = (end_time < t);
	if (is_past)
	{
		time_t tmp_time = end_time; end_time = t; t = tmp_time;
	}

#ifdef _WIN32
	struct tm *local_end_time = ::localtime(&end_time);
	if (NULL == local_end_time)
	{
		return 0;
	}
#else
	struct tm local_time_item;
	struct tm *local_end_time = &local_time_item;

	if (NULL == ::localtime_r(&end_time, &local_time_item))
	{
		return 0;
	}
#endif

	time_t end_zero_time = end_time - (local_end_time->tm_hour * 3600 + local_end_time->tm_min * 60 + local_end_time->tm_sec);
	int day_index = static_cast<int>((end_zero_time - t) / (24 * 3600));

	return (is_past ? -day_index : day_index);
}

//!< 以6点为一天的开始，6点前为前一天
int GetDayIndex_6(time_t begin_time, time_t t)
{
	bool is_past = (t < begin_time);
	if (is_past)
	{
		time_t tmp_time = begin_time; begin_time = t; t = tmp_time;
	}

#ifdef _WIN32
	struct tm *local_begin_time = ::localtime(&begin_time);
	if (NULL == local_begin_time)
	{
		return 0;
	}
#else
	struct tm local_time_item;
	struct tm *local_begin_time = &local_time_item;

	if (NULL == ::localtime_r(&begin_time, &local_time_item))
	{
		return 0;
	}
#endif

	time_t zero_time = begin_time - (local_begin_time->tm_hour * 3600 + local_begin_time->tm_min * 60 + local_begin_time->tm_sec);
	zero_time += 6 * 3600; // 加6小时
	int day_index = static_cast<int>((t - zero_time) / (24 * 3600));

	return (is_past ? -day_index : day_index);
}

int GetWeekIndex(time_t begin_time, time_t t)
{
	bool is_past = (t < begin_time);
	if (is_past)
	{
		time_t tmp_time = begin_time; begin_time = t; t = tmp_time;
	}

	struct tm *local_begin_time = ::localtime(&begin_time);
	if (NULL == local_begin_time)
	{
		return 0;
	}

	int begin_time_passday = (local_begin_time->tm_wday + 6) % 7;  // 从周一开始算第一天 举例周日tm_wday=0 但实际上是6天

	time_t week_zerotime = begin_time - (begin_time_passday * 24 * 3600 + local_begin_time->tm_hour * 3600 + local_begin_time->tm_min * 60 + local_begin_time->tm_sec);
	int week_index = static_cast<int>((t - week_zerotime) / (7 * 24 * 3600));

	return (is_past ? -week_index : week_index);
}

time_t GetWeekZeroTime(time_t cur_time)
{
#ifdef _WIN32
	struct tm *local_begin_time = ::localtime(&cur_time);
	if (NULL == local_begin_time)
	{
		return 0;
	}
#else	
	struct tm local_time_item;
	struct tm *local_begin_time = &local_time_item;

	if (NULL == ::localtime_r(&cur_time, &local_time_item))
	{
		return 0;
	}
#endif

	int begin_time_passday = (local_begin_time->tm_wday + 6) % 7;  // 从周一开始算第一天 举例周日tm_wday=0 但实际上是6天

	time_t week_zerotime = cur_time - (begin_time_passday * 24 * 3600 + local_begin_time->tm_hour * 3600 + local_begin_time->tm_min * 60 + local_begin_time->tm_sec);
	return week_zerotime;
}

time_t GetMonthZeroTime(time_t cur_time)
{
#ifdef _WIN32
	struct tm *local_begin_time = ::localtime(&cur_time);
	if (NULL == local_begin_time)
	{
		return 0;
	}
#else	
	struct tm local_time_item;
	struct tm *local_begin_time = &local_time_item;

	if (NULL == ::localtime_r(&cur_time, &local_time_item))
	{
		return 0;
	}
#endif
	
	time_t month_zero_time = cur_time - ((local_begin_time->tm_mday - 1) * 24 * 3600 + local_begin_time->tm_hour * 3600 + local_begin_time->tm_min * 60 + local_begin_time->tm_sec);

	return month_zero_time;
}

int GetMonthIndex(time_t begin_time, time_t t)
{
	bool is_past = (t < begin_time);
	if (is_past)
	{
		time_t tmp_time = begin_time; begin_time = t; t = tmp_time;
	}

	struct tm * tmp_time = ::localtime(&begin_time);
	if (NULL == tmp_time)
	{
		return 0;
	}
	struct tm local_begin_time = *tmp_time;

	tmp_time = ::localtime(&t);
	if (NULL == tmp_time)
	{
		return 0;
	}
	struct tm local_end_time = *tmp_time;

	int month_index = (local_end_time.tm_year - local_begin_time.tm_year) * 12 + (local_end_time.tm_mon - local_begin_time.tm_mon);

	return month_index;
}

time_t GetZeroTime(time_t time_stamp)
{
#ifdef _WIN32
	struct tm *local_time = ::localtime(&time_stamp);
	if (NULL == local_time)
	{
		return 0;
	}
#else	
	struct tm local_time_item;
	struct tm *local_time = &local_time_item;

	if (NULL == ::localtime_r(&time_stamp, &local_time_item))
	{
		return 0;
	}
#endif

	time_t zero_time = time_stamp - (local_time->tm_hour * 3600 + local_time->tm_min * 60 + local_time->tm_sec);
	return zero_time;
}

time_t GetZeroMinuteTime(time_t time_stamp)
{
#ifdef _WIN32
	struct tm *local_time = ::localtime(&time_stamp);
	if (NULL == local_time)
	{
		return 0;
	}
#else	
	struct tm local_time_item;
	struct tm *local_time = &local_time_item;

	if (NULL == ::localtime_r(&time_stamp, &local_time_item))
	{
		return 0;
	}
#endif

	time_t zero_time = time_stamp - (local_time->tm_min * 60 + local_time->tm_sec);
	return zero_time;
}

time_t GetZeroSecondTime(time_t time_stamp)
{
#ifdef _WIN32
	struct tm *local_time = ::localtime(&time_stamp);
	if (NULL == local_time)
	{
		return 0;
	}
#else	
	struct tm local_time_item;
	struct tm *local_time = &local_time_item;

	if (NULL == ::localtime_r(&time_stamp, &local_time_item))
	{
		return 0;
	}
#endif

	time_t zero_time = time_stamp - local_time->tm_sec;
	return zero_time;
}

/**
 * 输入格式：2015-08-25 12:30:00
 * 输出时间戳
 */
bool ConvertTimeStringToUnixTime(const char *time_str, time_t *timestamp)
{
	if (NULL == time_str || NULL == timestamp) return false;

	char temp[256] = {0};
	STRNCPY(temp, time_str, sizeof(temp));

	if (strlen(temp) < 19) return false;

	struct tm local_tm;
	memset((void*)&local_tm, 0, sizeof(local_tm));

	char *pt = &temp[0];

	pt[4] = '\0';
	local_tm.tm_year = atoi(pt) - 1900; if (local_tm.tm_year <= 70 || local_tm.tm_year > 200) return false;
	pt += 5;

	pt[2] = '\0';
	local_tm.tm_mon = atoi(pt) - 1; if (local_tm.tm_mon < 0 || local_tm.tm_mon > 11) return false;
	pt += 3;

	pt[2] = '\0';
	local_tm.tm_mday = atoi(pt); if (local_tm.tm_mday < 1 || local_tm.tm_mday > 31) return false;
	pt += 3;

	pt[2] = '\0';
	local_tm.tm_hour = atoi(pt); if (local_tm.tm_hour < 0 || local_tm.tm_hour > 23) return false;
	pt += 3;

	pt[2] = '\0';
	local_tm.tm_min = atoi(pt); if (local_tm.tm_min < 0 || local_tm.tm_min > 59) return false;
	pt += 3;

	pt[2] = '\0';
	local_tm.tm_sec = atoi(pt); if (local_tm.tm_sec < 0 || local_tm.tm_sec > 59) return false;
	pt += 3;

	*timestamp = mktime(&local_tm);

	return true;
}

/**
 * 输入格式：2015-08-25
 * 输出时间戳
 */
bool ConvertTimeStringToUnixTime2(const char *time_str, time_t *timestamp)
{
	if (NULL == time_str || NULL == timestamp) return false;

	char temp[12] = {0};
	STRNCPY(temp, time_str, sizeof(temp));

	if (strlen(temp) < 10) return false;

	struct tm local_tm;
	memset((void*)&local_tm, 0, sizeof(local_tm));

	char *pt = &temp[0];

	pt[4] = '\0';
	local_tm.tm_year = atoi(pt) - 1900; if (local_tm.tm_year <= 70 || local_tm.tm_year > 200) return false;
	pt += 5;

	pt[2] = '\0';
	local_tm.tm_mon = atoi(pt) - 1; if (local_tm.tm_mon < 0 || local_tm.tm_mon > 11) return false;
	pt += 3;

	pt[2] = '\0';
	local_tm.tm_mday = atoi(pt); if (local_tm.tm_mday < 1 || local_tm.tm_mday > 31) return false;
	pt += 3;

	local_tm.tm_hour = 0;
	local_tm.tm_min = 0;
	local_tm.tm_sec = 0;

	*timestamp = mktime(&local_tm);

	return true;
}

bool ConvertUniqueKeyStrToNumber(UniqueKeyStr unique_key_str, ARG_OUT long long* unique_key)
{
	std::stringstream ss;
	ss << unique_key_str;
	ss >> (*unique_key);
	return !ss.fail();
}

bool ConvertUniqueKeyNumberToStr(long long unique_key, ARG_OUT UniqueKeyStr unique_key_str)
{
	std::stringstream ss;
	ss << unique_key;
	ss >> unique_key_str;
	return !ss.fail();
}

int RandomNum(int max_num)
{
	return RandomNum(0, max_num);
}

// 注意随机范围为开区间 [min_num, max_num)
int RandomNum(int min_num, int max_num)
{
	if (min_num > max_num)
	{
		int tmp_num = min_num;
		min_num = max_num;
		max_num = tmp_num;
	}

	int interval_num = max_num - min_num;
	if (interval_num <= 0)
	{
		return 0;
	}

	if (interval_num < RAND_MAX)
	{
		return min_num + (rand() % interval_num);
	}
	else
	{
		int rand_num = (rand() << 16) + rand(); // 两次随机数补成一个int（主要解决windows下rand最大值只有2^15的问题）
		if (rand_num < 0)
		{
			rand_num *= -1;
		}
		return min_num + int(rand_num % interval_num);
	}
}

namespace gamecommon
{
	const Posi GetDisRandPos(const Posi &curr_pos, int rand_dis)
	{
		if (rand_dis <= 0) return curr_pos;

		int x = curr_pos.x + (rand() % (rand_dis * 2) - rand_dis);
		int y = curr_pos.y + (rand() % (rand_dis * 2) - rand_dis);

		return Posi(x, y);
	}

	// [min_rand_dis,max_rand_dis)
	const Posi GetDisRandRangePos(const Posi &curr_pos, int min_rand_dis, int max_rand_dis)
	{
		if (min_rand_dis <= 0 || max_rand_dis <= min_rand_dis) return curr_pos;
		int d = max_rand_dis - min_rand_dis;
		int x = curr_pos.x + (rand() % (d * 2) - d);
		int y = curr_pos.y + (rand() % (d * 2) - d);

		x = x < 0 ? x - min_rand_dis : x + min_rand_dis;
		y = y < 0 ? y - min_rand_dis : y + min_rand_dis;

		return Posi(x, y);
	}
}

int CalcDayIndexWithClockDayItem(ClockReturnDayItem *day1, ClockReturnDayItem *day2)
{
	if (NULL == day1 || NULL == day2)
	{
		return 0;
	}

	struct tm day1_tm, day2_tm;
	
	day1_tm.tm_year = day1->year;
	day1_tm.tm_mon = day1->mon;
	day1_tm.tm_mday = day1->day;
	day1_tm.tm_hour = 0;
	day1_tm.tm_min = 0;
	day1_tm.tm_sec = 1;

	day2_tm.tm_year = day2->year;
	day2_tm.tm_mon = day2->mon;
	day2_tm.tm_mday = day2->day;
	day2_tm.tm_hour = 0;
	day2_tm.tm_min = 0;
	day2_tm.tm_sec = 1;

	time_t day1_time = mktime(&day1_tm);
	time_t day2_time = mktime(&day2_tm);

	return GetDayIndex(day1_time, day2_time);
}

int GetGameNameHash(const GameName str)
{
	unsigned int seed = 1313; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	static const int MAX_LEN = sizeof(GameName);
	int i = 0;
	while (i < MAX_LEN && str[i] != 0)
	{
		hash = hash * seed + (unsigned char)str[i++];
	}
	return (int)(hash & 0x7FFFFFFF);
}

int GetStringHash(int max_len, const char *str)
{
	unsigned int seed = 1313; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	int i = 0;
	while (i < max_len && str[i] != 0)
	{
		hash = hash * seed + (unsigned char)str[i++];
	}
	return (int)(hash & 0x7FFFFFFF);
}

int GetStringHash(const char* str)
{
	int len = strlen(str);
	return GetStringHash(len, str);
}

int GetNumberOfOne(int n)
{
	int cnt = 0;
	while (n)
	{
		n = (n & (n - 1));
		++cnt;
	}
	return cnt;
}

const char * ItemConfigDataLog(const ItemConfigData *item_cfg, char log_str[LOG_BASE_LENGTH])
{
	static char defalut_log_str[LOG_BASE_LENGTH]; memset(defalut_log_str, 0, sizeof(defalut_log_str));

	if (NULL == item_cfg)
	{
		return defalut_log_str;
	}

	char *out_log = log_str;
	if (NULL == out_log)
	{
		out_log = defalut_log_str;
	}

	SNPRINTF(out_log, LOG_BASE_LENGTH, "item_id:%d, num:%d, is_bind:%d", (int)item_cfg->item_id, item_cfg->num, (item_cfg->is_bind ? 1 : 0));

	return out_log;
}

const char* GetMin(const char* t1, const char* t2)
{
	return (strcmp(t1, t2) < 0) ? t1 : t2;	
}

const char* GetMax(const char* t1, const char* t2)
{
	return (strcmp(t1, t2) < 0) ? t2 : t1;
}

SysMsgType GetMsgType(int t)
{
	SysMsgType s;
	s.SetPosBinary(1 << t);
	return s;
}

SysMsgType GetMsgType(int t1, int t2)
{
	SysMsgType s;
	s.SetPosBinary((1 << t1) | (1 << t2));
	return s;
}

SysMsgType GetMsgType(int t1, int t2, int t3)
{
	SysMsgType s;
	s.SetPosBinary((1 << t1) | (1 << t2) | (1 << t3));
	return s;
}

int GetBaseProf(int profession)
{
	return profession / PROFESSION_BASE;
}

int CompatibleGetProfType(int profession)
{
	if (profession >= PROFESSION_BASE)
	{
		return GetBaseProfession(profession);
	}
	else
	{
		return profession * PROFESSION_BASE;
	}

	return PROF_TYPE_INVALID;
}

int GetAdvanceTimes(int profession)
{
	return profession % PROFESSION_BASE;
}

int GetBaseProfession(int profession)
{
	return profession / PROFESSION_BASE * PROFESSION_BASE;
}



std::string STRING_SPRINTF(const char * format, ...)
{
	std::string str;
	if (NULL == format)
		return str;

	va_list va;
	va_start(va, format);
	int length = vsnprintf(NULL, 0, format, va);
	va_end(va);

	if (length <= 0)
		return str;

	length += 1;
	char * buf = new char[length];
	va_start(va, format);
	vsnprintf(buf, length, format, va);
	va_end(va);
	
	str = buf;

	delete[] buf;
	return str;
}

///////////////////////////////////////////////匹配逻辑///////////////////////////////////////////////
MatchLogic::MatchLogic()
{
	team_key = 0;
}

MatchLogic::~MatchLogic()
{
	this->Reset();
}

void MatchLogic::Reset()
{
	for (int i = 0; i < MAX_TEAM_MEMBER_NUM + 1; i++)
	{
		for (std::map<int, MatchTeamData*>::iterator it = m_team_list[i].begin(); it != m_team_list[i].end(); ++it)
		{
			delete it->second;
			it->second = NULL;
		}
		m_team_list[i].clear();
	}

	for (std::map<int, MatchUserData*>::iterator it = m_user_map.begin(); it != m_user_map.end(); ++it)
	{
		delete it->second;
		it->second = NULL;
	}
	m_user_map.clear();

	team_key = 0;
}

int MatchLogic::OnMatchReq(int count, int uid_list[MAX_TEAM_MEMBER_NUM], int level[MAX_TEAM_MEMBER_NUM])
{
	if (count <= 0 || count > MAX_TEAM_MEMBER_NUM) return -1;

	bool is_join = false;
	for (int i = MAX_TEAM_MEMBER_NUM - count; i >= 1; --i)
	{
		std::map<int, MatchTeamData*>::iterator it = m_team_list[i].begin();
		if (it != m_team_list[i].end())
		{
			MatchTeamData* team = it->second;
			if (NULL == team)
			{
				m_team_list[i].erase(it);
				continue;
			}

			for (int j = 0; j < count; j++)
			{
				MatchUserData* temp_data = new MatchUserData();
				temp_data->uid = uid_list[j];
				temp_data->level = level[j];

				m_user_map[uid_list[j]] = temp_data;
				team->AddMember(temp_data);
			}

			this->PutTeamMap(team);

			delete it->second;
			m_team_list[i].erase(it);
			is_join = true;

			break;
		}
	}

	if (!is_join)
	{
		team_key++;
		if (team_key <= 0)team_key = 1;

		MatchTeamData* team_data = new MatchTeamData();
		for (int i = 0; i < count; i++)
		{
			MatchUserData* temp_data = new MatchUserData();
			temp_data->uid = uid_list[i];
			temp_data->level = level[i];

			m_user_map[uid_list[i]] = temp_data;
			team_data->AddMember(temp_data);
		}

		this->PutTeamMap(team_data);
	}

	return 0;
}

int MatchLogic::OnCancelReq(int count, int uid_list[MAX_TEAM_MEMBER_NUM])
{
	for (int i = 0; i < count; i++)
	{
		int uid = uid_list[i];
		std::map<int, MatchUserData*>::iterator it = m_user_map.find(uid);
		if (it == m_user_map.end()) continue;
		if (NULL == it->second)
		{
			m_user_map.erase(it);
			continue;
		}
		MatchTeamData* team = it->second->team;
		if (NULL == team) continue;

		std::map<int, MatchTeamData*>::iterator team_it = m_team_list[team->count].find(team->team_key);
		if (team_it != m_team_list[team->count].end())
		{
			delete team_it->second;
			m_team_list[team->count].erase(team_it);
		}

		team->DecMember(it->second);
		this->PutTeamMap(team);
		m_user_map.erase(it);
	}

	return 0;
}

int MatchLogic::Logic(std::vector<FightGroup>& fight_group)
{
	std::vector<FightGroup> temp;
	for (std::map<int, MatchTeamData*>::iterator it = m_team_list[MAX_TEAM_MEMBER_NUM].begin(); it != m_team_list[MAX_TEAM_MEMBER_NUM].end(); ++it)
	{
		if (NULL == it->second) continue;

		bool is_join = false;
		for (std::vector<FightGroup>::iterator temp_it = temp.begin(); temp_it != temp.end(); ++temp_it)
		{
			MatchTeamData* temp_team = temp_it->team[0];
			if (NULL == temp_team) continue;

			if (!m_func(temp_team, it->second))continue;

			temp_it->count = 2;
			temp_it->team[1] = it->second;

			is_join = true;
			fight_group.push_back(*temp_it);
			temp.erase(temp_it);
			break;
		}
		if (!is_join)
		{
			FightGroup new_group;
			new_group.count = 1;
			new_group.team[0] = it->second;
			temp.push_back(new_group);
		}
	}
	return 0;
}

int MatchLogic::Match(std::vector<ActFightGroup>& ret_group)
{
	int team_count = m_team_list[MAX_TEAM_MEMBER_NUM].size();
	if ((team_count / 2) == 0) return 0;

	std::vector<FightGroup> fight_group;
	this->Logic(fight_group);

	for (std::vector<FightGroup>::iterator it = fight_group.begin(); it != fight_group.end(); ++it)
	{
		if (it->count != 2) continue;
		if (NULL == it->team[0] || NULL == it->team[1]) continue;

		ActFightGroup temp_group;
		for (int j = 0; j < 2; j++)
		{
			std::map<int, MatchTeamData*>::iterator team_it = m_team_list[MAX_TEAM_MEMBER_NUM].find(it->team[j]->team_key);
			if (NULL != team_it->second)
			{
				for (int i = 0; i < MAX_TEAM_MEMBER_NUM; i++)
				{
					if (NULL == team_it->second->user_list[i]) continue;

					std::map<int, MatchUserData*>::iterator it = m_user_map.find(team_it->second->user_list[i]->uid);
					if (it != m_user_map.end())
					{
						temp_group.user_list[j][i] = team_it->second->user_list[i]->uid;

						delete it->second;
						it->second = NULL;
						m_user_map.erase(it);
					}
				}

				delete team_it->second;
				team_it->second = NULL;
			}

			m_team_list[MAX_TEAM_MEMBER_NUM].erase(team_it);
		}

		ret_group.push_back(temp_group);
	}

	return 0;
}

void MatchLogic::PutTeamMap(MatchTeamData * team)
{
	if (NULL == team) return;
	if (team->count <= 0 || team->count > MAX_TEAM_MEMBER_NUM) return;

	team_key++;
	if (team_key <= 0)team_key = 1;

	m_team_list[team->count][team_key] = team;
	team->team_key = team_key;
}

const char * g_GetRetrieveReasonTypeStr(int reason)
{
	if (reason >= 0 && reason < RETRIEVE_REASON_MAX
		&& reason < int(sizeof(gs_retrieve_reason_str_list) / sizeof(gs_retrieve_reason_str_list[0])))
	{
		return gs_retrieve_reason_str_list[reason].str;
	}

	return "retrieve_reason_unknown";
};

std::vector<std::string> ParseStringByDelimiter(const char* str, const char* delimiter)
{
	std::vector<std::string> results;

	static char temp[1024] = { 0 };  // 必须是static  

	STRNCPY(temp, str, sizeof(temp));

	char *token = strtok(temp, delimiter);

	while (NULL != token)
	{
		results.push_back(token);
		token = strtok(NULL, delimiter);
	}

	return results;
}

bool ReadHotfixFile(const std::string& path)
{
#ifndef __unix
	return true;
#endif

	std::ifstream hotfix_file;
	hotfix_file.open((path + "/" + HOTFIX_FILE_NAME).c_str(), std::ios::in);
	if (!hotfix_file)
	{
		return true; // 读不到就不热更
	}

	printf("ReadHotfixFile from [%s]\n", (path + "/" + HOTFIX_FILE_NAME).c_str());

	bool succ = false;
	char line[1024] = { 0 };
	while (hotfix_file.getline(line, ARRAY_ITEM_COUNT(line)))
	{
		line[sizeof(line) - 1] = 0;
		std::vector<std::string> results = ParseStringByDelimiter(line, " ");
		if (2 != results.size())
		{
			printf("ReadHotfixFile failed, param error!! file[%s]\n", (path + "/" + HOTFIX_FILE_NAME).c_str());
			succ = false;
			break; // 热更文件内容有误就返回false让上层停止程序
		}

		std::string so_file_name = results[0];
		std::string symbol_name = results[1];

#ifdef __unix
		std::string error_msg;
		succ = cmd_fix_2(path + "/" + so_file_name, symbol_name, error_msg);
		if (!succ)
		{
			printf("ReadHotfixFile failed, so_name[%s] symbol_name[%s] error_msg[%s]\n", 
				so_file_name.c_str(), symbol_name.c_str(), error_msg.c_str());
			break;
		}
#endif

	}

	hotfix_file.close();
	return succ;
}

#ifdef _MSC_VER
#include <sys/timeb.h>
#else
#include <sys/time.h>
#endif
unsigned long long GetNowTime_MS()
{
#ifdef _MSC_VER
	timeb t;
	ftime(&t);//获取毫秒
	return 1LL * t.time * 1000 + t.millitm;
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	return 1LL * time.tv_sec * 1000 + 1LL * time.tv_usec / 1000;
#endif
}

std::string GetRoleNameWithServerId(const char* name, int server_id)
{
	std::string origin_name(name);
	if (origin_name.find_last_of("_s") != std::string::npos)
	{
		return origin_name;
	}
	else
	{
		return STRING_SPRINTF("%s_s%d", name, server_id);
	}
}

bool IsStringAllNumber(const std::string& str)
{
	if (str.empty()) return false;

	for (size_t i = 0; i < str.size(); ++i)
	{
		if (str[i] < '0' || str[i] > '9')
		{
			return false;
		}
	}

	return true;
}

bool IsStringMatchVersionStr(const std::string& version_str)
{
	if (version_str.size() != 10) return false;
	if (version_str[0] != '2' || version_str[1] != '0') return false;

	return true;
}

bool GetVersionStrFromPathStr(std::string* version_str, const std::string& path)
{
	// 先按 / 分割字符串
	std::vector<std::string> strs = ParseStringByDelimiter(path.c_str(), "/");

	for (size_t i = 0; i < strs.size(); ++i)
	{
		// 版本号的格式必是10个数字
		if (strs[i].size() != 10) continue;
		if (!IsStringAllNumber(strs[i])) continue;
		if (!IsStringMatchVersionStr(strs[i])) continue;

		if (NULL != version_str) *version_str = strs[i];
		
		return true;
	}

	return false;
}

bool GetVersionStr(std::string* version_str)
{
	std::string dir_str = GetCurrentWorkingDirectory();
	return GetVersionStrFromPathStr(version_str, dir_str);
}

std::string GetCurrentWorkingDirectory()
{
	char buff[1024] = { 0 };
#ifdef _MSC_VER
	_getcwd(buff, sizeof(buff));
#else
	getcwd(buff, sizeof(buff));
#endif
	std::string current_working_directory(buff);
	return current_working_directory;
}

