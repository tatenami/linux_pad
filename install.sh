# cmake によるライブラリのビルド
printf "\e[32m[SETUP 1/4] Build library \e[0m \n"
mkdir -p build && cd build
cmake ..
make -j$(nproc)

# ビルドした共有ライブラリとヘッダをシステムにインストール
printf "\e[32m[SETUP 2/4] Install library \e[0m \n"
sudo make install

# ライブラリを利用できるようにキャッシュを更新
printf "\e[32m[SETUP 3/4] update library cache \e[0m \n"
sudo ldconfig

# コントローラ用の udev rules をインストール
cd ..
printf "\e[32m[SETUP 4/4] install udev rules for game controller \e[0m \n"
sudo cp 99-linux_evdev_gamepad.rules /etc/udev/rules.d
sudo udevadm control --reload-rules     
sudo udevadm trigger       
