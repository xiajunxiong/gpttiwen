--复刻->山海宝盒 ACTIVITY_TYPE.RAND_SEA_GIFT_BOX
FILE_REQ("modules/serveractivity/sea_gift_box/sea_gift_box_data")
VIEW_REQ("modules/serveractivity/sea_gift_box/sea_gift_box_view")

SeaGiftBoxCtrl = SeaGiftBoxCtrl or BaseClass(BaseCtrl)
function SeaGiftBoxCtrl:__init()
    if SeaGiftBoxCtrl.Instance ~= nil then
        Debuger.LogError("[SeaGiftBoxCtrl] attempt to create singleton twice!")
        return
    end
    SeaGiftBoxCtrl.Instance = self
    self.data = SeaGiftBoxData.New()

    self:RegisterAllProtocols()
end

function SeaGiftBoxCtrl:__delete()
    if SeaGiftBoxCtrl.Instance == self then
        SeaGiftBoxCtrl.Instance = nil
    end
end

function SeaGiftBoxCtrl:OnInit()
    --初始化完毕操作
    Remind.Instance:Register(Mod.MountainSea.SeaGiftBox, self.data.gift_box_data, function()
		return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SEA_GIFT_BOX) and self.data:GetRemindNum() or 0
	end)
end

function SeaGiftBoxCtrl:OnUnloadGameLogic()
    --切换角色操作
end

function SeaGiftBoxCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAShanHaiBaoHeInfo, "OnSCRAXianShiBaoHeInfo")
end

--限时宝盒
function SeaGiftBoxCtrl:OnSCRAXianShiBaoHeInfo(protocol)
    self.data:SetGiftBoxInfo(protocol)
    -- self.data:SetBonusNum(ACTIVITY_TYPE.RAND_SEA_GIFT_BOX, protocol.flag_num, #self.data:GetGiftBoxShowList())
end
