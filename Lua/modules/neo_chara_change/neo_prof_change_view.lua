NeoProfChangeView = NeoProfChangeView or DeclareView("NeoProfChangeView", "neo_chara_change/neo_prof_change",Mod.ProfesstionChange.Change)
--ToggleProf====LuaUIEventToggle
--ToggleColor====LuaUIEventToggle

function NeoProfChangeView:NeoProfChangeView() 

end 

function NeoProfChangeView:LoadCallback(param)
    if param.mod_key == Mod.ProfesstionChange.ColorChange then
        self.ToggleColor:Trigger()
    else
        self.ToggleProf:Trigger()
    end
end 

function NeoProfChangeView:Close()
    ViewMgr:CloseView(NeoProfChangeView)
end 


function NeoProfChangeView:OnToggleClick()

end


NeoProfChangePanel = NeoProfChangePanel or DeclareMono("NeoProfChangePanel", UIWFlushPanel)
---ModelShow====ModelUIShow
---RoleProfName====UIImageSpriteSet
--AnimBlock========GameObject

function NeoProfChangePanel:NeoProfChangePanel()

    if self.cur_ui_obj == nil then
        self.cur_ui_obj = UIChDrawer.New()
        self.CurAppear:SetShowData({
            ui_obj = self.cur_ui_obj,
            view = self,
            seq = 1,
        })
    end 

    if self.next_ui_obj == nil then
        self.next_ui_obj = UIChDrawer.New()
        self.NextAppear:SetShowData({
            ui_obj = self.next_ui_obj,
            view = self,
            seq = 2,
        })
    end 

    self.view_data = NeoCharaChangeData.Instance
    self.CharaList:SetData(self.view_data:GetProfCharaList())
    self.ColorOne.isOn = true
    self:ColorClick(1)

    self.StuffItem:SetDataByItemId(CommonItemId.ChangeAppear)    --角色形象更换券
    
    self.data_cares = {
        {data = BagData.Instance.item_grid[ItemColumnType.Item],func = self.FlushStuff},
        {data = self.view_data.prof_view_data,func = self.FlushStuff,keys = {"appear_id","prof_id","color_index"},init_call = false},
        {data = self.view_data.prof_view_data,func = self.FlushProfListPos,keys = {"app_index"},init_call = false},
        {data = RoleData.Instance:GetBaseData(),func = self.FlushCurModel,keys = {"appearance"}},
        {data = self.view_data.prof_view_data,func = self.FlushNexthModel,keys = {"prof_id","color_index"},init_call = false},
        {data = self.view_data.prof_view_data,func = self.ChangeAppearFlushNext,keys = {"appear_id"},init_call = false},
        {data = self.view_data.prof_view_data,func = self.FlushCurModel,keys = {"is_show_attr"}},
        {data = self.view_data.prof_view_data,func = self.FlushSkillPanel,keys = {"is_show_skill_tips"}},
        {data = self.view_data.prof_view_data,func = self.ReastPanel,keys = {"is_reast"}},
    }


    self.CenterAnim:ListenEvent("anim_state",
        function (eve_param,anim_st_info)
            if eve_param == "showcur" then 
                self.view_data.prof_view_data.is_show_next = false
            end 
        end)
      
    self.CenterAnim:ListenEvent("anim_state",
        function (eve_param,anim_st_info)
            if eve_param == "shownext" then 
                self.view_data.prof_view_data.is_show_next = true
                self.AnimBlock:SetActive(false)
                self.CenterAnim.enabled = false
                self:AfterShowNextAnim()
            end 
        end)
end 

function NeoProfChangePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.cur_ui_obj ~= nil then
		self.cur_ui_obj:DeleteMe()
	end
    self.cur_ui_obj = nil

    if self.next_ui_obj ~= nil then
		self.next_ui_obj:DeleteMe()
	end
    self.next_ui_obj = nil

    self.view_data:ClearProfViewData()
end 

function NeoProfChangePanel:ClickCharaItem(data)
    self.view_data:SetAppearId(data.avartar_type)
    self.view_data:SetAppIndex(data.index)

    if data.color_list ~= nil then 
        local cur_color_list = Split(data.color_list,",")
        for i = 1,4 do 
            UH.SpriteName(self.ColorBtns[i], "PeiSe"..cur_color_list[i])
        end 
    end 
end

function NeoProfChangePanel:ClickProfItem(data)
    self.view_data:SetProfId(data.job_id)
    
    local desc_config = RoleData.Instance:GetProfessionDesc(data.job_id,true)
    self.ToggleTip1.text = desc_config.weapon
    self.ToggleTip2.text = desc_config.armor_type
    self.ToggleTip3.text = desc_config.point
    self.ToggleTip4.text = desc_config.desc

    local skill_data = SkillData.GetProfSkillList(RoleData.ProfIdToProfType(data.job_id))
    self.SkillList:SetData(skill_data)
    -- self.SkillList:JumpHorizontalIndex(1,1)
end

function NeoProfChangePanel:ColorClick(index)
    self.view_data:SetColorIndex(index)
end 

function NeoProfChangePanel:FlushProfListPos()
    local app_index = self.view_data.prof_view_data.app_index

    if app_index == 0 then return end 

    self.ProfListObj:SetActive(true)

    self.ProfListObj:SetParent(self.CharaList.item_list[app_index].ProfPos)
    UH.LocalPosG(self.ProfListObj, Vector3.zero)

    if app_index < 4 then 
        self.ProfListRect.anchorMin = Vector2.New(0, 1)	
        self.ProfListContentRect.anchorMin = Vector2.New(0, 1)	
        self.ProfListRect.anchorMax = Vector2.New(1, 1) 	
        self.ProfListContentRect.anchorMax = Vector2.New(1, 1)	
        self.ProfListRect.pivot = Vector2.New(0.5, 1)	
        self.ProfListContentRect.pivot = Vector2.New(0.5, 1)	
    else 
        self.ProfListRect.anchorMin = Vector2.New(0, 0)	
        self.ProfListContentRect.anchorMin = Vector2.New(0, 0)	
        self.ProfListRect.anchorMax = Vector2.New(1, 0) 	
        self.ProfListContentRect.anchorMax = Vector2.New(1, 0)	
        self.ProfListRect.pivot = Vector2.New(0.5, 0)	
        self.ProfListContentRect.pivot = Vector2.New(0.5, 0)	
    end 

    self.ProfListRect.anchoredPosition = Vector2.New(0, 0)	
    self.ProfListContentRect.anchoredPosition = Vector2.New(0, 0)	

    self.ProfToggleGroup:SetAllTogglesOff()

    self.ProfList:SetData(self.view_data:GetProfCharaProfList())
    self.ProfList:ClickItem(1)
end

function NeoProfChangePanel:FlushCurModel()
    self.CurAppParent:SetActive(not self.view_data.prof_view_data.is_show_attr)

    local role_appearance = RoleData.Instance:GetApearance()
    self.cur_ui_obj:SetData(role_appearance)
   
    self.CurProfLbl.text = string.format(Language.RoleAppearChange.CurProfLbl,RolePromoteData.GetProfName(RoleData.Instance:GetRoleProfId()))

    if self.view_data.prof_view_data.is_show_next then 
        -- self.view_data.prof_view_data.is_reast = true
    end 
end

function NeoProfChangePanel:ChangeAppearFlushNext()
    self.view_data:SetProfId(0)
    -- self:FlushNexthModel()
end

function NeoProfChangePanel:FlushNexthModel()
    local can_show,chance_appearance = self.view_data:GetNextAppearance()
    -- self.NextAppParent:SetActive(can_show)
    self.ColorGroupObj:SetActive(can_show)
    self.ProfAttrGroup:SetActive(can_show)
    self.StuffGroup:SetActive(can_show)
    if not can_show then return end 
    self.next_ui_obj:SetData(chance_appearance)


    self:TryPlayChangeNextAnim()
end

function NeoProfChangePanel:TryPlayChangeNextAnim()
    local role_appearance = RoleData.Instance:GetApearance()
    local flag_ = role_appearance.avatar_type ~= self.view_data.prof_view_data.appear_id
        or role_appearance.color ~= self.view_data.prof_view_data.color_index
        or RoleData.Instance:GetRoleProfId() ~= self.view_data.prof_view_data.prof_id

    if not self.view_data.prof_view_data.is_show_next and flag_ then 
        self.CenterAnim.enabled = true
        self.AnimBlock:SetActive(true)
        self.CenterAnim:SetTrigger(APH("ChangeNext"))
    else 
        self:AfterShowNextAnim()
    end 
end

function NeoProfChangePanel:AfterShowNextAnim()
    self.next_ui_obj:PlayAnim(ChAnimType.WeaponShow)

    local audio = Cfg.AudioProf(self.view_data.prof_view_data.appear_id,RoleData.ProfIdToProfType(self.view_data.prof_view_data.prof_id))
    if audio == nil then return end 
    AudioMgr:StopEffectHandle(self.handle_audio1)
    AudioMgr:StopEffectHandle(self.handle_audio2)
    self.handle_audio1 = AudioMgr:PlayEffectHandle(AudioType.UI, audio.show_audio)
    self.handle_audio2 = AudioMgr:PlayEffectHandle(AudioType.UI, audio.show_voice)
end

function NeoProfChangePanel:FlushStuff()
    self.CouponNum.text = tostring( BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) .. "/1")
    local role_appearance = RoleData.Instance:GetApearance()

    local flag_1 = BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) > 0 
    local flag_2 = role_appearance.avatar_type ~= self.view_data.prof_view_data.appear_id
        or role_appearance.color ~= self.view_data.prof_view_data.color_index
        or RoleData.Instance:GetRoleProfId() ~= self.view_data.prof_view_data.prof_id

    self.CostText.text = Config.career_advancements_auto.other[1].transfer_cost_min

    self.BtnChangeInetrActor.Interactable = flag_1 and flag_2
    local flag_ = flag_1 and flag_2
    self.BtnChangeText.color = flag_ and COLORS.Red2 or COLORS.Gray1 
end

function NeoProfChangePanel:ClickToggleOn()
    self.BtnToggleOn:SetActive(false)
    self.ProfToggleAnim:SetTrigger(APH("is_show"))
    -- self.ProfToggle:SetActive(true)

    if self.view_data.prof_view_data.is_show_next then 
        self.view_data:SetIsShowAttr(true)
    end 
end

function NeoProfChangePanel:ClickToggleOff()
    self.BtnToggleOn:SetActive(true)
    self.ProfToggleAnim:SetTrigger(APH("is_hide"))
    -- self.ProfToggle:SetActive(false)

    if self.view_data.prof_view_data.is_show_next then 
        self.view_data:SetIsShowAttr(false)
    end 
end 

function NeoProfChangePanel:FlushSkillPanel()
    self.SkillPanel:SetActive(self.view_data.prof_view_data.is_show_skill_tips)   
    self.SkillTipsText.text = self.view_data.prof_view_data.skill_info_tips
end

function NeoProfChangePanel:CloseSkillPanel()
    NeoCharaChangeData.Instance:SetProfViewSkill({is_show = false})
end 

function NeoProfChangePanel:ChangeClick()
    if BagData.Instance:GetNumByItemId(CommonItemId.ChangeAppear) == 0 then 
        local item = Item.New({item_id = CommonItemId.ChangeAppear})
        PublicPopupCtrl.Instance:DialogTips{
            content = string.format(Language.Fight.NotEnoughKey, item:Name()),
            confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                        local key,config = MallData.Instance:GetShopItemConfig(item.item_id)
                        StoreData.Instance:SetJumpIndex(config,config.item_id)
                        ViewMgr:OpenViewByKey(Mod.Store.Gold)
                end
            }
        }
        return 
    end 
    local role_appearance = RoleData.Instance:GetApearance()
    if role_appearance.avatar_type == self.view_data.prof_view_data.appear_id and 
        role_appearance.color == self.view_data.prof_view_data.color_index and 
        RoleData.Instance:GetRoleProfId() == self.view_data.prof_view_data.prof_id then
        PublicPopupCtrl.Instance:Center(Language.Role.PromoteSameAvatarTips)
        return
    end

    ViewMgr:OpenView(NeoChangeTipsView, {func =  function ()
        RoleCtrl.Instance:SendRoleCareerChangeReq({
        to_profession = self.view_data.prof_view_data.prof_id,
        avartar_type = self.view_data.prof_view_data.appear_id, 
        color = self.view_data.prof_view_data.color_index,})

        self.view_data.prof_view_data.is_reast = true
        ViewMgr:CloseView(NeoProfChangeView)
        RoleCtrl.Instance:SendRequestChangeProfTime()
        end
    })

    self.view_data:CacheAppearance({
        to_profession = RoleData.Instance:GetRoleProfId(),
        avartar_type =  role_appearance.avatar_type, 
        color = role_appearance.color,
    })

    -- local role_appearance = RoleData.Instance:GetApearance()
    -- local content_str = Language.Role.PopupTips0
    -- PublicPopupCtrl.Instance:DialogTips{
    --     content = content_str,
    --     confirm = {
    --         func = function ()
    --             RoleCtrl.Instance:SendRoleCareerChangeReq({
    --                 to_profession = self.view_data.prof_view_data.prof_id,
    --                 avartar_type = self.view_data.prof_view_data.appear_id, 
    --                 color = self.view_data.prof_view_data.color_index,
    --             })
    --         RoleCtrl.Instance:SendRequestChangeProfTime()
    --         PublicPopupCtrl.Instance:CloseDialogTips()

    --         ViewMgr:CloseView(NeoProfChangeView)
    --         self.view_data.prof_view_data.is_reast = true
    --         end
    --     }
    -- }
end

function NeoProfChangePanel:ReastPanel()
    if not self.view_data.prof_view_data.is_reast then return end 
    self.view_data.prof_view_data.is_reast = false
    self.ProfListObj:SetActive(false)
    self.ColorGroupObj:SetActive(false)
    self.ColorGroup:SetAllTogglesOff()
    self.ProfAttrGroup:SetActive(false)
    self.StuffGroup:SetActive(false)
    self.CenterAnim.enabled = true
    self.CenterAnim:SetTrigger(APH("ChangeCur"))
    
    self.ToggleGroup:SetAllTogglesOff()
    -- local cur_select_chara = self.CharaList:GetItem(self.view_data.prof_view_data.app_index)
    -- cur_select_chara:ReastCell()
    -- self.view_data:ClearProfViewData()
    self.view_data:ResetRoleAppData()

end

NeoCharaListItem = NeoCharaListItem or DeclareMono("NeoCharaListItem", UIWidgetBaseItem)

function NeoCharaListItem:NeoCharaListItem()

end

function NeoCharaListItem:SetData(data)
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
		local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset("Headp"..data.avartar_type)
		if stream_has then
			ChannelAgentConsts.SetRawImg(self.NomalIconRawImg,img_path)
			ChannelAgentConsts.SetRawImg(self.SelectIconRawImg,img_path)
			self.NomalIconRawImgObj:SetActive(true)
			self.SelectIconRawImgObj:SetActive(true)
			UH.SpriteName(self.NomalIconImg)
			UH.SpriteName(self.SelectIconImg)
        else
            UH.SpriteName(self.NomalIconImg, "headp"..data.avartar_type)
            UH.SpriteName(self.SelectIconImg, "headp"..data.avartar_type)
		end
	else
		UH.SpriteName(self.NomalIconImg, "headp"..data.avartar_type)
		UH.SpriteName(self.SelectIconImg, "headp"..data.avartar_type)
	end
    self.ProfPosRect.anchoredPosition = Vector2.New(126, data.index < 4 and -5 or 20)	
    UIWidgetBaseItem.SetData(self, data)
end

function NeoCharaListItem:ReastCell()

    self.Toggle.isOn = false
    self.SelectIcon:SetActive(false)
    self.NomalIcon:SetActive(true)
end

NeoProfListItem = NeoProfListItem or DeclareMono("NeoProfListItem", UIWidgetBaseItem)
function NeoProfListItem:NeoProfListItem()

end

function NeoProfListItem:SetData(data)
    local icon_name = RoleData.GetProfSpByProfId(data.job_id, true)
    local prof_name = RolePromoteData.GetProfName(data.job_id)
    self.SelectImg.SpriteName = icon_name
    self.NomalImg.SpriteName = icon_name

    self.SelectName.text = prof_name
    self.NomalName.text = prof_name
    UIWidgetBaseItem.SetData(self, data)
end

function NeoProfListItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

NeoProfSkillItem = NeoProfSkillItem or DeclareMono("NeoProfSkillItem", UIWidgetBaseItem)

function NeoProfSkillItem:NeoProfSkillItem()
    -- body
end

function NeoProfSkillItem:SetData(data)
    self.SkillName.text = data.skill_name
    UH.SetIcon(self.SkillIcon,data.icon_id,ICON_TYPE.SKILL)
    UIWidgetBaseItem.SetData(self, data)
end

function NeoProfSkillItem:Click()
    local rand_jod_info = MiniGameData.Instance:CreateRandJobInfo()
    local prof_id = 0
    if self.Index then
        prof_id = rand_jod_info[self.Index].job_id
    end
    if NeoCharaChangeData.Instance.prof_view_data.prof_id > 0 then
        prof_id = NeoCharaChangeData.Instance.prof_view_data.prof_id
    end
    local desc_info = Cfg.GetChangeJobDesc(RoleData.ProfIdToProfType(prof_id),self.data.skill_id)
    if desc_info == nil then 
        LogError("empty desc!",RoleData.ProfIdToProfType(prof_id),self.data.skill_id)
        return 
    end

    local data = {
        tips_str = string.format(Language.Role.RolePromoteSkillTips,desc_info.skill_name,desc_info.skill_level,desc_info.description),
        is_show = true
        -- pos_info = self.SkillItemRect.localPosition,
        -- height = self.SkillItemRect.rect.height,
        -- onclick = true
    }
    
    if not StringIsEmpty(self.SpColor) then
        local color = COLORSTR[self.SpColor]
        data.tips_str = string.format(Language.Role.RolePromoteSkillTipsSpColor, color, desc_info.skill_name, color, desc_info.skill_level, color, desc_info.description)
    end
    NeoCharaChangeData.Instance:SetProfViewSkill(data)
end

--------------------------------------------
NeoChangeTipsView = NeoChangeTipsView or DeclareView("NeoChangeTipsView", "neo_chara_change/neo_change_tips")
function NeoChangeTipsView:LoadCallback(data)
    self.data = data
    self.Board:SetData(self:GetType(), Language.RoleAppearChange.ChangeTipsView, Vector2.New(682, 520))
    UH.SetText(self.Desc, Language.RoleAppearChange.WarningTisp)
end 

function NeoChangeTipsView:OnClickOk()
    if self.data.func ~= nil then
        self.data.func()

        ViewMgr:CloseView(NeoChangeTipsView)
    end 
end