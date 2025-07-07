--寅虎利是
FILE_REQ("modules/serveractivity/tiger_year/tiger_mora/tiger_mora_data")
VIEW_REQ("modules/serveractivity/tiger_year/tiger_mora/tiger_mora_view")

TigerMoraCtrl = TigerMoraCtrl or BaseClass(BaseCtrl)
function TigerMoraCtrl:__init()
    if TigerMoraCtrl.Instance ~= nil then
        Debuger.LogError("[TigerMoraCtrl] attempt to create singleton twice!")
        return
    end
    TigerMoraCtrl.Instance = self
    self.data = TigerMoraData.New()
    self:RegisterAllProtocols()
end

function TigerMoraCtrl:__delete()
    TigerMoraCtrl.Instance = nil
    Delete(self, "data")
end

function TigerMoraCtrl:OnInit()
    Remind.Instance:Register(Mod.TigerYear.TigerMora, self.data.act_info, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TIGER_MORA) then
            return 0
        end
    	return self.data:GetRemindNum()
    end)
end

function TigerMoraCtrl:OnUnloadGameLogic()
    self.data:ClearAllInfo()
end

function TigerMoraCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRATigerTeaseNewYearInfo, "OnSCRATigerTeaseNewYearInfo")
end

function TigerMoraCtrl:SendActInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIGER_MORA, 0)
end

--猜拳
function TigerMoraCtrl:SendPK()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIGER_MORA, 1)
end

function TigerMoraCtrl:OnSCRATigerTeaseNewYearInfo(protocol)
    if protocol.is_win ~= 0 then
        local left_pic = GameMath.Ramdon(1, 3)
        local right_pic = protocol.is_win == 1 and left_pic + 1 or left_pic - 1
        if right_pic == 0 then
            right_pic = 3
        elseif right_pic == 4 then
            right_pic = 1
        end
        self.data:SetLastGameInfo(protocol.is_win, left_pic, right_pic)
        Invoke(BindTool.Bind1(self.ShowGetItems, self), 1.5)
    end
    self.data:SetActInfo(protocol)
end

function TigerMoraCtrl:ShowGetItems()
    local show_list = self.data:GetDelayItems()
    if show_list == nil then
        return
    end
    for k, v in ipairs(show_list) do
        if Item.GetUseType(v.item_id) == ItemUseType.CurrencyItem then
            local co = Item.GetConfig(v.item_id)
            PublicPopupCtrl.Instance:CenterV {type = co.param1, num = co.param2 * v.num}
        else
            PublicPopupCtrl.Instance:CenterI({item_id = v.item_id, num = v.num, color_str = v:ColorStr()})
            ChatCtrl.Instance:ChannelChatCustom {item_get = {item = v, num = v.num}}
        end
    end
    ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success, show_list)
    self.data:ReleaseNeedDelayItem()
end
