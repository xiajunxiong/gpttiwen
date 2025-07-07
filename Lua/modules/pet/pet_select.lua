
PetSelectView = PetSelectView or DeclareView("PetSelectView", "pet/pet_select", Mod.PetSelect.Main)
VIEW_DECLARE_LEVEL(PetSelectView, ViewLevel.Lt)
-- item_array = Array(PetSelectViewItem)
function PetSelectView:PetSelectView()
	self.lang = Language.Pet
end

function PetSelectView:LoadCallback()
	-- self.Board:SetData(self:GetType(), self.lang.Pet)
	local data_list = PetData.Instance:GetPetSelectViewData()
	for i=1,self.item_array:Length() do
		self.item_array[i]:SetData(data_list[i])
	end
end

function PetSelectView:CloseCallback()
	PetData.Instance:SetGuidePetSelectCurSeq({seq = 0, show_pet = 0})
end
--------------PetSelectViewItem--------------------------
PetSelectViewItem = PetSelectViewItem or DeclareMono("PetSelectViewItem", UIWidgetBaseItem)
-- pet_name = Text
-- BigBgAnimation = Animation
-- Anim = Animation
function PetSelectViewItem:PetSelectViewItem()
	self.first_click = false

end

function PetSelectViewItem:SetData(data)
	self.data = data
	self.pet_cfg = PetData.Instance:GetPetCfg(self.data.show_pet)
	-- self.pet_name.text = self.pet_cfg.name

	-- self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.pet_cfg.res_id))
end

function PetSelectViewItem:OnEffectEnd()
	if not ViewMgr:IsOpen(PetSelectView) then
		return
	end
	for i=1, self.OtherBtns:Length() do
		self.OtherBtns[i].enabled = true
	end
	PetData.Instance:SetGuidePetSelectCurSeq(self.data)
	PetData.Instance.guide_pet_select_cur.is_info_view = true
	-- TimeHelper:AddDelayTimer(function ()
	-- 	if not self.Anim:Equals(nil) then
			self.Anim:SetTrigger(APH("End"))
	-- 	end
	-- end,0.4)
end

function PetSelectViewItem:OnClickCheckInfo()
	if not PetData.Instance.guide_pet_select_cur.is_info_view and not self.BigBgAnimation.isPlaying then
		-- PetData.Instance:SetGuidePetSelectCurSeq(self.data)
		-- PetData.Instance.guide_pet_select_cur.is_info_view = true
		self.EffectTool:Play("100108111")
		if self.Anim then
			self.Anim:SetTrigger(APH("Play"))
			-- self.Anim:Play()
		end
		TimeHelper:AddDelayTimer(function ()
			-- self.EffectTool:Play("10010811")
			self:OnEffectEnd()
		end,2.5)
		for i=1, self.OtherBtns:Length() do
			self.OtherBtns[i].enabled = false
		end
	end
end

---------------PetSelectRightInfoView----------------------------
PetSelectRightInfoView = PetSelectRightInfoView or DeclareMono("PetSelectRightInfoView",UIWFlushPanel)
-- rightAnim = Animator
-- skillInfo = Text
-- skillIcon = UIImageSpriteSet
-- conVal = Text
-- strVal = Text
-- defVal = Text
-- agiVal = Text
-- magicVal = Text
-- growValue = Text
-- fiveSided = UIImageSpriteSet
-- showList = Array(GameObject) --废弃了
-- BigBgCanvas = CanvasGroup
-- ModelShow = ModelUIShow
-- BigBgAnimation = Animation
-- ThreePetsCanvas = CanvasGroup
-- SkillCornerText = Text
-- SkillCornerObj = GameObject
function PetSelectRightInfoView:PetSelectRightInfoView()
	-- if self.ui_obj == nil then
	-- 	self.ui_obj = UIObjDrawer.New()
	--     self.ModelShow:SetShowData(
	--         {
	--             ui_obj = self.ui_obj,
	--             view = self,
	--             show_shadow = true,
	--             shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
	--         }
	--     )
	-- end
	self.data_cares = {
		{data = PetData.Instance.guide_pet_select_cur, func = self.OnCurSeqChange, keys = {"seq"}}
	}
	self.rightAnim:ListenEvent("AnimEnd",BindTool.Bind(self.OnAnimEnd,self))
	self.ui_objs = {}
end

function PetSelectRightInfoView:Awake()
	UIWFlushPanel.Awake(self)
	local data_list = PetData.Instance:GetPetSelectViewData()
	for i=1,3 do
		if self.ui_objs[i] == nil then
			self.ui_objs[i] = UIObjDrawer.New()
		    self.ModelShowArray[i]:SetShowData(
		        {
		            ui_obj = self.ui_objs[i],
		            view = self,
		            show_shadow = true,
		            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
		        }
		    )
		end
		local pet_cfg = PetData.Instance:GetPetCfg(data_list[i].show_pet)
		self.ui_objs[i]:SetData(DrawerHelper.GetNpcPath(pet_cfg.res_id))
	end
end

function PetSelectRightInfoView:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    for i=1,3 do
    	if self.ui_objs[i] ~= nil then
    		self.ui_objs[i]:DeleteMe()
    	end
    end
    self.ui_objs = nil
end

function PetSelectRightInfoView:OnCurSeqChange(old)
	if old ~= nil and old.seq == PetData.Instance.guide_pet_select_cur.seq or PetData.Instance.guide_pet_select_cur.seq == 0 then
		return
	end
	self:SetData(PetData.Instance.guide_pet_select_cur)
end

function PetSelectRightInfoView:SetData(data)
	-- self.rightInfoObj:SetActive(true)
	self.seq = data.seq
	local pet_cfg = PetData.Instance:GetPetCfg(data.show_pet)
	self.conVal.text = pet_cfg.con
	self.strVal.text = pet_cfg.str
	self.defVal.text = pet_cfg.def
	self.agiVal.text = pet_cfg.agi
	self.magicVal.text = pet_cfg.magic
	self.growValue.text = PetData.GetPetMaxBp(pet_cfg)
	self.fiveSided.SpriteName = "Pet_five"..tostring(self.seq)
	
	local skilldesc = Cfg.SkillDescPet(pet_cfg.skill_id_1,1)
	-- LogError("pet_cfg.skill_id",pet_cfg.skill_id,pet_cfg)
	-- self.skillInfo.text = skilldesc.description
	--string.format(skilldesc.description_2, COLORSTR.Green9, tostring(skilldesc.skill_value)))--
	-- UH.SetText(self.skillInfo, string.format(Language.Common.Xstr, COLORSTR.Yellow5, skilldesc.description))
	UH.SetText(self.skillInfo, Language.Pet.PetSelectSkillDesc[skilldesc.skill_id])
	self.skillName.text = skilldesc.skill_name
	UH.SetIcon(self.skillIcon,skilldesc.icon_id,ICON_TYPE.SKILL)
	if self.BigBgAnimation.isPlaying then
		self.BigBgAnimation:Stop()
	end
	self.BigBgCanvas.alpha = 1
	self:ShowList(false)
	self.rightAnim:SetTrigger(APH("Show"))
	for i=1,3 do
		self.ModelShowObjArray[i]:SetActive(data.seq == i)
		if data.seq == i then
			self.ui_objs[i]:PlayAnim(ChAnimType.Show)
		end
	end
	-- self.ModelShowObj:SetActive(true)
	-- self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet_cfg.res_id))
	-- TimeHelper:AddDelayFrameTimer(function()
		-- self.ui_obj:PlayAnim(ChAnimType.Show)
	-- end,10)
	
	self.ThreePetsCanvas.alpha = 0
	self.SkillCornerObj:SetActive(skilldesc.corner_icon ~= nil and skilldesc.corner_icon ~= "")
	if skilldesc.corner_icon ~= nil and skilldesc.corner_icon ~= "" then
		UH.SetText(self.SkillCornerText, skilldesc.corner_icon)
	end
	-- self.models[self.seq]:SetTrigger(APH("Show"))
	-- self.petNames[self.seq]:SetTrigger(APH("Show"))
end

function PetSelectRightInfoView:OnAnimEnd(param)
	-- LogError("PetSelectRightInfoView OnAnimEnd ",param)
	if param == "1" then--showEnd
		
	elseif param == "2" then --hideEnd
		-- self.rightInfoObj:SetActive(false)
		self:ShowList(true)
	end
end

function PetSelectRightInfoView:ShowList(show)
	self.BigBgCanvas.alpha = show and 0 or 1
	-- for i=1, self.showList:Length() do
	-- 	if not show and i ~= self.seq or show then
	-- 		self.showList[i]:SetActive(show)
	-- 	end
	-- end
end

function PetSelectRightInfoView:OnClickConfirm()
	-- self.EffectTool:Play(10010811)
	self:SendSelectConfirm()
end

function PetSelectRightInfoView:SendSelectConfirm()
	RoleCtrl.Instance:SendFuncGuideOperator(FUNCTIONGUIDEOPTYPE.PETSELECT, self.seq)
	PetData.Instance.guide_pet_select_cur.is_info_view = false
	ViewMgr:CloseView(PetSelectView)
end

function PetSelectRightInfoView:OnClickCancel()
	PetData.Instance.guide_pet_select_cur.is_info_view = false
	self.ThreePetsCanvas.alpha = 1
	self.BigBgAnimation:Play()
	PetData.Instance:SetGuidePetSelectCurSeq({seq = 0, show_pet = 0})
	self.rightAnim:SetTrigger(APH("Hide"))
end