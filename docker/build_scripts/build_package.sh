set -e -u
base_dir=$(dirname $BASH_SOURCE)

skbuild_platform=""
python_executable=""
openvds_path="$base_dir/../.."
cmake_args="-DBUILD_TESTS=OFF"
openvds_version=""
name="openvds"

cmake_generator=""
platform_name=""
skplat_name=""
distribution=""
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  cmake_generator="Ninja"
  platform_name="linux"
  skplat_name="linux-x86_64"
  toolset=""
  python_executable="python"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cmake_generator="Ninja"
  platform_name="mac"
  skplat_name="maxosx-10.6-x86_64"
  toolset=""
  python_executable="python"
elif [[ "$OSTYPE" == "msys" ]]; then
  cmake_generator="Visual Studio 16 2019"
  platform_name="win"
  skplat_name="win-amd64"
  toolset="-Tv140"
  python_executable="python.exe"
fi
  
while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -s|--skbuildplatform)
    skbuild_platform="$2"
    shift # past argument
    shift # past value
    ;;
    -p|--python)
    python_executable="$2"
    shift # past argument
    shift # past value
    ;;
    -v|--version)
    openvds_version="$2"
    shift # past argument
    shift # past value
    ;;
    -c|--cmake_arg)
    cmake_args="$cmake_args $2"
    shift # past argument
    shift # past value
    ;;
    -d|--distribution)
    distribution="$2"
    shift # past argument
    shift # past value
    ;;
    -n|--name)
    name="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    openvds_path="$1" # save it in an array for later
    shift # past argument
    ;;
esac
done

if [[ "$openvds_version" == "" ]]; then
  openvds_version=$(grep -E "project.*VERSION" "$openvds_path/CMakeLists.txt" | sed 's/[^0-9|.]*//g')
fi
if [[ "$openvds_version" == "" ]]; then
  echo "Please specify OpenVDS version using -v --version"
  exit 1
fi

if [[ "$distribution" == "" ]]; then
    distribution="$platform_name"
fi

python_ver=$("$python_executable" -c "import sys;  print('.'.join(map(str, sys.version_info[:2])))")
python_root_dir=$("$python_executable" -c "import sys; import os; print(os.path.dirname(sys.executable))")

openvds_path=$(realpath $openvds_path)

if [[ "$skbuild_platform" == "" ]]; then
  skbuild_platform="$skplat_name-$python_ver"
fi

skbuild_dir="$openvds_path/_skbuild/$skbuild_platform"
[[ -d $skbuild_dir ]] || mkdir -p $skbuild_dir
skbuild_dir=$(realpath $skbuild_dir)

cd "$skbuild_dir"

[[ -d cmake-build ]] || mkdir cmake-build
cd cmake-build

if [[ "$platform_name" == "win" ]]; then
  cmake -DPython3_ROOT_DIR="$python_root_dir" -DCMAKE_INSTALL_PREFIX=$skbuild_dir/cmake-install $cmake_args -G"$cmake_generator" $toolset $openvds_path
  cmake --build . --config Debug --target install
  cmake --build . --config Release --target install
else
  cmake -DPython3_ROOT_DIR="$python_root_dir" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=$skbuild_dir/cmake-install $cmake_args -G"$cmake_generator" $toolset $openvds_path
  cmake --build . --config Release --target install
fi

cd "$openvds_path"

"$python_executable" setup.py --skip-cmake bdist_wheel 

[[ -d binpackage ]] || mkdir binpackage
cp -r $skbuild_dir/cmake-install binpackage/$name-$openvds_version
cp dist/* binpackage/$name-$openvds_version/
cd binpackage
if [[ "$platform_name" == "win" ]]; then
  zip -r $name-$openvds_version-$distribution.zip $name-$openvds_version
else
  tar -zcvf $name-$openvds_version-$distribution.tar.gz $name-$openvds_version
fi
rm -rf $name-$openvds_version
