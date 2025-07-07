PetFeiSheng = PetFeiSheng or DeclareView("PetFeiSheng", "pet/pet_feisheng",Mod.Pet.PetFeiSheng)
VIEW_DECLARE_LEVEL(PetFeiSheng,ViewLevel.L1)

----Board=======UIWBoard
----Tabbar======UIWidgetList
function PetFeiSheng:PetFeiSheng()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
end
function PetFeiSheng:LoadCallback(param_t)

end

function PetFeiSheng:CloseCallback()

end

function PetFeiSheng:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[192].desc})
    -- PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = "待定"})
end

function PetFeiSheng:OnClickReturn()
    ViewMgr:CloseView(PetFeiSheng)    
end

function PetFeiSheng.LvName(fs_lv)
    return fs_lv == PetFeiShengData.Lv.Two and Language.PetFeiSheng.NameTwo or Language.PetFeiSheng.NameOne
end

function PetFeiSheng.LvNameFormatStr(fs_lv,format_str)
    local lv_name = PetFeiSheng.LvName(fs_lv)
    return string.format(format_str,lv_name)
end


----------------------------------------------------------------------
PetFeiShengPanel = PetFeiShengPanel or DeclareMono("PetFeiShengPanel", UIWFlushPanel)
--EffectTool==========UIEffectTool
--ModelShow===========ModelUIShow
--PetNameText=========Text
--FlyStarEffs1========FlyStarEffectCell[]
--FlyStarEffs2========FlyStarEffectCell[]
--ClickBlock==========GameObject
--FuncObj=============GameObject
--FuncBtnText=========Text
--TitleTxt============Text
--PreCondiTitleTxts===Text[]
--RightTitlext========Text

--FuncBtnInter========InteractorHandler

local effid_star = "3165037"
local effid_com = "3165039"

function PetFeiShengPanel:PetFeiShengPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    self.data = PetFeiShengData.Instance
    self.pet_index = -1

    self.data_cares = {
        -- {data = self.pet_data.pet_select, func = self.FlushPetInfo, keys = {"index"}},
        -- {data = self.pet_data.pet_list, func = self.FlushInfo, },
    }
    self.cur_pet = self.pet_data:GetCurSelect()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(self.cur_pet:ResId()),self.cur_pet:FeiShengTimes())
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
        })
    end
    self.playing_star_count = 0
    for i = PetFeiShengData.Lv.One,PetFeiShengData.Lv.Max do
        local effs = self["FlyStarEffs" .. i]
        if effs == nil then break end
        for i=1,effs:Length() do
            local star_eff = effs[i]
            local com_eff = effs:Length() == i and effid_com or nil
            star_eff:SetData({
                mono = self,
                -- start_effect = "2161041",
                complete_effect = com_eff,
                pre_pause = 0.5,
                end_func = BindTool.Bind(self.OnFlyStartPlayComplete, self,star_eff),
                -- run_func = 0.1,
                -- start_wait_time = space_t * (i - 1),
                end_func_time = 0,
            })
        end 
    end


    self:onInit()
end

function PetFeiShengPanel:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    UIWFlushPanel.OnDestroy(self)
end


function PetFeiShengPanel:showFSAttri(cfg,attr_type_name,attr_num_name)
    if cfg == nil then return end
    for i=1,4 do        --最大属性是4
        local i_str = tostring(i)
        local attr_type = cfg[attr_type_name .. i_str]
        if attr_type >= 0 then
            local attr_num = cfg[attr_num_name .. i_str]
            if attr_num > 0 then
                UH.SetText(self["AttriTxts"][i], string.format("%s +%s",Language.Common.AttrList[attr_type],DataHelper.Percent(attr_type,attr_num)))
            else
                UH.SetText(self["AttriTxts"][i],"")
            end
        else
            UH.SetText(self["AttriTxts"][i],"")
        end
    end    
end

function PetFeiShengPanel:onInit()
    local pet = self.cur_pet
    if pet == nil then
        return
    end
    local target_fs_lv = pet:FeiShengTimes() + 1
    UH.SetText(self.TitleTxt,PetFeiSheng.LvNameFormatStr(target_fs_lv,Language.PetFeiSheng.Title))
    UH.SetText(self.RightTitlext,PetFeiSheng.LvName(target_fs_lv))
    for i = 1 ,self.PreCondiTitleTxts:Length() do
        UH.SetText(self.PreCondiTitleTxts[i],string.format(Language.PetFeiSheng.PreCondiTitle,
            PetFeiSheng.LvName(target_fs_lv),Language.DataHelper.Word[i]))
    end
    UH.SetText(self.PetNameText, pet:Name())
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()),pet:FeiShengTimes())
    self.ui_obj:PlayAnim(ChAnimType.Show)
    -- for i = 1, PetFeiShengData.Lv.Max do
    --     self.Sections[i]:SetActive(target_fs_lv == i)
    -- end
    -- for i = 1, 4 do
    --     self["SectionList"..target_fs_lv][i]:SetData({pet_index = pet:Index()})
    -- end
    for i = 1, PetFeiShengData.PRE_COND_COUNT do
        local is_com,desc =  PetFeiShengData.GetFlyPreInfo(pet:Index(),i,target_fs_lv)
        self.Conditions[i]:SetData({pre_i = i,fs_lv = target_fs_lv,flag = is_com,condition_desc = desc})
    end
    --前置任务没完成/都完成了可以飞升，不显示功能按钮
    local pre_codi = PetFeiShengData.IsAllPreCom(pet:Index(),target_fs_lv)
    -- local can_feisheng = pet:FeiShengJieDuan() / PetFeiShengData.STAGE_COUNT >= target_fs_lv

    UH.SetText(self.FuncBtnText,PetFeiSheng.LvName(target_fs_lv))

    if pre_codi == false then
        self.FuncBtnInter.Interactable = false
        -- self.FuncObj:SetActive(false)
    else--if can_feisheng == true then
        -- UH.SetText(self.FuncBtnText,PetFeiSheng.LvName(target_fs_lv))
        self.FuncBtnInter.Interactable = true
        -- self.FuncObj:SetActive(true)
    -- else
    --     UH.SetText(self.FuncBtnText,string.format(Language.PetFeiSheng.FuncBtnText, Language.DataHelper.FanTi[pet:FeiShengJieDuan() + 1]))
    --     self.FuncObj:SetActive(true)
    end
    UH.SetText(self.AttriTitle,PetFeiSheng.LvNameFormatStr(target_fs_lv,Language.PetFeiSheng.TitleAttri))
    UH.SetText(self.SkillTitle,PetFeiSheng.LvNameFormatStr(target_fs_lv,Language.PetFeiSheng.TitleSkill))
    UH.SetText(self.UnlockTitle,PetFeiSheng.LvNameFormatStr(target_fs_lv,Language.PetFeiSheng.TitleUnlock))
    
    local attr_cfg = Cfg.PetFeiShengAttri(pet:ID(),target_fs_lv)
    self:showFSAttri(attr_cfg,"att_type","att_num")
    UH.SetText(self.UnlockTxt,Language.PetFeiSheng["SuccUnlock" .. target_fs_lv])
    self.SkillObj:SetActive(target_fs_lv == PetFeiShengData.Lv.One)

    for i = 1,PetFeiShengData.SKILL_COUNT do
        local skill_id = Config.pet_cfg_auto.other[1]["fly_skill_id" .. tostring(i)]
        local skill_cfg = SkillData.GetDecConfig(skill_id)
        UH.SetIcon(self.SkillIcons[i],skill_cfg.icon_id,ICON_TYPE.SKILL)
        UH.SetText(self.SkillNames[i],skill_cfg.skill_name)
    end
end

function PetFeiShengPanel:OnClickFeiSheng()
end

function PetFeiShengPanel:OnClickSkillItem(idx)
    local skill_id = Config.pet_cfg_auto.other[1]["fly_skill_id" .. tostring(idx)]
    local skill_cfg = SkillData.GetDecConfig(skill_id)
    local skill_data = PetData.GetEmptySkillData(self.cur_pet:Index())
    skill_data.skill_id = skill_id
    skill_data.skill_cfg = skill_cfg
    skill_data.skill_level = 1
    skill_data.fs_skill = true
    skill_data.owner_pet = self.cur_pet
    PetData.Instance:SetClickSkill(skill_data)
    ViewMgr:OpenView(PetSkillTip)
end

function PetFeiShengPanel:exeFeisheng()
    -- local cur_pet = self.cur_pet
    -- if cur_pet:FeiShengJieDuan() / PetFeiShengData.STAGE_COUNT < cur_pet:FeiShengTimes() + 1 then
    --     PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskPreNeedComplete)
    --     return
    -- end
    -- --前置任务完成，开始飞升（进入选择飞升技能流程）
    -- local effs = self["FlyStarEffs" .. cur_pet:FeiShengTimes() + 1]
    -- if effs == nil then
    --     return
    -- end
    -- self.playing_star_count = effs:Length()
    -- for i=1,effs:Length() do
    --     effs[i]:Play(effid_star) 
    -- end
    -- self.ClickBlock:SetActive(true)
    self:OnFeiShengShowComplete()
end

function PetFeiShengPanel:OnFlyStartPlayComplete(star_eff)
    star_eff:AllHandleCancel()
    self.playing_star_count = self.playing_star_count - 1
    if self.playing_star_count <= 0 then
        self:OnFeiShengShowComplete()
    end
end
function PetFeiShengPanel:OnFeiShengShowComplete()
    local cur_pet = self.cur_pet
    PetFeiShengData.Instance:SetSuccData(cur_pet:Index(),cur_pet:FeiShengTimes() + 1)
    ViewMgr:OpenView(PetFeiShengSucc)
end

function PetFeiShengPanel:OnDestroy()
    self:removeWatiTaskRunner()
    UIWFlushPanel.OnDestroy(self)
end

function PetFeiShengPanel:removeWatiTaskRunner()
    if self.wait_task_runner ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.wait_task_runner)
        self.wait_task_runner = nil
    end 
end


function PetFeiShengPanel:ExeTask()

    local cur_task_stage = self.cur_pet:FeiShengJieDuan() + 1
    -- if cur_task_stage > self.ItemIndex then
    --     PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskCompleted)
    -- elseif cur_task_stage < self.ItemIndex then
    --     PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskPreNeedComplete)
    -- else

        if PetFeiShengData.Instance:IsTodayTaskFinished() then
            PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskErrTodayFinished)
        elseif PetFeiShengData.Instance:HasCurTaskData() then
            if PetFeiShengData.Instance:IsInTask(self.cur_pet) then  --如果点了已接的自己的任务则去寻路完成
                local task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.pet_soar)--TaskData.Instance:GetTaskInfoByTaskId(PetFeiShengData.Instance:CurTaskId())
                if task_info == nil then
                    PublicPopupCtrl.Instance:Center(string.format(Language.PetFeiSheng.TaskErrNoTaskInfo,PetFeiShengData.Instance:CurTaskId()))
                else
                    task_info:GoTask()
                    ViewMgr:CloseView(PetFeiSheng)
                    ViewMgr:CloseView(PetView)
                end
            else
                PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskErrIsInOtherTask)
            end
        else

            local func = function ()
                PetFeiShengCtrl.Instance:RequestTask(self.cur_pet:Index())
                self:removeWatiTaskRunner()
                self.wait_task_runner = Runner.Instance:RunUntilTrue(function() 
                    local task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.pet_soar)
                    if task_info ~= nil then
                        task_info:GoTask()
                        PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskSucc)
                        ViewMgr:CloseView(PetFeiSheng)
                        ViewMgr:CloseView(PetView)
                        self.wait_task_runner = nil
                        return true
                    end
                    return false
                end)
            end
            if PetFeiShengData.Instance:IsInTask(self.cur_pet) then
                func()
            else
                PublicPopupCtrl.Instance:DialogTips({
                    content = string.format(Language.PetFeiSheng.TaskAccpetTips, self.cur_pet:Name()),
                    confirm = {
                        func = function()
                            ViewMgr:CloseView(DialogTipsView)
                            func()
                        end,
                        name = Language.Common.Comfirm,
                        wait_time = 5,
                    }
                })
            end
        end
    -- end
end

function PetFeiShengPanel:OnClickGotoTask()
    local target_fs_lv = self.cur_pet:FeiShengTimes() + 1
    if PetFeiShengData.IsAllPreCom(self.cur_pet:Index(),target_fs_lv) == false then
        PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.PreCondiNeedComplete)
        return
    end
    local can_feisheng = self.cur_pet:FeiShengJieDuan() / PetFeiShengData.STAGE_COUNT >= target_fs_lv
    if can_feisheng then
        self:exeFeisheng()
        return
    end
    self:ExeTask()
end

----------------------------------------
PetFeiShengTaskItem = PetFeiShengTaskItem or DeclareMono("PetFeiShengTaskItem", UIWidgetBaseItem)
--ItemIndex=============Int
--TaskBg================UIImageSpriteSet
--AttrText1=============Text
--AttrText2=============Text
--TaskText==============Text
--GrayInter=============InteractorHandler
--DescText==============Text
--FlyTxt================Text(任务Item没有)
function PetFeiShengTaskItem:PetFeiShengTaskItem()
    self.pet_data = PetData.Instance
end

local max_attri_count = 2
function PetFeiShengTaskItem:showFSAttri(cfg,attr_type_name,attr_num_name)
    if cfg == nil then return end
    for i=1,max_attri_count do
        local i_str = tostring(i)
        local attr_type = cfg[attr_type_name .. i_str]
        if attr_type >= 0 then
            local attr_num = cfg[attr_num_name .. i_str]
            if attr_num > 0 then
                UH.SetText(self["AttrText" .. i_str], string.format("%s +%d",Language.Common.AttrList[attr_type],attr_num))
            else
                UH.SetText(self["AttrText" .. i_str],"")
            end
        else
            UH.SetText(self["AttrText" .. i_str],"")
        end
    end    
end

function PetFeiShengTaskItem:isFeishengItem()
    return self.ItemIndex == 0
end


function PetFeiShengTaskItem:SetData(data)
    self.data = data
    local cur_pet = self.pet_data:GetPetInfo(self.data.pet_index)
    local cur_task_stage = cur_pet:FeiShengJieDuan()
    local target_fs_time = cur_pet:FeiShengTimes() + 1
    local is_next_stage_task = self.ItemIndex - cur_task_stage == 1
    --LogError("cur_task_stage===",self.ItemIndex,cur_task_stage,cur_pet:FeiShengTimes())
    if not self:isFeishengItem() then
        UH.SetText(self.TaskText, Language.DataHelper.FanTi[self.ItemIndex])
        local attr_cfg = PetFeiShengData.GetFSTaskAttriCfg(cur_pet:NormalId(),target_fs_time,self.ItemIndex)
        self:showFSAttri(attr_cfg,"att_type_","att_")

        local is_all_pre_com = PetFeiShengData.IsAllPreCom(cur_pet:Index(),target_fs_time)
        -- local tasking = false
        -- if self.ItemIndex == 1 then
        --     tasking = (cur_task_stage + 1 == self.ItemIndex) and is_all_pre_com
        -- else
        --     tasking = (cur_task_stage + 1 == self.ItemIndex)
        -- end
        -- UH.SpriteName(self.TaskBg,tasking and "DaDaoDi" or "WeiDaDaoDi")
        -- tasking = (cur_task_stage + 1 == self.ItemIndex) and is_all_pre_com
        self.GrayInter.Interactable = is_all_pre_com and    --前置都完成
            ((is_next_stage_task and PetFeiShengData.Instance:IsInTask(cur_pet) and PetFeiShengData.Instance:IsTodayTaskFinished() == false) or    --已接当前任务
            cur_task_stage >= self.ItemIndex)   --任务已完成
        
        local desc_txt
        if self.GrayInter.Interactable == true then
            desc_txt = ""
        elseif is_all_pre_com == false and (self.ItemIndex % PetFeiShengData.STAGE_COUNT) == 1 then
            desc_txt = Language.PetFeiSheng.TaskPreNeedComplete
        elseif is_next_stage_task then    --下一个任务判断
            if PetFeiShengData.Instance:IsTodayTaskFinished() or PetFeiShengData.Instance:HasCurTaskData() then
                desc_txt = Language.PetFeiSheng.DescLimitDay
            else    --任务可接，不给提示
                desc_txt = ""
            end
        else
            desc_txt = string.format(Language.PetFeiSheng.DescLimitNextDay,Language.DataHelper.FanTi[self.ItemIndex - 1])
        end
        UH.SetText(self.DescText,desc_txt)
    else
        UH.SetText(self.FlyTxt,PetFeiSheng.LvName(target_fs_time))
        self.GrayInter.Interactable = ((cur_task_stage / PetFeiShengData.STAGE_COUNT) >= target_fs_time)
        local pet_id = cur_pet:NormalId()
        local attr_cfg = Cfg.PetFeiShengAttri(pet_id,target_fs_time)
        self:showFSAttri(attr_cfg,"att_type","att_num")
        local desc_txt = ""
        if self.GrayInter.Interactable == false then
            local stage_i_base = (target_fs_time - 1) * PetFeiShengData.STAGE_COUNT
            desc_txt = string.format(Language.PetFeiSheng.DescFeiSheng,Language.DataHelper.FanTi[stage_i_base + 1],
                Language.DataHelper.FanTi[stage_i_base + 2],Language.DataHelper.FanTi[stage_i_base + 3],PetFeiSheng.LvName(target_fs_time))
        end
        UH.SetText(self.DescText,desc_txt)
    end
end


-- function PetFeiShengTaskItem:OnClickTask()
--     local cur_pet = self.pet_data:GetPetInfo(self.data.pet_index)
--     if PetFeiShengData.IsAllPreCom(cur_pet:Index(),cur_pet:FeiShengTimes() + 1) == false then
--         PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.PreCondiNeedComplete)
--         return
--     end
--     local cur_task_stage = cur_pet:FeiShengJieDuan() + 1
--     if cur_task_stage > self.ItemIndex then
--         PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskCompleted)
--     elseif cur_task_stage < self.ItemIndex then
--         PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskPreNeedComplete)
--     else

--         if PetFeiShengData.Instance:IsTodayTaskFinished() then
--             PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskErrTodayFinished)
--         elseif PetFeiShengData.Instance:HasCurTaskData() then
--             if PetFeiShengData.Instance:IsInTask(cur_pet) then  --如果点了已接的自己的任务则去寻路完成
--                 local task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.pet_soar)--TaskData.Instance:GetTaskInfoByTaskId(PetFeiShengData.Instance:CurTaskId())
--                 if task_info == nil then
--                     PublicPopupCtrl.Instance:Center(string.format(Language.PetFeiSheng.TaskErrNoTaskInfo,PetFeiShengData.Instance:CurTaskId()))
--                 else
--                     task_info:GoTask()
--                     ViewMgr:CloseView(PetFeiSheng)
--                     ViewMgr:CloseView(PetView)
--                 end
--             else
--                 PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskErrIsInOtherTask)
--             end
--         else
--             PetFeiShengCtrl.Instance:RequestTask(cur_pet:Index())
--             PublicPopupCtrl.Instance:Center(Language.PetFeiSheng.TaskSucc)
--             -- LogError("PetFeiShengCtrl.Instance:RequestTask===",cur_pet:Index())
--         end
--     end
-- end



-----------------------------------------

local condition_goto_funcs = {
    [PetFeiShengData.PreType.PetLv] = function()
        ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
    end,
    [PetFeiShengData.PreType.PetScore] = function()
        ViewMgr:OpenViewByKey(Mod.Pet.PetAttr)
    end,
    [PetFeiShengData.PreType.RoleFlyUpLv] = function()
        ViewMgr:OpenViewByKey(Mod.Role.RoleAttr)
    end,    
    [PetFeiShengData.PreType.PetStrenLv] = function()
        ViewMgr:OpenViewByKey(Mod.Pet.PetStrChange)
    end,    
}

PetFeiShengConditionItem = PetFeiShengConditionItem or DeclareMono("PetFeiShengConditionItem", UIWidgetBaseItem)
--ConditionText=======Text
--ConditionProTxt=====Text
--ConditionProImg=====UIImageSpriteSet
--ParentPanel=========PetFeiShengPanel
function PetFeiShengConditionItem:PetFeiShengConditionItem()
    self.pet_data = PetData.Instance
end

function PetFeiShengConditionItem:SetData(data)
    self.data = data
    self.GotoBtnObj:SetActive(not data.flag)
    self.CompleteObj:SetActive(data.flag)

    local cfg = PetFeiShengData.GetPreConditionCfg(self.data.pre_i,self.data.fs_lv)
    UH.Color(self.ConditionText,data.flag and COLORS.Green10 or COLORS.Red8)
    if cfg ~= nil and cfg.fly_type == PetFeiShengData.PreType.PetStrenLv then       --强化条件要显示强化图标
        local desc = Language.PetFeiSheng.PreDescs[cfg.fly_type]
        local txt_arr = string.split(desc,"|")
        UH.SetText(self.ConditionText,txt_arr[1])
        self.ConditionProImg:SetObjActive(true)
        PetData.SetStrLevel(nil,self.ConditionProImg,cfg.param1)
        if #txt_arr >= 2 then
            self.ConditionProTxt:SetObjActive(true)
            UH.Color(self.ConditionProTxt,data.flag and COLORS.Green10 or COLORS.Red8)
            UH.SetText(self.ConditionProTxt,txt_arr[2])
        else
            self.ConditionProTxt:SetObjActive(false)
        end
    else
        UH.SetText(self.ConditionText, data.condition_desc)
        self.ConditionProTxt:SetObjActive(false)
        self.ConditionProImg:SetObjActive(false)
    end
end

function PetFeiShengConditionItem:OnClickGoto()

    if PetFeiShengData.Instance.feisheng_task_info.pet_unique_id1 ~= 0 and not PetFeiShengData.Instance:IsInTask(PetData.Instance:GetCurSelect()) then
        PublicPopupCtrl.Instance:Error(Language.PetFeiSheng.TaskErrIsInOtherTask)
        return
    end

    if self.data.pre_i == PetFeiShengData.TASK_CONDI_INDEX then --去做前置任务流程
        self.ParentPanel:ExeTask()
    else
        local cfg = PetFeiShengData.GetPreConditionCfg(self.data.pre_i,self.data.fs_lv)
        if cfg ~= nil then
            local func = condition_goto_funcs[cfg.fly_type]
            if func ~= nil then
                func()
                ViewMgr:CloseView(PetFeiSheng)
            end
        end
    end
end
