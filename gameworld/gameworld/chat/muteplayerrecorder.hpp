#ifndef __MUTE_PLAYER_RECORDER_HPP__
#define __MUTE_PLAYER_RECORDER_HPP__

#include <map>

class MutePlayerRecorder
{
public:
	typedef std::map<int, unsigned int> MutePlayerMap; // key:role_id value:unmute_timestamp

public:
	static MutePlayerRecorder& Instance();

	void Update(unsigned int now);

	bool IsMute(int role_id);
	void AddMutePlayer(int role_id, unsigned int unmute_timestamp);
	void ClearMutePlayer(int role_id);
private:
	MutePlayerRecorder();
	~MutePlayerRecorder();
	MutePlayerRecorder(const MutePlayerMap&);
	MutePlayerRecorder& operator= (const MutePlayerMap&);

	void CheckMuteTimeExpire(unsigned int now);

	MutePlayerMap m_mute_map;
	unsigned int m_next_check_expire_timestamp;

};

#endif