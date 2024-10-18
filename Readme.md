这是一个对海康工业相机调用的简单封装。
使用opencv去显示画面。
对GigE协议的相机应该都能使用。
测试使用了海康和大华的相机。
测试平台是 Arch Linux

## 安装
```
sudo pacman -S cmake gcc make opencv
```

## 使用
```
cd build
cmake .. && make && ../bin/./main
```sh
