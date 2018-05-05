# 图形学 粒子效果作业

### 何昊 1600012742

实现了两个粒子效果，一个是类似喷出烟雾的粒子效果，另一个是五毛钱特效的枪口火焰。

按Esc键可以调出GUI在固定位置旋转的相机和鼠标键盘控制的相机之间切换。

实现了一个基本的PBR光照。

## 主要代码

1. `src/ParticleEffects.cpp` 主程序入口
2. `src/Camera.h` 基于四元数实现的简单相机
3. `src/ParticleEmitter.h` `src/ParticleEmitter.cpp` 粒子系统的实现
4. `src/Scene.h` `src/Scene.cpp` 模型的加载与渲染
5. `shaders/ParticleEffects.vert(.frag .geom)`烟雾粒子系统的shader
6. `shaders/PBR.vert(.frag)`光照的shader

## 可执行程序位置

1. Mac OS X: `bin/osx/ParticleEffects`，要用terminal才能启动
2. Windows: `bin/win32/ParticleEffects.exe`

