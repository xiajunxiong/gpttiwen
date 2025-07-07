LustrateNianView = LustrateNianView or DeclareView("LustrateNianView", "serveractivity/lustrate_nian_view", Mod.RandActivity.LustrateNian)
VIEW_DECLARE_LEVEL(LustrateNianView, ViewLevel.L1)

function LustrateNianView:LustrateNianView()
    self.Board:SetData(self:GetType(), ActivityRandData.GetName(ACTIVITY_TYPE.RAND_LUSTRATE_NIAN))
    self.Currency:DefaultSet()
end

LustrateNianPanel = LustrateNianPanel or DeclareMono("LustrateNianPanel", UIWFlushPanel)

function LustrateNianPanel:LustrateNianPanel()
    self.data = ServerActivityData.Instance
end

function LustrateNianPanel:onFlush()
    self.consume = self.data:GetNianConsume()
    local other = self.data:GetNianOther()
    self.Item:SetData(Item.Create(self.consume.item))
    UH.SetText(self.Price, self.consume.need_gold)
    UH.SpriteName(self.IconSp, ActHuoBi[self.consume.consume_type])
    UH.SetText(self.ItemName, self.Item:GetData():Name())
   self.UITime:CreateActTime(ACTIVITY_TYPE.RAND_LUSTRATE_NIAN,TimeType.Type_Time_3,Language.Nian.Time)

   local list = self.data:GetRewardShow()
   for i=1,3 do
       self.RewardList[i]:SetData(Item.Create(list[i].reward_item))
   end
   UH.SetText(self.DescTex, Language.Nian.Desc1)
   UH.SetText(self.CountTex, string.format( Language.Nian.Count, ServerActivityData.Instance.lusrate_nian_data.today_times, other.reward_frequency))
   self.is_limit = ServerActivityData.Instance.lusrate_nian_data.today_times >= other.reward_frequency
end

function LustrateNianPanel:OnClickToGo()
    --判断如果是跨服
    if self.is_limit then
        PublicPopupCtrl.Instance:Center(Language.Nian.LimitTip)
        return
    end
    if LoginData.Instance:IsOnCrossSever() then
        local info = {
            	content = Language.Nian.CrossTip,
            	confirm = {
            		func = function ()
                        local id = SceneData.Instance:SceneId()
                        local main = SceneLogic.Instance:MainCityID()
                        if id == main then
                            PublicPopupCtrl.Instance:Center(ErrorText[114])
                        else
                            SceneLogic.Instance:AutoToPos(main,nil,nil,nil,true)
                        end
                        LoginCtrl.Instance:SendBackToOriginServer()
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end,
            	},
            }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    local id = SceneData.Instance:SceneId()
    local main = SceneLogic.Instance:MainCityID()
    if self.is_limit then
        PublicPopupCtrl.Instance:Center(Language.Nian.LimitTip)
        return
    elseif id == main then
        PublicPopupCtrl.Instance:Center(ErrorText[114])
        return
    else
        SceneLogic.Instance:AutoToPos(main,nil,nil,nil,true)
    end
    ViewMgr:CloseView(LustrateNianView)
    ViewMgr:CloseView(PlanActivityView)
end

function LustrateNianPanel:OnClickBuy()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_LUSTRATE_NIAN
    param_t.opera_type = 0
    param_t.param_1 = self.consume.seq
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

function LustrateNianPanel:OnClickDesc()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[52].desc})
end