version="$(wget -q -O - http://sourceforge.net/projects/opencvlibrary/files/opencv-unix | egrep -m1 -o '\"[0-9](\.[0-9])+' | cut -c2-)"
echo "Removing any pre-installed ffmpeg and x264"
sudo apt-get -qq remove ffmpeg x264 libx264-dev
echo "Installing Dependencies"
sudo apt-get -qq install libopencv-dev build-essential checkinstall cmake pkg-config yasm libtiff4-dev libjpeg-dev libjasper-dev libavcodec-dev libavformat-dev libswscale-dev libdc1394-22-dev libxine-dev libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libv4l-dev python-dev python-numpy libtbb-dev libqt4-dev libgtk2.0-dev libfaac-dev libmp3lame-dev libopencore-amrnb-dev libopencore-amrwb-dev libtheora-dev libvorbis-dev libxvidcore-dev x264 v4l-utils ffmpeg
sudo apt-get -qq install libavfilter-dev libboost-dev libboost-program-options-dev libboost-graph-dev python-setuptools python-dev libcppunit-dev sqlite3 libsqlite3-dev cmake-qt-gui libboost-all-dev
sudo easy_install -U mercurial
echo "Installing OpenCV" $version
cd
mkdir OpenCV
cd OpenCV
echo "Downloading OpenCV" $version
wget -O OpenCV-$version.tar.gz http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/$version/opencv-"$version".tar.gz/download
echo "Installing OpenCV" $version
tar -xvf OpenCV-$version.tar.gz
cd opencv-$version
mkdir release
cd release
cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ..
make
sudo make install
echo "OpenCV" $version "ready to be used"

echo "Installing Traffic Intelligence..."
cd
mkdir Research
cd Research
mkdir Code
cd Code
hg clone https://Nicolas@bitbucket.org/trajectories/trajectorymanagementandanalysis
hg clone https://Nicolas@bitbucket.org/Nicolas/trafficintelligence
cd trajectorymanagementandanalysis/trunk/src/TrajectoryManagementAndAnalysis/
cmake .
make TrajectoryManagementAndAnalysis
cd
cd trafficintelligence/c/
make feature-based-tracking
cd

