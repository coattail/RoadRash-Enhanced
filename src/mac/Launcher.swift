import AppKit
import Foundation
final class Launcher: NSObject, NSApplicationDelegate {
 var task: Process?
 var window: NSWindow?
 var timer: Timer?
 let support = FileManager.default.homeDirectoryForCurrentUser.appendingPathComponent("Library/Application Support/RoadRashMac")
 func applicationDidFinishLaunching(_ notification: Notification) {
  let menu = NSMenu()
  let item = NSMenuItem(); menu.addItem(item)
  let submenu = NSMenu(); item.submenu = submenu
  submenu.addItem(withTitle: "打开存档目录", action: #selector(openSaves), keyEquivalent: "")
  submenu.addItem(withTitle: "打开运行日志", action: #selector(openLogs), keyEquivalent: "")
  submenu.addItem(NSMenuItem.separator())
  submenu.addItem(withTitle: "退出暴力摩托", action: #selector(NSApplication.terminate(_:)), keyEquivalent: "q")
  NSApp.mainMenu = menu
  let w = NSWindow(contentRect: NSRect(x: 0, y: 0, width: 420, height: 130), styleMask: [.titled], backing: .buffered, defer: false)
  w.title = "暴力摩托 Mac"
  let label = NSTextField(labelWithString: "正在准备游戏…\n首次启动需要约一分钟，之后可直接进入。")
  label.frame = NSRect(x: 65, y: 42, width: 335, height: 55)
  w.contentView?.addSubview(label)
  let spinner = NSProgressIndicator(frame: NSRect(x: 22, y: 55, width: 25, height: 25))
  spinner.style = .spinning; spinner.startAnimation(nil); w.contentView?.addSubview(spinner)
  w.center(); w.makeKeyAndOrderFront(nil); window = w
  do {
   try FileManager.default.createDirectory(at: support, withIntermediateDirectories: true)
   try Data("preparing\n".utf8).write(to: support.appendingPathComponent("run.state"))
   let p = Process()
   p.executableURL = URL(fileURLWithPath: "/bin/bash")
   p.arguments = [Bundle.main.resourceURL!.appendingPathComponent("launch.sh").path]
   p.terminationHandler = { [weak self] process in DispatchQueue.main.async {
    self?.timer?.invalidate()
    if process.terminationStatus != 0 {
     self?.window?.orderOut(nil)
     let alert = NSAlert(); alert.messageText = "游戏未能启动"
     alert.informativeText = process.terminationStatus == 10 ? "此运行包需要 Rosetta。请先安装 Rosetta，再重新打开。" : "请查看 Library/Logs/RoadRashMac/game.log 中的运行日志。"
     alert.addButton(withTitle: "打开日志"); alert.addButton(withTitle: "关闭")
     if alert.runModal() == .alertFirstButtonReturn { self?.openLogs() }
    }
    NSApp.terminate(nil)
   }}
   task = p; try p.run()
   timer = Timer.scheduledTimer(withTimeInterval: 0.5, repeats: true) { [weak self] _ in
    guard let self = self else { return }
    let state = try? String(contentsOf: self.support.appendingPathComponent("run.state"), encoding: .utf8)
    if state?.trimmingCharacters(in: .whitespacesAndNewlines) == "ready" { self.window?.orderOut(nil); self.timer?.invalidate() }
   }
  } catch {
   let alert = NSAlert(); alert.messageText = "启动失败"; alert.informativeText = error.localizedDescription; alert.runModal(); NSApp.terminate(nil)
  }
 }
 @objc func openSaves() { NSWorkspace.shared.open(support.appendingPathComponent("Prefix/drive_c/RoadRash")) }
 @objc func openLogs() { NSWorkspace.shared.open(FileManager.default.homeDirectoryForCurrentUser.appendingPathComponent("Library/Logs/RoadRashMac")) }
 func applicationShouldTerminate(_ sender: NSApplication) -> NSApplication.TerminateReply {
  if let task = task, task.isRunning { task.terminate() }
  return .terminateNow
 }
}
let app = NSApplication.shared
let delegate = Launcher()
app.delegate = delegate
app.setActivationPolicy(.regular)
app.run()
