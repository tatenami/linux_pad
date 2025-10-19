# cmake によるライブラリのビルド
echo "\e[32m[SETUP 1/4] Build library \e[0m"
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# ビルドした共有ライブラリとヘッダをシステムにインストール
echo "\e[32m[SETUP 2/4] Install library \e[0m"
sudo make install

# ライブラリを利用できるようにキャッシュを更新
echo "\e[32m[SETUP 3/4] update library cache \e[0m"
sudo ldconfig

# コントローラ用の udev rules をインストール
cd ..
echo "\e[32m[SETUP 4/4] install udev rules for game controller \e[0m"
sudo cp 99-linux_evdev_gamepad.rules /etc/udev/rules.d
sudo udevadm control --reload-rules     
sudo udevadm trigger       
