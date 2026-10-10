# 构建与目录

## 目录

- `src/cnc-ddraw/`：完整当前兼容层源码。新增战斗、U、仪表与动画逻辑在 `src/roadrash_mac.c`，暂停音频在 `src/roadrash_audio.c`。
- `src/mac/`：Swift 启动器、当前启动脚本、注册表与显示配置。
- `src/windows/`：Windows 首次初始化、启动脚本和配置。
- `src/patches/`：开场与追击间距策略源码及二进制修改记录。
- `docs/gameplay-module.json`：当前运行时挂接入口和规则。
- `docs/verification/`：历史验证结果快照，详细限制见 `verification.md`。

源码仓库不包含原版游戏素材、Wine 运行库、旧调试程序或个人存档。直接游玩请下载 Releases 中的完整发布包。

## 构建兼容 DLL

需要 GNU make 和面向 i686 Windows 的 MinGW 编译器。已使用 llvm-mingw 20261006 构建当前发布 DLL。

```sh
# 编译器已在 PATH：
bash scripts/build-ddraw.sh

# 或给出 llvm-mingw 的 bin 目录：
RR_TOOLCHAIN_BIN=/path/to/llvm-mingw/bin bash scripts/build-ddraw.sh
```

输出为 `src/cnc-ddraw/ddraw.dll` 和 `build/roadrash-ddraw.map`。修改 `inc/git.h` 中的版本信息会影响二进制哈希，因此重新编译不保证逐字节一致。

仅使用 **32 位 i686** DLL。将 DLL 放进自己下载的 1.9.7 Windows 游戏目录可以替换兼容模块；不要用于其他版本的 EXE。当前 DLL 安装运行时挂接前会校验目标指令，地址依赖本项目的稳定 EXE。

Mac 运行目录有版本迁移标记；手工替换 App 内 DLL 不会自动覆盖已经初始化的运行目录。开发时退出游戏后单独备份并替换运行目录的 DLL，或者实现新的迁移标记，不要删除用户存档。

## Mac 启动器

在 Apple Silicon Mac 上安装 Xcode Command Line Tools：

```sh
mkdir -p build
swiftc -O src/mac/Launcher.swift -framework AppKit -o build/RoadRashMac
```

运行包的 Wine 来自 Gcenx 的 11.0_1 构建。完整 App 还需要原版游戏、图标及运行库；仅编译 Swift 启动器不会生成完整可玩的 App。

## 校验发布包

```sh
python3 scripts/verify-release.py /path/to/archives
```

检查 Windows/Mac ZIP 的 CRC、核心 EXE/DLL 哈希、个人存档与日志排除。运行该检查不会启动游戏。

## 维护约束

保留原版比赛计时、键盘轮询和动画更新入口。不要恢复早期将新增逻辑嵌入错误可执行段的实验补丁。改动涉及原 x86 引擎地址、调用约定和固定警察槽；回归时必须覆盖双方处理顺序、左右两侧、倒地与暂停等状态。
