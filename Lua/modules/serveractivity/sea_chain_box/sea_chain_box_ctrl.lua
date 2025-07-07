--复刻->山海宝袋 ACTIVITY_TYPE.RAND_SEA_CHAIN_BOX
FILE_REQ("modules/serveractivity/sea_chain_box/sea_chain_box_data")
VIEW_REQ("modules/serveractivity/sea_chain_box/sea_chain_box_view")

SeaChainBoxCtrl = SeaChainBoxCtrl or BaseClass(BaseCtrl)
function SeaChainBoxCtrl:__init()
    if SeaChainBoxCtrl.Instance ~= nil then
        Debuger.LogError("[SeaChainBoxCtrl] attempt to create singleton twice!")
        return
    end
    SeaChainBoxCtrl.Instance = self
    self.data = SeaChainBoxData.New()

    self:RegisterAllProtocols()
end

function SeaChainBoxCtrl:__delete()
    if SeaChainBoxCtrl.Instance == self then
        SeaChainBoxCtrl.Instance = nil
    end
end

function SeaChainBoxCtrl:OnInit()
    --初始化完毕操作
    Remind.Instance:Register(Mod.MountainSea.SeaChainBox, self.data.chain_box_data, function()
    	return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SEA_CHAIN_BOX) and self.data:GetRemindNum() or 0
	end)
end

function SeaChainBoxCtrl:OnUnloadGameLogic()
    --切换角色操作
end

function SeaChainBoxCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAShanHaiBaoDaiInfo, "OnSCRAXianShiBaoDaiInfo")
end

function SeaChainBoxCtrl:OnSCRAXianShiBaoDaiInfo(protocol)
    self.data:SetChainBoxInfo(protocol)
    -- self.data:SetBonusNum(ACTIVITY_TYPE.RAND_SEA_CHAIN_BOX, protocol.flag_num, #self.data:GetChainBoxShowList())
end
