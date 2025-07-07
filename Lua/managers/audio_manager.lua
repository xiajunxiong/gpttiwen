AudioManager = AudioManager or BaseClass()
local unityAudioMgr = Game.AudioManager.Singleton()

AudioType = {
    Bg = "bg",
    UI = "ui",
    Scene = "scene",
    Battle = "battle",
    Npc = "npc",
    Story = "story",
}

CommonAudio = {
    ViewOpenL1 = "view_open_l1",--打开界面
    ViewCloseL1 = "view_close_l1",--退出、返回类按钮通用
    ViewOpenL2 = "view_open_l2",--打开二级弹窗
    ClickError = "click_error",--点击按钮条件不足等原因产生飘字时播放
    ClickGeneral1 = "click_tongyong1",
    ClickGeneral2 = "click_tongyong2",
    ClickGeneral3 = "click_tongyong3",
    UpGeneral2 = "tongyong_up2",
    ZhuangBeiFuShuJia = "zhuangbeifushujia",
    ZhuangBeiFuShuJian = "zhuangbeifushujian",
    QiangHuaChengGong = "qianghuachenggong",--强化成功
    QiangHuaShiBai = "qianghuashibai",--强化失败
    QieHuanZhanKai = "qiehuanzhankai",--切换展开
    JinJie1 = "tongyongjinjie01",
    JinJie2 = "tongyongjinjie02",
    MatchSuccess = "match_success",--匹配成功
    QieHuanBiaoQian = "tab_change_l1",--切换标签
    OpenGift = "choose_gift",--打开礼包
    MenuOpen = "menu_open",--菜单切换展开
    MenuClose = "menu_close",--菜单切换收缩
    EquipEquipment = "equip_equipment",--穿戴装备
    BagTidy = "bag_tidy",--整理背包
    BagClose = "bag_close",--关闭背包
    ItemInfoOpen = "item_info_open",--物品信息开启
    ItemInfoClose = "item_info_close",--物品信息关闭
    ToggleIn = "toggle_in",--切换收缩
    ToggleOn = "toggle_on",--切换展开
    SkillLevelUp = "skill_levelup",--技能升级
    ComposeSucc = "compose_succ",--获得特殊物品
    PetGain = "pet_gain",   --激活宠物
    PartnerLockOpen = "partner_lock_open",--获得伙伴
    LoginStartClick = "login_start_click",--开始游戏音效
    GetDiamond = "get_diamond", -- 获得元宝
    MapOpen = "map_open", -- 地图打开
    ItemGet = "item_get", -- 获得物品
    PettEgg2 = "pet_egg2",-- 获得高资质宠物
    ProgressAdd = "progress_add",--进阶读条
    SkillGetNew = "skill_get_new",--获得新技能 
    PetStrength = "pet_strength", --宠物强化,
    TaskFinish = "task_complete",
    FaBaoShengJie = "fabao_shengjie",--法宝升阶
    TreasureHuntOpen = "treasure_hunt_open",--打开寻宝界面
    YinYangEye = "yin_yang_yan",--打开阴阳眼
    WorkshopForging = "workshop_forging",--工坊锻造
    FabaoLianZhiGuangQiu = "fabao_lianzhi_guangqiu",--法宝炼制光球
    XuanZhuanFeiFuShouJi = "XuanZhuanFeiFu1",--旋转飞斧受击
    WoekshopMake = "zy_gongfangzhizhao",--工坊制作
    BaDaoZhan = "BaDaoZhan",--拔刀斩
    LevelUp = "level_up",--角色升级
    Death = "10070501",--死亡
    Door = "door",--：传送门
    RollDice = "roll_dice",--投骰
    ZhuaHuDie = "zhua_hu_die",--抓蝴蝶
    EnterGame = "EnterGame",--进入游戏
    ChuShiDuanWeiXingXing = "chushixingxing",--初始段位-星星
    BoxOpen = "box_open_ui",   --宝箱开启
}

local function getPath(type,name)
    return string.format("audio/%s/%s.asset",type,name)
end

function AudioManager:__init()
	if AudioMgr ~= nil then
		Debuger.LogError("[AudioManager] attempt to create singleton twice!")
		return
	end
	AudioMgr = self
    self.effect_flag = true
    self.bg_flag = true
    -- unityAudioMgr.BgFlag = self.bg_flag
    -- unityAudioMgr.EffectFlag = self.effect_flag
    -- self.cur_bg_name = nil
    self.cur_bg_name = ""
    self.audio_listener = unityAudioMgr.gameObject:AddComponent(typeof(UnityEngine.AudioListener))
    self.cur_listener = self.audio_listener
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().audio_state then
        self.cur_listener.enabled = false
    end
    -- self:AddAudioListener()
end

function AudioManager:__delete()
	if AudioMgr == self then
		AudioMgr = nil
	end
end

function AudioManager:PlayBg(bg_name)
    if self.cur_bg_name == bg_name then return end
    if not StringIsEmpty(bg_name) then
        self.cur_bg_name = bg_name
    else
        bg_name = self.cur_bg_name
    end
    if not self.bg_flag then return end
    unityAudioMgr:PlayBg(getPath(AudioType.Bg,bg_name))
end

function AudioManager:PlayEffect(type,eff_name)
    if not self.effect_flag then return end
    local p = getPath(type,eff_name)
    unityAudioMgr:PlayEffect(p)
end

function AudioManager:PlayEffectByPath(path)
    if not self.effect_flag then return end
    path = string.format("audio/%s.asset",path)
    unityAudioMgr:PlayEffect(path)
end


function AudioManager:PlayEffectHandle(type,eff_name)
    if not self.effect_flag then return end
    local p = getPath(type,eff_name)
    return unityAudioMgr:PlayEffectHandle(p)
end

-- 对话语音分包需要分路径单独一个接口
function AudioManager:PlayEffectNpcHandle(type,eff_name,audio_day)
    if not self.effect_flag then return end
    local p = audio_day > 0 and getPath(string.format("%s/%s", type, tostring(audio_day)), eff_name) or getPath(type,eff_name)
    return unityAudioMgr:PlayEffectHandle(p)
end

function AudioManager:StopEffectHandle(handle)
    if nil == handle then return end
    handle:StopEffect()
end

function AudioManager:StopBg()
    unityAudioMgr:StopBg()
end

function AudioManager:SetBgFlag(flag)
    if self.bg_flag == flag then return end
    self.bg_flag = flag
    unityAudioMgr.BgFlag = flag
    if flag then
        if not StringIsEmpty(self.cur_bg_name) then     --恢复背景音乐
            unityAudioMgr:PlayBg(getPath(AudioType.Bg,self.cur_bg_name))
        end
    end
end

function AudioManager:GetBgFlag()
    return self.bg_flag
end

function AudioManager:SetEffectFlag(flag)
    if self.effect_flag == flag then return end
    self.effect_flag = flag
    unityAudioMgr.EffectFlag = flag
end

function AudioManager:GetEffectFlag()
    return self.effect_flag
end

function AudioManager:SetBgVolume(vol)
    -- if vol <= 0 then vol = 0.1 end --为0的话音乐会重播
    unityAudioMgr.BgVolume = vol
end

function AudioManager:GetBgVolume()
    return unityAudioMgr.BgVolume
end

function AudioManager:SetEffectVolume(vol)
    unityAudioMgr.EffectVolume = vol
end

function AudioManager:GetEffectVolume()
    return unityAudioMgr.EffectVolume
end

--普攻音效获取
function AudioManager:GetAttackName(ch_data)
    if ch_data == nil then return end--容错
    if ch_data.character_type == BATTLE_CH_TYPE.ROLE then
        local weapon_type = ch_data.weapon_type
        local prof = ch_data.appearance.avatar_type--RoleData.Instance:GetRoleProf(ch_data.prof)
        local config = Cfg.AudioLead(prof,weapon_type,false)
        if config then
            return config
        end
    else
        local config = BattleCh.Config(ch_data)
        if config then
            local audio_cfg = Cfg.AudioNLead(config.res_id,false)
            if audio_cfg then
                return audio_cfg
            end
        end
    end
    return nil
end

function AudioManager:GetHitName(ch_data)
    if ch_data == nil then return end--容错
    if ch_data.character_type == BATTLE_CH_TYPE.ROLE then
        local weapon_type = ch_data.weapon_type
        local prof = ch_data.appearance.avatar_type --RoleData.Instance:GetRoleProf(ch_data.prof)
        local config = Cfg.AudioLead(prof,weapon_type,false)
        if config then
            return config.hit
        end
    else
        local config = BattleCh.Config(ch_data)
        if config then
            local audio_cfg = Cfg.AudioNLead(config.res_id,false)
            if audio_cfg then
                return audio_cfg.hit
            end
        end
    end
    return nil
end

function AudioManager:AddAudioListener(obj)
    --#4374687 LuaException
    if self.cur_listener ~= nil and ((self.cur_listener.gameObject == obj) or (obj == nil and self.audio_listener == self.cur_listener)) then
        return
    end
    -- if self.cur_listener ~= nil then
        if self.audio_listener == self.cur_listener then
            self.cur_listener.enabled = false
        else
            UnityGameObj.Destroy(self.cur_listener)
        end
    -- end
    if obj ~= nil then
        self.cur_listener = obj:AddComponent(typeof(UnityEngine.AudioListener))
    else
        self.cur_listener = self.audio_listener
        self.cur_listener.enabled = true
    end
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().audio_state then
        self.cur_listener.enabled = false
    end
end

function AudioManager:OnListenerEnable(enabled)
    if self.cur_listener == self.audio_listener then
        return
    end
    self.cur_listener.enabled = enabled
    self.audio_listener.enabled = not enabled
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().audio_state then
        self.cur_listener.enabled = false
    end
end


function AudioManager:SetSingletonListenerEnable(enable)
    if self.last_enable == enable then
        return
    end
    self.last_enable = self.audio_listener.enabled
    self.audio_listener.enabled = enable
end

function AudioManager:ResetSingletonListenerEnable()
    self.audio_listener.enabled = self.last_enable
end