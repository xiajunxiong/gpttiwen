#ifndef __ROBOT_AI_PARAM_HPP_
#define __ROBOT_AI_PARAM_HPP_

struct RobotAIParam
{
	RobotAIParam()
	{
		this->Reset();
	}
	void Reset()
	{
		memset(data_buff, 0, sizeof(data_buff));
	}

	int data_buff[20];
};
#endif