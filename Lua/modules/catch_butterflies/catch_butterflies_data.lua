CatchButterfliesData = CatchButterfliesData or BaseClass()
function CatchButterfliesData:__init()
    if CatchButterfliesData.Instance ~= nil then
        Debuger.LogError("[CatchButterfliesData] attempt to create singleton twice!")
        return
    end
    CatchButterfliesData.Instance = self
    self.all_config = Config.xunmenghudie_auto
    self.other_info = self.all_config.other[1]
    self.info = SmartData.New({count = 0})
end
function CatchButterfliesData:SetCount(count)
    self.info.count = count
end
function CatchButterfliesData:ShowTime()
    return self.other_info.time
end
function CatchButterfliesData:Desc()
    return self.other_info.desc
end
function CatchButterfliesData:SceneID()
    return self.all_config.scene[1].scene_id
end
function CatchButterfliesData:NpcID()
    return self.all_config.scene[1].npc_id
end
function CatchButterfliesData:MaxCount()
    return self.other_info.frequency
end
function CatchButterfliesData:Count()
    return self.info.count
end
function CatchButterfliesData:Info()
    return self.info
end
function CatchButterfliesData:IsCantCatch()
    return self:Count() >= self:MaxCount()
end
function CatchButterfliesData:GetActReward()
	local day_num = TimeCtrl.Instance:GetActOpenServerDay(ACTIVITY_TYPE.CATCH_BUTTERFLY)
	local cfg = self.all_config.reward
	for i,v in pairs(cfg) do
		if day_num >= v.section_start and (day_num <= v.section_end or v.section_end == 0) then
			return DataHelper.FormatItemList(v.reward_item)
	    end
	end
end
function CatchButterfliesData:GetActTime()
    local act_type = ACTIVITY_TYPE.CATCH_BUTTERFLY
    local start = ActivityData.GetStartStampTime(act_type)
    local end_time = ActivityData.GetSwitchStampTime(act_type)
    end_time = end_time - 1
    if start == 0 and end_time == 0 then
        LogError("活动未开启或者已结束")
        return Language.Butterfly.Close
    else
        local s = os.date("*t",start)
        local e = os.date("*t",end_time)
        return string.format(Language.Butterfly.Time, s.month, s.day, e.month, e.day)
    end
end
function CatchButterfliesData:SetObjVo(obj_vo)
    self.obj_vo = obj_vo
end
function CatchButterfliesData:ObjVo()
    return self.obj_vo
end