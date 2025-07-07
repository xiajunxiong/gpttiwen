GoThroughView = GoThroughView or DeclareView("GoThroughView", "go_through/go_through_view",Mod.Challenge.GoThrough)

VIEW_DECLARE_LEVEL(GoThroughView,ViewLevel.L0)
VIEW_DECLARE_MASK(GoThroughView,ViewMask.None)

function GoThroughView:GoThroughView()
    ResMgr:LoadGameObject("actor/other/DJCJ_1.prefab", function (obj)
        self.obj = obj
        self.obj.transform.localPosition = Vector3.New(1000,1000,1000)
        self.obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
    end, true, false)
    self.data = GoThroughData.Instance
    GoThroughCtrl.Instance:SendGoThroughReq()
    MainOtherCtrl.Instance:CloseKeyUse()
end

function GoThroughView:LoadCallback(param_t)
    local data = self.data:GetPassLevelData()
    if data ~= nil then
        self:OnFlushView(data)
        self:FlushModelView(data)
    else
        local max_data = self.data:GetMaxPassLevelData()
        self:OnFlushView(max_data)
        self:FlushModelView(max_data)
    end
    self.Panel:SetActive(data ~= nil)
    self.Finish:SetActive(data== nil)
end

function GoThroughView:CloseCallback()
    Delete(self,[[ModelGame]])
    if self.obj then
        UnityGameObj.Destroy(self.obj)
    end
end

function GoThroughView:OnClickClose()
    ViewMgr:CloseView(GoThroughView)
end

function GoThroughView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(162)
end

function GoThroughView:OnFlushView(data)
    local is_finish = self.data:GetProgress() >= data.cond_param
    self.AttrList:SetData(self.data:GetAttrList(data))
    self.Grid:SetData(DataHelper.FormatItemList(data.reward))
    self:FlushDescView(data.pre_require_des,data.fight_target,data)
    for i=1,self.TitleItem:Length() do
        self.TitleItem[i]:SetData(TitleData.GetTitleConfig(data.title))
    end
    self.Complete:SetActive(is_finish)
    UH.SetInter(self.BtnInter,is_finish)
    self:FlushRewardView(data)
end

function GoThroughView:FlushDescView(desc,target,data)
    local progress = self.data:GetProgress()
    local COLOR = progress >= data.cond_param and COLORSTR.Green7 or COLORSTR.Red10
    UH.SetText(self.Progress,Format(Language.GoThrough.ProgressTip,desc,ColorStr(progress,COLOR),data.cond_param))
    self.Progress:SetObjActive(target == 0)
    self.Desc:SetObjActive(target ~= 0)
    UH.SetText(self.Desc,desc)
end

function GoThroughView:FlushModelView(data)
	local npc_config = Cfg.NpcById(data.npc_id)
	local path = DrawerHelper.GetNpcPath(npc_config.res_id)
	if not self.ModelGame then
		self.ModelGame = UIChDrawer.New()
		self.ModelGame:SetMainSolo(path)
		self.ModelShow:SetShowData({ui_obj = self.ModelGame, view = self})
	else
		self.ModelGame:SetMainSolo(path)
	end
end

function GoThroughView:OnClickEnter()
    SceneLogic.Instance:AutoToNpcs(self.data:GetNpcId())
    ViewMgr:CloseView(GoThroughView)
end

function GoThroughView:OnClickChallenge()
    local data = self.data:GetPassLevelData()
    if data.fight_target ~= 0 then
        SceneLogic.Instance:AutoToNpcs(data.fight_target)
    end
    ViewMgr:CloseView(GoThroughView)
end

---------------------------助战奖励---------------------------
function GoThroughView:FlushRewardView(data)
    local times = self.data:GetAssistTimes()
    local config = Config.cross_robbery_auto.other[1]
    UH.SetText(self.HelpTitle,Language.GoThrough.RewardTip)
    UH.SetText(self.HelpTimes,Format("%s/%s",times,config.help_times))
    self.HelpGrid:SetData(DataHelper.FormatList(config.help_reward))
end

function GoThroughView:OnClickBlock()
	self.HelpPanel:SetActive(not GetActive(self.HelpPanel))
end

GoThroughRewardItem = GoThroughRewardItem or DeclareMono("GoThroughRewardItem", UIWidgetBaseItem)

function GoThroughRewardItem:SetData(data)
	self.Call:SetData(Item.Create(data))
end

----------------------------GoThroughAttrItem----------------------------
GoThroughAttrItem = GoThroughAttrItem or DeclareMono("GoThroughAttrItem", UIWidgetBaseItem)

function GoThroughAttrItem:SetData(data)
    UH.SetText(self.Value,Format("%s  %s",data.attr_name,
    ColorStr("+" .. data.attr_value,COLORSTR.Green4)))
end

GoThroughOpera = GoThroughOpera  or DeclareMono("GoThroughOpera",UIWFlushPanel)

function GoThroughOpera:GoThroughOpera()
    self.data = GoThroughData.Instance
    local pass_level = self.data:GetPassLevel()
    local model_tab = {[2] = 0.7,[4] = 0.8,[7] = 0.8,[8] = 0.55}
    local size = model_tab[pass_level] or 1
    local data = self.data:GetPassLevelData() or self.data:GetMaxPassLevelData()
    local npc_config = Cfg.NpcById(data.npc_id)
    local res_path = DrawerHelper.GetNpcPath(npc_config.res_id)
    if self.ModelGame ~= nil then 
        self.ModelGame:SetData(res_path)
    else 
        self.ModelGame = UIObjDrawer.CreateByPath(res_path)
        self.ModelGame:Enable()
        self.ModelGame:GetRoot():SetParent(self.ModelPos)
        self.ModelGame:GetRoot():SetLocalPosition(Vector3.zero)
        self.ModelGame:GetRoot():SetLocalEulerAngles(Vector3.zero)
        self.ModelGame:GetRoot():SetLocalScale(Vector3.New(size,size,size))
    end 
end

function GoThroughOpera:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Delete(self,[[ModelGame]])
end