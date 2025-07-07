---------------打开红包界面---------------
OpenRedbagView = OpenRedbagView or DeclareView("OpenRedbagView","guild/guild_redbag/open_redbag",0,ViewLayer.Normal + 2)

function OpenRedbagView:OpenRedbagView()   
     if ViewMgr:IsOpen(GuildHongBaoTipView) == false then 
          ViewMgr:OpenView(GuildHongBaoTipView)
     else
          GuildHongBaoPanel.Instance.hide.count = 0 
     end
end

function OpenRedbagView:LoadCallback()
     self.hong_bao_id = GuildRedbagData.Instance:GetCurGetHongBaoInfo().hong_bao_id
     GuildRedbagData.Instance.family_redbag_last_info.is_stop = true
end

function OpenRedbagView:NoDisplayed()
    if ViewMgr:IsOpen(GuildHongBaoTipView) == false then return end
    if GuildHongBaoPanel.Instance.no_displayed_list_cache[self.hong_bao_id] == nil then
      table.insert(GuildHongBaoPanel.Instance.no_displayed_list_cache,self.hong_bao_id,self.hong_bao_id)
    end
    GuildHongBaoPanel.Instance.hide.count = 1 
    ViewMgr:CloseView(OpenRedbagView)
end

------------单个红包消息的列表------------
GuildGetNameListPanel = GuildGetNameListPanel or DeclareMono("GuildGetNameListPanel", UIWFlushPanel)
function GuildGetNameListPanel:GuildGetNameListPanel()
  self.data = GuildRedbagData.Instance
  self.data_cares = {
    {data = self.data.family_redbag_get_one_info, func = self.FulshOpenInfo, init_call = true},
    {data = self.data.family_redbag_get_one_info, func = self.FulshLingQuInfo,init_call = false},
  }
    local Str1 = RichTextHelper.LinkStr(Language.Guild.HongBaoNoDisplayed, "_ul",COLORSTR.Yellow19)
    UH.SetText(self.NoDispalyed, Str1)
    if self.data.family_redbag_cur_hong_bao_id then
      if self.data.family_redbag_info_cfg[self.data.family_redbag_cur_hong_bao_id] then
         self:FulshLingQuInfo()
         self.EffectBg:SetActive(false)
         self.Lingqu:SetActive(true)
         self.AnimCtrl:SetTrigger(APH("Open"))
      else
        self.AnimCtrl:SetTrigger(APH("Close"))
      end
    end
end

---可领取
function GuildGetNameListPanel:FulshOpenInfo()
  local data = self.data:GetHongBaoGetOneInfo()
  for i=1,4 do
    self.Objs[i]:SetActive(false)
  end
  if data.hongbao_type == nil then 
    self:OnClickClose() 
    return 
  end
  self.Objs[GuildHongBaoConfig.IconByType[data.hongbao_type]]:SetActive(true)
  UH.SetAvatar(self.HeadIcon,self.data:GetCurGetHongBaoInfo().send_avatar_type,self.data:GetCurGetHongBaoInfo().avatar_id,self.data:GetCurGetHongBaoInfo().avatar_quality)
  UH.SetText(self.Name,data.send_name)  
end
function GuildGetNameListPanel:OnClickClose()
    self.data.family_redbag_cur_hong_bao_id = nil
    GuildRedbagData.Instance.family_redbag_last_info.is_stop = false
    if self.guoqi then
      TimeHelper:CancelTimer(self.guoqi)
    end
    if self.remove_id then --点击查看后  删除可领取红包
      GuildRedbagData.Instance:RemoveHongBaoById(self.remove_id,true)
    end
    ViewMgr:CloseView(OpenRedbagView)
    if GuildHongBaoPanel.Instance then
          GuildHongBaoPanel.Instance.hide.count = 1 
    end
end

--打开红包
function GuildGetNameListPanel:OnClickOpen()
  self.EffectBg:SetActive(false)
  self.EffectTool:Play(3161034)
  local data = self.data:GetHongBaoGetOneInfo()
  self.ClickEffect:SetActive(true)
  self.data.family_redbag_last_info.id = data.hong_bao_id
  if data.get_count ~=nil or data.change_type == 1 then
    if data.get_count >= Config.family_auto.redbag[data.hongbao_type].redbag_num then 
        self.is_click = true
        self.remove_id = data.hong_bao_id
        self:FulshLingQuInfo()
    end
    if data.change_type == 1 then
        self.is_click = true
        self:FulshLingQuInfo()
    end
  end
  if self.is_click == nil then
       GuildRedBagCtrl.ReqGuildHongBaoById(data.hong_bao_id)
  end
  self.is_click = true
end

local crossPos1 = Vector3.New(-300, 100, 0)
local crossPos2 = Vector3.New(-62, 240, 0)

function GuildGetNameListPanel:PlayCurrEffect(startPos, endPos, count)
  if not self.LingYuObjs then
    return
  end 
  local effect_lsit = {}
  for i = 1, count, 1 do
    effect_lsit[i] = {}
    effect_lsit[i].index = i
    effect_lsit[i].shoot_curve_list = 
          Game.CurveParabola(startPos, endPos, i % 2 == 0 and crossPos1 or crossPos2, GameMath.Ramdon())
    effect_lsit[i].ling_yu = self.LingYuObjs[i]
    effect_lsit[i].count = count
  end
  self.EffectLst:SetDataList(effect_lsit)
end

---灵玉飞向
function GuildGetNameListPanel:ShowCreateEffectEffect()
  self:PlayCurrEffect(Vector3.New(0, 24, 0), Vector3.New(648,-319, 0),7)
end 

--领取红包详细界面刷新 单个
function GuildGetNameListPanel:FulshLingQuInfo()
  local data = self.data:GetHongBaoGetOneInfo()
    --local over_time = TimeHelper.FormatDHMS(TimeCtrl.Instance:GetServerTime() - data.invalid_timestamp)
    --if over_time.s > 0 or over_time.min > 0 or over_time.hour > 0 or over_time.day > 0 then
    if data.change_type == 1 and self.is_click then
       PublicPopupCtrl.Instance:Center(Language.Guild.HongBaoOverTiemText)
       GuildRedbagData.Instance:RemoveHongBaoById(data.hongbao_id)
       self:OnClickClose()
       return 
    end

    if self.is_click then
      self.Lingqu:SetActive(true)
      if data.get_count ~=nil then
        --if data.get_count < Config.family_auto.redbag[data.hongbao_type].redbag_num then
        if data.get_val then
             self.AnimCtrl:SetTrigger(APH("GetLingYu"))  
             self.fly_effect_cache = TimeHelper:AddDelayTimer(BindTool.Bind(self.ShowCreateEffectEffect,self),0.8)
             UH.SetText(self.GetTex,string.format(Language.Guild.HongBaoGetText,data.get_val))
        else
             self.AnimCtrl:SetTrigger(APH("NotGetLingYu"))
             UH.SetText(self.NotGetTex,Language.Guild.HongBaoNotGetText)
        end
       end
      self.is_click = false 
    end
  --剩余情况
  if self.data.family_redbag_cur_hong_bao_id then
    data = self.data.family_redbag_info_cfg[self.data.family_redbag_cur_hong_bao_id]
  end
  local tb=self:GetHongBaoInfo(data.hongbao_type)
  if tb == nil then return end
  UH.SetText(self.Desc,string.format(Language.Guild.HongBaoDesc,tb.redbag_sum,data.get_count,tb.redbag_num))
  UH.SetText(self.DescLen,string.format(" (%d/%d)",data.get_count,tb.redbag_num))
  if data.get_count > 0 then
    table.sort(data.bag_get_name_list,function(a,b)
      return a.times > b.times
    end)
    self.NameLst:SetData(data.bag_get_name_list)
  end
end

function GuildGetNameListPanel:GetHongBaoInfo(type)
  for i,v in ipairs(Config.family_auto.redbag) do
     if v.redbag_type == type then
       return v 
     end
  end
end

function GuildGetNameListPanel:OnDestroy()
  UIWFlushPanel.OnDestroy(self)
  if self.fly_effect_cache then
    TimeHelper:CancelTimer(self.fly_effect_cache)
    self.fly_effect_cache = nil
  end
  GuildData.Instance.guild_trial_point:Notify()
end

LingYuListItem = LingYuListItem or DeclareMono("LingYuListItem", UIWidgetBaseItem)
function LingYuListItem:LingYuListItem()
    
end
function LingYuListItem:SetData(data)
    self.shoot_curve_list = data.shoot_curve_list
    self.start_frame = Time.realtimeSinceStartup  
    self.effect_cache = TimeHelper:AddCountDownTT(BindTool.Bind(self.UpdatePos, self,data.index, data.count, true),function ()
    end, (20 / 24), (1 / 24))
    data.ling_yu:SetActive(false)
end
function LingYuListItem:OnRelease()
  if self.effect_cache then
      TimeHelper:CancelTimer(self.effect_cache)
  end
end
function LingYuListItem:UpdatePos(index, count, usetime)
  if self.is_close then return end
  if index <= count then
      local time = 0
      if usetime then
          time = (Time.realtimeSinceStartup - self.start_frame) / (20/24)
      else
          time = (Time.frameCount - self.start_frame) / 20
      end
      time = time > 1 and 1 or time
      UH.LocalPos(self.PlayObj, self.shoot_curve_list:GetPos(time))
      if time == 1 and (index == 1 or index == 4 or index == 7) then
         self.EffectTool:Play(3161047)
      end
  end
end


-------------玩家领取到的红包消息--------------------
HongBaoListItem =HongBaoListItem or DeclareMono("HongBaoListItem", UIWidgetBaseItem)
function HongBaoListItem:HongBaoListItem()
    
end
function HongBaoListItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Val,data.val)
end
------------

------------发送红包界面----------------------
SendRedbagView = SendRedbagView or DeclareView("SendRedbagView","guild/guild_redbag/feng_redbag",0,ViewLayer.Normal + 2)
function SendRedbagView:SendRedbagView()
    
end

function SendRedbagView:LoadCallback()
  self.type = GuildRedbagData.Instance:GetSendHongBaoType()
  self.icon_type = GuildHongBaoConfig.IconByType[self.type]
  self.Objs[self.icon_type]:SetActive(true)
  UH.SetText(self.Val,Config.family_auto.redbag[self.type].redbag_sum)
end
--发送红包
function SendRedbagView:OnClickSend()
    --播放特效
    self.HongBao:SetActive(false)
    self.Effect:SetActive(true)
    self.EffectTool:Play("3161035")
    GuildRedBagCtrl.SendGuildHongBaoByType(self.type,Language.Guild.HongBaoGreeting)
end
function SendRedbagView:OnClickClose()
  ViewMgr:CloseView(SendRedbagView)
end

------------主界面红包提示------------
GuildHongBaoTipView = GuildHongBaoTipView or DeclareView("GuildHongBaoTipView","guild/guild_redbag/btn_redbag",0,ViewLayer.Normal + 1)
VIEW_DECLARE_MASK(GuildHongBaoTipView ,ViewMask.None)
VIEW_DECLARE_LEVEL(GuildHongBaoTipView, ViewLevel.Lt)
function GuildHongBaoTipView:GuildHongBaoTipView()
  
end

GuildHongBaoPanel = GuildHongBaoPanel or DeclareMono("GuildHongBaoPanel", UIWFlushPanel)
function GuildHongBaoPanel:GuildHongBaoPanel()
  GuildHongBaoPanel.Instance = self
  self.data = GuildRedbagData.Instance
  self.hide = SmartData.New({count = 0})
  self.data_cares = {
    {data = self.data:GetHongBaoGetList(), func = self.FulshTips, init_call = true },
    {data = self.data:GetHongBaoGetOneInfo(), func = self.FulshCloseTips, init_call = true},
    {data = self.hide, func = self.HideOrShow,init_call = false},
    --{data = LoginData.Instance:BaseData(), func = self.Cross,keys = {"is_on_crosssever"},init_call = false},  取消跨服限制
  }
  --避免EffectTool未初始化
  self.effect_init = TimeHelper:AddDelayTimer(BindTool.Bind(self.HideOrShow,self),0.1)
  self.info = {}
  self.no_displayed_list_cache = {}
end
--[[function GuildHongBaoPanel:Cross()
	  if  LoginData.Instance:BaseData().is_on_crosssever then
		   self.hide.count = 2 
	  else
		   self.hide.count = 3 	
	  end
end--]]
function GuildHongBaoPanel:HideOrShow() 
  if self.hide.count == 1 then
    self.Effect:SetActive(true)
    if self.effect == nil then
      self.effect = self.EffectTool:Play(3161037)
    end
  else
    self.Effect:SetActive(false)
  end
  ---检测在跨服 LoginData.Instance:BaseData().is_on_crosssever or
  if GuildRedbagData.Instance:IsCanUseHongBao() == false 
    or self.data:GetHongBaoGetList().count <= 0 or self:IsFindValue() == false then 
      self.Effect:SetActive(false)
      self.hide.count = 0
  end
end

function GuildHongBaoPanel:FulshCloseTips()
  if GuildRedbagData.Instance.family_redbag_last_info.is_stop then return end
  local info = self.data:GetHongBaoGetOneInfo()
  if GuildRedbagData.Instance:GetHongBaoGetList().count ~= 0 and GuildRedbagData.Instance:GetHongBaoGetList().count then
    local id = self.info.hong_bao_id
    if self:IsFindValue() then
      if self.info.hong_bao_id ~= id then
          GuildRedBagCtrl.GetGuildHongBaoByID(self.info.hong_bao_id)
          --GuildCtrl.Instance.GuildHongBaoReq({req_type=OPERA_TYPE.OPERA_TYPE_BASE_INFO_REQ,p2=self.info.hong_bao_id})
      return
     end
   end
  end
  if info.get_count ~=nil then
    if info.get_count >= Config.family_auto.redbag[info.hongbao_type].redbag_num then
     self.is_send = true
     self.remove_id = info.hong_bao_id
     if GuildRedbagData.Instance.family_redbag_last_info.id ~= self.remove_id then
      self.show_remove = TimeHelper:AddDelayTimer(BindTool.Bind(self.FulshShow,self),5)
     end
    end
  end
end
function GuildHongBaoPanel:OnDestroy()
  UIWFlushPanel.OnDestroy(self)
  if self.effect_init then 
    TimeHelper:CancelTimer(self.effect_init)
  end
  if self.show_remove then
    TimeHelper:CancelTimer(self.show_remove)
  end
end
---可领取
function GuildHongBaoPanel:FulshTips()
    if GuildRedbagData.Instance:IsCanUseHongBao() then   
     if self.data:GetHongBaoGetList().count ~= nil and self.data:GetHongBaoGetList().count ~= 0 then 
      if self:IsFindValue() then       
        self.hide.count = 1
      end
        local info = self.data:GetHongBaoGetOneInfo()
        if info.hong_bao_id == nil or self.is_remove then 
          if self:IsFindValue() then
            GuildRedBagCtrl.GetGuildHongBaoByID(self.info.hong_bao_id)
            self.is_remove = false
          end
        end
     else
      self.hide.count = 0
     end   
    else
      self.hide.count = 0
    end
end
function GuildHongBaoPanel:IsFindValue()
  for i,v in ipairs(self.data:GetHongBaoGetList().redbag_get_list)  do
    if self.no_displayed_list_cache[v.hong_bao_id] == nil then
      self.info = v
      return true
    end
  end
  return false
end
function GuildHongBaoPanel:FulshShow()
  if GuildRedbagData.Instance.family_redbag_last_info.is_stop then self.is_remove = true return end
  if self.data:GetHongBaoGetList().count ~= nil and self.data:GetHongBaoGetList().count ~= 0 then 
    if self.is_send then
       if GuildRedbagData.Instance.family_redbag_last_info.id ~= self.remove_id then
        GuildRedbagData.Instance.family_redbag_last_info.id = self.remove_id
        self.is_remove = true 
       end
    else
      self.remove_id = 0
    end
  else
    self.hide.count = 0
  end
end
function GuildHongBaoPanel:ClickLingQu()
    self.is_send = false
    self.hide.count = 0
    if self:IsFindValue() then
     GuildRedBagCtrl.GetGuildHongBaoByID(self.info.hong_bao_id)
     GuildRedbagData.Instance:SetCurGetHongBaoIdInfo(self.info)
     ViewMgr:OpenView(OpenRedbagView)
    end
end