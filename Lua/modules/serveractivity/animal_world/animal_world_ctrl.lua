FILE_REQ("modules/serveractivity/animal_world/animal_world_data")

VIEW_REQ("modules/serveractivity/animal_world/animal_world_view")
VIEW_REQ("modules/serveractivity/animal_world/animal_world_detail_view")

AnimalWorldCtrl = AnimalWorldCtrl or BaseClass(BaseCtrl)

function AnimalWorldCtrl:__init()
    if AnimalWorldCtrl.Instance ~= nil then
        Debuger.LogError("[AnimalWorldCtrl] attempt to create singleton twice!")
        return
    end
    AnimalWorldCtrl.Instance = self
    self.data = AnimalWorldData.New()
end

function AnimalWorldCtrl:OnInit()
    self:OnRemind()
    self:RegisterAllProtocols()
end

function AnimalWorldCtrl:OnRemind()
    local main_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.AnimalWorld.Main,
        self.data.info_data,
        main_func
    )
    -- 主界面图标
    ActivityRandData.Instance:Register(Mod.AnimalWorld.Main, function()
		return self.data:IsOpen()
	end, self.data.info_data)
end

function AnimalWorldCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCGodBeastAdventInfo, "OnSCGodBeastAdventInfo")
    --self:RegisterProtocol(SCGodBeastAdventOpenInfo, "OnSCGodBeastAdventOpenInfo")
    self:RegisterProtocol(SCGodBeastAdventBuyInfo, "OnSCGodBeastAdventBuyInfo")
    self:RegisterProtocol(SCGodBeastAdventNotifyInfo, "OnSCGodBeastAdventNotifyInfo")
end

function AnimalWorldCtrl:__delete()
    --注销网络事件
    self:UnRegisterProtocol(SCGodBeastAdventInfo)
    self:UnRegisterProtocol(SCGodBeastAdventOpenInfo)
    self:UnRegisterProtocol(SCGodBeastAdventBuyInfo)
    self:UnRegisterProtocol(SCGodBeastAdventNotifyInfo)
    RemindCtrl.Instance:Remind():UnRegister(Mod.AnimalWorld.Main)
    if AnimalWorldCtrl.Instance == self then
        AnimalWorldCtrl.Instance = nil
    end
end

function AnimalWorldCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function AnimalWorldCtrl:FlushRemind()
    local num = self.data:AllRemind()
    return num
end

function AnimalWorldCtrl:Send(m_type, param1)
    self:MyLogError("Send",m_type, param1)
    ServerActivityCtrl.Instance:SendActivityReq(
        ACTIVITY_TYPE.RAND_ANIMAL_WORLD,
        m_type or AnimalWorldType.Info,
        param1 or 0)
end

function AnimalWorldCtrl:OnSCGodBeastAdventInfo(protocol)
    self:MyLogError("OnSCGodBeastAdventInfo", protocol)
    self.data:SetInfoData(protocol)
end

function AnimalWorldCtrl:OnSCGodBeastAdventOpenInfo(protocol)
    self:MyLogError("OnSCGodBeastAdventOpenInfo", protocol)
    self.data:SetBaseData(protocol)
end

function AnimalWorldCtrl:OnSCGodBeastAdventBuyInfo(protocol)
    self:MyLogError("OnSCGodBeastAdventBuyInfo", protocol)
    self.data:SetBuyCountData(protocol)
end

function AnimalWorldCtrl:OpenPetInfo()
    local pet_show_info = self.data:PetShowInfo()
    local pet_id = pet_show_info.pet_id
    local pet = Pet.New({pet_id = pet_id})
    ViewMgr:OpenView(AnimalWorldDetailView, pet)
end

function AnimalWorldCtrl:OnSCGodBeastAdventNotifyInfo(protocol)
    self:MyLogError("OnSCGodBeastAdventNotifyInfo", protocol)
    if protocol.add_like > 0 then
        PublicPopupCtrl.Instance:Center(Format(Language.AnimalWorld.AddSatisfyTip, protocol.add_like))
    end
end

function AnimalWorldCtrl:MyLogError(...)
    -- LogError(...)
end

-- 判断是否是异兽临世的交换道具
function AnimalWorldCtrl:IsAnimalItem(item_id)
    for k, v in pairs(self.data:GetItemListen()) do
        if item_id == v then
            return true
        end
    end
    return false
end

-- 判断是否需要上架、购买提示（1购买，2上架）
function AnimalWorldCtrl:IsNeedTip(index, item_id)
    -- 小于5天都要提示，写死
    local min_day = 5
    local str = Language.AnimalWorld.TimesTip[index]
    local has_time = math.floor(self.data:TimeInfo().end_time - TimeManager.GetServerTime())
    local time_struct = TimeManager.FormatDHMS(has_time)
    if time_struct.day < min_day then
        return true, Format(str, Item.GetName(item_id), time_struct.day, time_struct.hour)
    else
        return false, ""
    end
end