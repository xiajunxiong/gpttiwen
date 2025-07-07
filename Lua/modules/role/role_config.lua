MAIN_PLAY_BASE_MOVE_SPEED = 6 -- 主角基础移速

OBJ_BASE_MOVE_SPEED = 6 -- obj基础移速

ROLE_NAME_COUNT = 6 -- 人物名字长度

ROLE_BATTLE_POSITTION = {-- 角色战斗站位
    BEHIND = 0, -- 后排
    FRONT = 1 -- 前排
}
ROLE_LEVEL_CHANGE_TYPE = {
    NORMAL = 0,
    TOP = 1,
}

ROLE_MAX_PROF_STAGE = 6 --转职最高几转

ROLE_MAX_LEVEL = 120 -- 人物最大等级

ProfSettingS = {
    [RoleProfessionType.QiGongShi] = "QiGongShi",
    [RoleProfessionType.JianXiaKe] = "JianXiaKe",
    [RoleProfessionType.KuangZhanShi] = "KuangZhanShi",
    [RoleProfessionType.TieWei] = "TieWei",
    [RoleProfessionType.ShenSheShou] = "ShenSheShou",
    [RoleProfessionType.XingShuShi] = "XingShuShi",
    [RoleProfessionType.YiShuShi] = "YiShuShi",
    [RoleProfessionType.FuZhouShi] = "FuZhouShi"
}

ProfSettingB = {
    [RoleProfessionType.QiGongShi] = "QiGongShi1",
    [RoleProfessionType.JianXiaKe] = "JianXiaKe1",
    [RoleProfessionType.KuangZhanShi] = "KuangZhanShi1",
    [RoleProfessionType.TieWei] = "TieWei1",
    [RoleProfessionType.ShenSheShou] = "ShenSheShou1",
    [RoleProfessionType.XingShuShi] = "XingShuShi1",
    [RoleProfessionType.YiShuShi] = "YiShuShi1",
    [RoleProfessionType.FuZhouShi] = "FuZhouShi1"
}

--角色属性按照属性排
RoleAttributeTypeConfig = {
    "maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu",--基础信息
    "mogong","mokang","bisha","mingzhong","shanduo","fanji","jianren","bisha_dmg",--修正值
    "def_zhongdu","def_shihua","def_hunshui","def_hunluan","def_jiuzui","def_chenmo","def_zhongshang",--抵抗值
    "phy_inc","mag_inc","phy_red","mag_red","xiaozhong","zhenshang", --特殊属性
}
--基础信息
RoleAttributeTypeBaseConfig = {"maxhp","maxmp","gongji","fangyu","minjie","jingshen","huifu"}

RoleScoreKey = {"score_b","score_a","score_s"}

RecvRoleFullKey = {"role_id","name","role_name","level","cur_exp","profession_id","profession","title_id","appearance","facescroe_id","move_speed","create_time", "kill_value", "role_uuid"}--"guild_name"家族名字不走1400协议