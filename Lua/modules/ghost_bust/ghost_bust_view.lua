GhostBustView = GhostBustView or DeclareView("GhostBustView", "ghost_bust/ghost_bust", Mod.GhostBust.Main)
function GhostBustView:GhostBustView()
    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_YAO_XING_SHI_LIAN)
end

function GhostBustView:OpenCallback()
    self.panel:FlushView()
end

function GhostBustView:OnClickClose()
    ViewMgr:CloseView(GhostBustView)
end

GhostBustPanel = GhostBustPanel or DeclareMono("GhostBustPanel", UIWFlushPanel)
function GhostBustPanel:GhostBustPanel()
    self.data = GhostBustData.Instance

    self.data_cares = {
        {data = self.data.ghost_info,func = self.FlushView,init_call = false},        
        {data = self.data.view_data,func = self.PlayRoleShow,init_call = false},        
    }

    self.cell_list:SetCompleteCallBack(function()
        self:CompleteList()
    end)

    Runner.Instance:AddRunObj(self, 16)

    self.AutoImg:SetActive(PrefsInt(PrefKeys.GhostBustAuto())==1)

    if self.data.wait_viedo then 
        ViewMgr:OpenView(GhostBustPreView)
        self.data:WaitVideo(false)
    end 
end

function GhostBustPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    Runner.Instance:RemoveRunObj(self)
    
    if self.team_wait ~= nil then
        TimeHelper:CancelTimer(self.team_wait)
        self.team_wait = nil
    end
end

function GhostBustPanel:FlushView()
    local cell_data,mark,lock = self.data:CheckSeqPartNeo()
    self.cell_list:SetData(cell_data or {})
    self.gbs:FlushGray(mark)
    self.init_mark = mark
    self.is_lock = lock

    self.achieve_red:SetNum(self.data:GetAchieveRed() and 1 or 0)
end

function GhostBustPanel:CompleteList()
    for k,v in pairs(self.cell_list.item_list)do 
        local pos_fix = self.gbs.poses[k]
        if pos_fix ~= nil then 
            v.gameObject:SetParent(pos_fix,false)
            UH.AnchoredPosition(v.gameObject.transform, Vector2.zero)
            v.gameObject:SetParent(self.cell_list.ContentParent,true)
        end 
        v:SetIndex(k)

        -- 开启界面自动贴已完成的点
        self:Stand()
    end 

    self.cell_list:ClickItem(self.is_lock and self.init_mark or self.init_mark +1)

    -- if self.data.wait_viedo then 
    --     self:PlayRoleShow()
    --     self.data:WaitVideo(false)
    -- end 
end

function GhostBustPanel:PlayRoleShow()
    if not self:MarkCheck() then return end 
    self:MoveTo()
end

-- 从init_mark移动到init_mark+1
function GhostBustPanel:MoveTo()
    if GhostBustData.Instance:GetNextData() == nil then return end 
    -- 坐标获取
    local start_pos = self.cell_list.item_list[self.init_mark].gameObject.transform.localPosition
    local end_pos = self.cell_list.item_list[self.init_mark+1].gameObject.transform.localPosition
    local old_pos = self.cell_list.ContentParent.transform.localPosition

    -- 灰进度获取
    local start_gray = GhostBustData.GBMask[self.init_mark].fill
    local end_gray = GhostBustData.GBMask[self.init_mark+1].fill

    UH.AnchoredPosition(self.cell_list.ContentParent.transform, Vector2.New(-start_pos.x,0))
    self.step_count = 0
    if ViewMgr:IsOpen(GhostBustView) then 
        self.single_runner = TimeHelper:AddRunFrameTimer(function ()
            if not ViewMgr:IsOpen(GhostBustView) then 
                return 
            end 
            self.step_count = self.step_count + 1

            local step_x = self.cell_list.ContentParent.transform.anchoredPosition.x - (end_pos.x - start_pos.x) / 30
		    UH.AnchoredPosition(self.cell_list.ContentParent.transform, Vector3.New(step_x, 0,0))

            local step_gray = self.gbs.Slider.value + (end_gray - start_gray) / 30
            self.gbs.Slider.value = step_gray

            if self.step_count >= 30 then 

            end 
        end,1,30)
    end
end

-- 检查init_mark
function GhostBustPanel:MarkCheck()
    -- 检查位置是否正确
    local flag_1 = self.init_mark > 0 and self.init_mark < 10 and not self.cell_list.item_list[self.init_mark].data.empty
    local flag_2 = self.data.ghost_info.pass_seq > 2
    return flag_1 and not self.is_lock and flag_2
end

-- 初始化站位
function GhostBustPanel:Stand()
    -- 坐标获取
    local mark = self.cell_list.item_list[ self.init_mark ].gameObject.transform.anchoredPosition
    local old_pos = self.cell_list.ContentParent.transform.anchoredPosition
    UH.AnchoredPosition(self.cell_list.ContentParent.transform,  Vector3.New(-mark.x,old_pos.y,old_pos.z))
    if self.data.ghost_info.pass_seq <= 2 then 
        UH.AnchoredPosition(self.cell_list.ContentParent.transform,  Vector3.New(771,old_pos.y,old_pos.z))
    end 

end

-- 根据点击刷新desc的信息组
function GhostBustPanel:OnClickItem(data)
    -- self.chaprter_cap 
    self.chapter_info:SetActive(true)
    self.reward:SetActive(true)

    local ckeck_mark = self.is_lock and self.init_mark or self.init_mark +1
    if data.seq ~= self.cell_list.item_list[ckeck_mark].data.seq then 
        if data.index > self.init_mark then 
            PublicPopupCtrl.Instance:Center(Language.GhostBust.MonsterLocking)
        else
            PublicPopupCtrl.Instance:Center(Language.GhostBust.MonsterPass) 
        end 
        return 
    end 

    UH.SetText(self.chapter_cap, "/"..data.score)
    -- 获取总分
    self.role_cap.text = self.data:GetFormationScore()
    UH.SetText(self.chapter_txt, data.desc)

    self.reward_list:SetData(BagData.Instance:ItemListBuild(data.clear_reward))

    for k,v in pairs(self.cell_list.item_list) do
        v:SetSelectShow(k == data.index)
    end 

    self.btn_inter.Interactable = self.data:GetCellIsChallenging(data.seq)

    self.select_data = data
end

function GhostBustPanel:OnClickAchieve()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    ViewMgr:OpenView(GhostBustAchieve)
end 

function GhostBustPanel:Update()
    if ViewMgr:IsOpen(GhostBustAchieve) and not self.achieve_wait then 
        self.achieve_wait = true  
    end 

    if not ViewMgr:IsOpen(GhostBustAchieve) and self.achieve_wait then 
        self.achieve_red:SetNum(self.data:GetAchieveRed() and 1 or 0)
        self.achieve_wait = false
    end 

    if ViewMgr:IsOpen(ActFormationView) and not self.format_wait then
        self.format_wait = true
    end 

    if not ViewMgr:IsOpen(ActFormationView) and self.format_wait then 
        self.role_cap.text = self.data:GetFormationScore()
        -- self.format_wait = false
    end 

end 

function GhostBustPanel:OnClickVedio()
    -- if LoginData.Instance:BaseData().is_on_crosssever == true then 
    --     PublicPopupCtrl.Instance:Center(Language.GhostBust.CrossVedioError)         
    --     return 
    -- end 

    local cur_need = self.select_data.score
    local my_score = self.data:GetFormationScore()
    local group = my_score > cur_need and self.select_data.monster_two or self.select_data.monster_one
	local video_config = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_YAO_XING_SHI_LIAN, group)
	if video_config ~= nil then
		BattleVideo.Instance:PlayVideoCfg(video_config)
		BattleCtrl.BattleReportDetailInfo(video_config.monster_group_id)
		self.IsGoToPlayVideo = true

        self.data:WaitVideo(true)
	end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function GhostBustPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[294].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)

    -- self:CompleteList()
end

function GhostBustPanel:OnClickBuZhen()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_YAO_XING_SHI_LIAN)
end

function GhostBustPanel:OnClickChallenge()
    TeamData.Instance:CheckSingle(function ()
        self:StartChallenge()
    end)

    -- local function send_msg()
    --     TeamCtrl.Instance:SendExitTeam()
    --     PublicPopupCtrl.Instance:CloseDialogTips()


    --     if self.team_wait ~= nil then
    --         TimeHelper:CancelTimer(self.team_wait)
    --         self.team_wait = nil
    --     end
    --     self.team_wait =  TimeHelper:AddDelayFrameTimer(function ()
    --         self:StartChallenge()
    --     end,2)
    -- end

    -- if TeamData.Instance:InTeam() then 
    --     local info = {
    --         content = Language.GhostBust.TeamErrorTips,
    --         cancel = {
    --             name = nil,
    --             func = function ()
    --                 PublicPopupCtrl.Instance:CloseDialogTips()
    --             end
    --         },
    --         confirm = {
    --             name = nil,
    --             func = send_msg
    --         }
    --     }
    --     PublicPopupCtrl.Instance:DialogTips(info)
    --     return
    --     -- PublicPopupCtrl.Instance:Center(Language.LuckyRing.TeamErrorTips) 
    -- end 

    -- self:StartChallenge()
end

function GhostBustPanel:StartChallenge()
    if self.data:CanChallenge(self.select_data.seq) then 
        return 
    end 

    local function send_msg()
        self:OnClickBuZhen()
        PublicPopupCtrl.Instance:CloseDialogTips()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    ActFormationData.Instance:SetCurFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_YAO_XING_SHI_LIAN)
    if ActFormationData.Instance:GetAssistCount() < 4 then
        local info = {
            content = Language.GhostBust.PetErrorDialog,
            cancel = {
                name = nil,
                func = function ()
                    ActFormationData.Instance.formation_smart.auto_save_type = -1
                    PublicPopupCtrl.Instance:CloseDialogTips()
                    
                    GhostBustCtrl.Instance:SendGhostBustSeq(1)
                end
            },
            confirm = {
                name = nil,
                func = send_msg
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end

    GhostBustCtrl.Instance:SendGhostBustSeq(1)
end

function GhostBustPanel:OnToggleAuto()
	local is_active = not GetActive(self.AutoImg)
	if is_active then
		PublicPopupCtrl.Instance:Center(Language.CourageChallenge.AutoFightTips)
	end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.AutoImg:SetActive(is_active)
	PrefsInt(PrefKeys.GhostBustAuto(),is_active and 1 or 0)
end

GhostBustBgs = GhostBustBgs or DeclareMono("GhostBustBgs", UIWFlushPanel)
function GhostBustBgs:FlushGray(pass)
    self.Slider.value = GhostBustData.GBMask[pass].fill
end 

GhostBustMonsterCell = GhostBustMonsterCell or DeclareMono("GhostBustMonsterCell", UIWidgetBaseItem)
function GhostBustMonsterCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.gameObject:SetActive(not data.empty)
    if data.empty == true then return end

    local mon_group = Cfg.MonsterGroupById(data.monster_one)
    local mon_config = Cfg.MonsterById(mon_group.show_monster, false)
    local is_pass = GhostBustData.Instance:GetCellIsPass(data.seq)
    local role_level = RoleData.Instance:GetRoleLevel()
    local is_level_e = role_level >= data.level_limit
    UH.SetText(self.name,is_pass and Language.GhostBust.CellPass or mon_config.name)
    UH.SetText(self.limit,is_level_e and Language.GhostBust.LimitSimple or string.format(Language.GhostBust.LimitLevel,data.level_limit))--string.format(Language.GhostBust.Limit,data.score))
    UH.SetText(self.chacter,string.format(Language.GhostBust.Chapter,DataHelper.GetWord(data.seq)))

    self.limt_obj:SetActive(GhostBustData.Instance:GetCellIsLock(data.seq))
    -- if not GhostBustData.Instance:GetCellIsLock(data.seq) then 
        self.lock:SetActive(GhostBustData.Instance:GetCellIsLock(data.seq))
    -- end 
    
    self.inter.Interactable = not GhostBustData.Instance:GetCellIsLock(data.seq)

    self.mod_obj:SetActive(not is_pass)
    self.complete:SetActive(is_pass)

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.model_show:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
            seq = 1,
        })
    end

    self.ui_obj:SetData(DrawerHelper.GetNpcPath(mon_config.res_id))

    UH.LocalPosG(self.title_obj,Vector3(0,GhostBustData.Instance:GetCellIsLock(data.seq) and -85 or -53 ,0))
end 

function GhostBustMonsterCell:SetIndex(index)
    self.data.index = index    
    self.gameObject.name = "chapter_cell_"..index
end

function GhostBustMonsterCell:SetSelectShow(flag)
    self.select:SetActive(flag) 
    self.select_eff:SetActive(flag) 

    -- self.arrow_obj:SetActive(flag and not GhostBustData.Instance:GetCellIsChallenging(self.data.seq) )
end

function GhostBustMonsterCell:Click()
    if self.ItemClick ~= nil then 
        self.ItemClick:Trigger()
    end 
end

function GhostBustMonsterCell:OnRelease()
    UIWidgetBaseItem.OnRelease(self)

    if self.setPosRunner ~= nil then
        TimeHelper:CancelTimer(self.setPosRunner)
        self.setPosRunner = nil
    end

    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end 
