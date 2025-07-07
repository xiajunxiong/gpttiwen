
VIEW_REQ("modules/serveractivity/da_zhan_hong_tu/da_zhan_hong_tu_view")

DaZhanHongTuCtrl = DaZhanHongTuCtrl or BaseClass(BaseCtrl)
function DaZhanHongTuCtrl:__init()
    if DaZhanHongTuCtrl.Instance ~= nil then
        Debuger.LogError("[DaZhanHongTuCtrl] attempt to create singleton twice!")
        return
    end
	self.data = DaZhanHongTuData.New()

	--大展宏兔
    self:RegisterProtocol(SCRADaZhanHongTuInfo, "OnSCRADaZhanHongTuInfo")
	ActivityRandData.Instance:Register(Mod.RandActivity.DaZhanHongTu, function ()
		return self.data:ActivityIsOpen()
	end,self.data.open_sm_data)
	Remind.Instance:Register(Mod.RandActivity.DaZhanHongTu, self.data.sm_data, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DA_ZHAN_HONG_TU) then
            return 0
        end
        return self.data:DaZhanHongTuRewardCanGet() and 1 or 0
    end)
end

function DaZhanHongTuCtrl:OnInit()
	EventSys:Bind(GlobalEventKeys.RoleNoticeChange, BindTool.Bind(self.OnRoleNoticeChange, self))
end

function DaZhanHongTuCtrl:OnRoleNoticeChange()
	DaZhanHongTuCtrl.GetDaZhanHongTuInfo()
end

-- 大展宏兔
function DaZhanHongTuCtrl:OnSCRADaZhanHongTuInfo(protocol)
    self.data:SetDaZhanHongTuInfo(protocol)
end

function DaZhanHongTuCtrl.SendActivityReq(op_type)
	local param = {}
	param.rand_activity_type = ACTIVITY_TYPE.RAND_DA_ZHAN_HONG_TU
	param.opera_type = op_type
	ServerActivityCtrl.Instance:SendRandActivityOperaReq(param)
end

function DaZhanHongTuCtrl.GetDaZhanHongTuInfo()
	DaZhanHongTuCtrl.SendActivityReq(0)
end

function DaZhanHongTuCtrl.SendGetReward()
	ViewMgr:CloseView(DaZhanHongTuView)
	DaZhanHongTuCtrl.SendActivityReq(1)
end
----------------------------------------------------
DaZhanHongTuData = DaZhanHongTuData or BaseClass()
function DaZhanHongTuData:__init()
	if DaZhanHongTuData.Instance ~= nil then
		Debuger.LogError("[DaZhanHongTuData] attempt to create singleton twice!")
		return
	end
    DaZhanHongTuData.Instance = self
	self.sm_data = SmartData.New({flush = false})
	self.open_sm_data = SmartData.New({val = false})
end

function DaZhanHongTuData:GetHasRechargeNum()
	if self.da_zhan_hong_tu_info then
		return self.da_zhan_hong_tu_info.chong_zhi_num or 0
	end
	return 0
end

function DaZhanHongTuData:ActivityIsOpen()
	if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DA_ZHAN_HONG_TU) then
		return self.da_zhan_hong_tu_info.fetch_flag == 0
	else
		return false
	end
end

function DaZhanHongTuData:SetDaZhanHongTuInfo(protocol)
	self.da_zhan_hong_tu_info = self.da_zhan_hong_tu_info or {}
	self.da_zhan_hong_tu_info.fetch_flag = protocol.fetch_flag
	self.da_zhan_hong_tu_info.chong_zhi_num = protocol.chong_zhi_num
	self.sm_data:SetDirty("flush")
	self.open_sm_data:SetDirty("val")
end

function DaZhanHongTuData:DaZhanHongTuRewardCanGet()
	if self.da_zhan_hong_tu_info == nil or self.da_zhan_hong_tu_info.chong_zhi_num == nil or self.da_zhan_hong_tu_info.fetch_flag == 1 then
        return false
	elseif self.da_zhan_hong_tu_info.fetch_flag == 1 then
		return false
    end
    return self.da_zhan_hong_tu_info.chong_zhi_num >= Config.dazhanhongtu_auto.reward[1].acc_price
end