GuildRedbag = GuildRedbag or DeclareView("GuildRedbag","guild/guild_redbag/guild_redbag",Mod.Guild.GuildRedbag)
VIEW_DECLARE_LEVEL(GuildRedbag, ViewLevel.L1)

function GuildRedbag:LoadCallback()
    UH.SetText(self.LeftTex,Language.Guild.HongBaoLeftTabText)
    UH.SetText(self.RightTex,Language.Guild.HongBaoRightTabText)
    UH.SetText(self.NotLeftTex,Language.Guild.HongBaoNotLeftText)
    UH.SetText(self.NotRightTex,Language.Guild.HongBaoNotRightText)
end
function GuildRedbag:OnClickDesc()
  PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[GuildHongBaoConfig.DescIndex].desc})
end
function GuildRedbag:OnClickClose()
    ViewMgr:CloseView(GuildRedbag)
    --ViewMgr:OpenView(GuildView)
end

GuildRedbagPanel = GuildRedbagPanel or DeclareMono("GuildRedbagPanel", UIWFlushPanel)
function GuildRedbagPanel:GuildRedbagPanel()
  self.data = GuildRedbagData.Instance
  self.data_cares = {
      {data = self.data:GetHongBaoGetList(), func = self.FlushHongBaoGetList, init_call = false},
      {data = self.data:GetHongBaoSendList(), func = self.FlushHongBaoSendList, init_call = false}
  }
  self:FlushHongBaoGetList()
  self:FlushHongBaoSendList()
  ---延迟0.1秒
  TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayAnim, self),0.05)
end

function GuildRedbagPanel:PlayAnim()  
  self.ShowAnim:Play()
end
--刷新领取列表
function GuildRedbagPanel:FlushHongBaoGetList() 
    local list1 = self.data:GetHongBaoGetList()
    if list1.count ~= nil then
      self.GetList:SetDataList(list1.redbag_get_list)
      if list1.count ~= 0 then
        self.NotLq:SetActive(false)
      else
        self.NotLq:SetActive(true)
      end
    end   
end
--刷新可发红包列表
function GuildRedbagPanel:FlushHongBaoSendList()
    local list2 = self.data:GetHongBaoSendList()
    if list2.count ~= nil then
      self.SendList:SetDataList(list2.redbag_send_list)
      if list2.count ~= 0 then
          self.NotRb:SetActive(false)
      else
          self.NotRb:SetActive(true)
      end
    end
end

-------------------------------------
HongBaoItem = HongBaoItem or DeclareMono("HongBaoItem", UIWidgetBaseItem)
function HongBaoItem:HongBaoItem()
    
end
function HongBaoItem:SetData(data)
    UH.RenameGame(self,Format("item_%s",data.index))
    if self.IsSendType then
      for i=1,4 do 
        self.Objs[i]:SetActive(false)
      end
      data.icon_type = GuildHongBaoConfig.IconByType[data.type]
      self.Objs[data.icon_type]:SetActive(true)
    else
      UH.SetText(self.NameDesc,string.format(Language.Guild.HongBaoNameDesc,data.send_name))
    end
    self.info = data
    UIWidgetBaseItem.SetData(self,data)
end
--打开红包
function HongBaoItem:OnClickHongBaoGetItem()
   ---检测在跨服
   --if LoginCtrl.Instance.data:BaseData().is_cross then 
   --   PublicPopupCtrl.Instance:Center(Language.Guild.HongBaoCrossGetTip)
   --else
     if GuildRedbagData.Instance:IsCanUseHongBao() then 
       GuildRedBagCtrl.GetGuildHongBaoByID(self.info.hong_bao_id)
       GuildRedbagData.Instance:SetCurGetHongBaoIdInfo(self.info)
       ViewMgr:OpenView(OpenRedbagView)
     else
       PublicPopupCtrl.Instance:Center(Language.Guild.HongBaoCannotGetTip)
     end
   --end
end

function HongBaoItem:OnClickHongBaoSendItem()
   ---检测在跨服
   --if LoginCtrl.Instance.data:BaseData().is_cross then 
   --  PublicPopupCtrl.Instance:Center(Language.Guild.HongBaoCrossSendTip)
   --else
   if GuildRedbagData.Instance:IsCanUseHongBao() then 
     local type = self.info.type
     GuildRedbagData.Instance:SetSendHongBaoType(type)
     ViewMgr:OpenView(SendRedbagView)
   else
     PublicPopupCtrl.Instance:Center(Language.Guild.HongBaoCannotSendTip)
   end
  --end
end