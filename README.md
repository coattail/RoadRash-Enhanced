# 暴力摩托增强版 · RoadRash Enhanced

经典 Windows《暴力摩托》的兼容与玩法增强项目，当前版本 **1.9.7**。提供 Windows 下载包和 Apple Silicon Mac 应用。

## 下载游玩

**请下载下面的发布包。右上角 Code → Download ZIP 是源码，不能直接启动游戏。**

| 系统 | 下载 | 运行要求与验证情况 |
| --- | --- | --- |
| Windows | [RoadRash-Windows-1.9.7.zip](https://github.com/coattail/RoadRash-Enhanced/releases/download/v1.9.7/RoadRash-Windows-1.9.7.zip) | 面向 Windows 10/11；包完整性已检查，尚未在真实 Windows 电脑实测 |
| Apple Silicon Mac | [RoadRash-Mac-ARM64-1.9.7.zip](https://github.com/coattail/RoadRash-Enhanced/releases/download/v1.9.7/RoadRash-Mac-ARM64-1.9.7.zip) | M 系列芯片、macOS 11 或更新系统、Rosetta；已在开发者的 M1 Mac 游玩，其他系统版本尚未逐一测试 |

[全部版本与 SHA-256 校验文件](https://github.com/coattail/RoadRash-Enhanced/releases)

### Windows

1. 解压整个下载包到可写目录，例如 `D:\Games\RoadRash`，不要直接在压缩包里启动。
2. 首次右键 `Setup.cmd`，选择“以管理员身份运行”。它写入游戏的 32 位注册表配置，会重置已有暴力摩托的按键、音量和显示设置。
3. 之后双击 `Start.cmd`。不需要安装 Wine，也不需要每次重新执行 Setup。
4. 如果配置无法保存，可右键 `Start.cmd` 以管理员身份运行。优先使用自己的游戏目录，避免安装在只读位置。

### Mac

1. 解压下载包，把“暴力摩托 Mac.app”拖入“应用程序”。此应用的启动器仅支持 Apple Silicon，不支持 Intel Mac。
2. 安装 Rosetta；系统出现安装提示时按提示完成。
3. 打开应用。首次建立独立运行环境需要约一分钟。
4. 应用使用本地签名，未经过 Apple 公证。如果系统阻止打开，可在确认来源并核对校验值后，在“系统设置 → 隐私与安全性”中允许这一个应用打开。

游戏通过 Wine 运行原 Windows 程序，使用修改后的 cnc-ddraw 兼容层；不是原生重写。Wine 及运行库已包含在 Mac 发布包内。

## 当前改动

- 对手主动接近和出手，支持减速、低速近身战斗。
- 开场分散起步，左右各限制一名近身追击者，其余车手保持正常超车和绕行，保留转向空间。
- 按攻击键时，优先反击**最近一次向你出手的车手，按时间排序，挥空也计入**；只选择当前可攻击目标，无记录时选择最近可达目标。
- 当前挥击保持方向，连续棍击的下一次挥击重新选方向；修复警察固定车手槽和部分对手状态筛选错误。
- 对攻击过你的目标补偿武器距离；同一帧双方接触时，实际命中的玩家攻击优先结算。没有自动攻击，挥空也不会免伤。
- 玩家近身打斗防御为原版四倍，正伤害及对应附加伤害降为四分之一，小数向上取整，最小伤害 1 保留。任何角色和旧存档均适用；撞车、障碍物伤害及我方输出不变。
- 对手实际打中玩家后有 50% 概率单手庆祝；玩家按 U 可主动庆祝。
- 暂停静音；修复倒地后短暂闪回骑行姿态。
- 窗口可缩放，保持 4:3 比例；修复缩放后的菜单点击坐标，鼠标可以移出窗口。
- 修复原压缩程序启动、倒计时、攻击键及仪表切换问题；Mac 应用使用原版图标。

## 按键

先切换到英文输入法；点击游戏窗口取得焦点。

| 按键 | 功能 |
| --- | --- |
| ↑ / ↓ / ← / → | 加速 / 刹车 / 转向 |
| J | 出拳或使用武器 |
| K | 踢人 |
| 空格 | 反手攻击 |
| U | 单手庆祝；单次触发，正在攻击时等收招后执行 |
| M 或 F10 | 完整仪表 → 迷你／数码仪表 → 隐藏仪表；Mac 可能需要 Fn+F10 |
| 按住 ↑，点按 N | 原版氮气加速，仅支持带氮气的车型 |
| Q / E | 左右倾身 |
| D | 下车 |
| Esc | 暂停；比赛中连续 Esc 返回菜单 |
| Alt+Enter | 窗口／全屏切换（Mac 通常使用 Option+Return） |

Corsair、Diablo、Stiletto、Kamikaze 每场有 10 次氮气。商店超级跑车左上角 Banzai 没有氮气，所以不显示储量红点。隐藏仪表时也不显示红点。

## 存档

在 Restroom 菜单中选择 Save Game As / Load Game。

- Windows：游戏目录中的 `.RRS` 文件。
- Mac：`~/Library/Application Support/RoadRashMac/Prefix/drive_c/RoadRash`，也可通过应用菜单“打开存档目录”访问。

下载包不附带个人存档。迁移进度时复制自己的 `.RRS` 文件。Mac 更新应用保留独立运行目录中的存档。

## 排障与反馈

- Windows 图像异常：退出游戏，将 `ddraw.ini` 的 `renderer=auto` 改为 `renderer=gdi` 后重试。
- Mac 启动错误：通过应用菜单打开运行日志，位置是 `~/Library/Logs/RoadRashMac/game.log`。
- 原素材包未包含的 CD 音乐和视频没有补充。
- 最新玩法模块已做 x86 引擎／DLL 逻辑回归；不是所有功能都经过整场画面测试，也不能保证每台电脑无兼容问题。[验证记录](docs/verification.md)。
- [报告问题](https://github.com/coattail/RoadRash-Enhanced/issues/new)：请注明系统、芯片、版本、地图和复现操作；上传日志前删除个人路径等信息。

## 源码和许可

修改后的完整 cnc-ddraw 源码、Mac 启动器、Windows 启动配置及验证记录在本仓库。[构建说明](docs/build.md)。

项目新增代码采用 MIT；cnc-ddraw 保留上游 MIT 许可，Wine 与其第三方运行库适用各自许可。**原版游戏程序、美术、音频和图标不属于 MIT 许可范围，权利归原权利人所有。** 本仓库不是 EA 官方项目；仅有本项目的开源许可不代表获得原版游戏素材的再分发授权。组件来源与许可见 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。
