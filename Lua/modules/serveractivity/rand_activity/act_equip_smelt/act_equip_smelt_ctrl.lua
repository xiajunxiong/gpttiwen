VIEW_REQ("modules/serveractivity/rand_activity/act_equip_smelt/act_equip_smelt_data")
VIEW_REQ("modules/serveractivity/rand_activity/act_equip_smelt/act_equip_smelt_view")
VIEW_REQ("modules/serveractivity/rand_activity/act_equip_smelt/act_equip_smelt_show_view")

ActEquipSmeltCtrl = ActEquipSmeltCtrl or BaseClass(BaseCtrl)
function ActEquipSmeltCtrl:__init()
    if ActEquipSmeltCtrl.Instance ~= nil then
        Debuger.LogError("[ActEquipSmeltCtrl] attempt to create singleton twice!")
        return
    end
    ActEquipSmeltCtrl.Instance = self
    self.data = ActEquipSmeltData.New()
    self:RegisterAllProtocols()
end

function ActEquipSmeltCtrl:__delete()
    ActEquipSmeltCtrl.Instance = nil
    Delete(self, "data")
end

function ActEquipSmeltCtrl:OnInit()
    RemindCtrl.Instance:Remind():Register(Mod.RandActivity.ActEquipSmelt, self.data.act_info, function()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_EQUIP_SMELT) and self.data:GetRemindNum() or 0
	end)
end

function ActEquipSmeltCtrl:OnUnloadGameLogic()
    --切换角色操作
end

function ActEquipSmeltCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRARongLianYouLiInfo, "OnSCRARongLianYouLiInfo")
end

function ActEquipSmeltCtrl:OnSCRARongLianYouLiInfo(protocol)
    self.data:SetActInfo(protocol)
end

function ActEquipSmeltCtrl:SendFetchTaskReward(seq)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_EQUIP_SMELT, 1, seq)
end

function ActEquipSmeltCtrl:SendFetchStage()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_EQUIP_SMELT, 2)
end
