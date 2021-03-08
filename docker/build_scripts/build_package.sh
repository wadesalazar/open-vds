set -e -u
base_dir=$(dirname $BASH_SOURCE)

skbuild_platform=""
python_executable="python"
openvds_path="$base_dir/../.."
cmake_args="-DBUILD_TESTS=OFF"
openvds_version=""

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

cmake_generator=""
platform_name=""
skplat_name=""
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
  cmake_generator="Ninja"
  platform_name="linux"
  skplat_name="linux-x86_64"
elif [[ "$OSTYPE" == "darwin"* ]]; then
  cmake_generator="Ninja"
  platform_name="mac"
  skplat_name="maxosx-10.6-x86_64"
elif [[ "$OSTYPE" == "msys" ]]; then
  cmake_generator="Visual Studio 14 2015 Win64"
  platform_name="win"
  skplat_name="win-amd64"
fi
python_ver=$($python_executable -c "import sys;  print('.'.join(map(str, sys.version_info[:2])))")

if [[ "$skbuild_platform" == "" ]]; then
  skbuild_platform="$skplat_name-$python_ver"
fi

skbuild_dir="_skbuild/$skbuild_platform"

cd "$openvds_path"

$python_executable setup.py bdist_wheel --build-type Release -G"$cmake_generator" $cmake_args

if [[ "$platform_name" == "win" ]]; then
  cmake --build $skbuild_dir/cmake-build --config Debug --target install
fi

[[ -d binpackage ]] || mkdir binpackage
cp -r $skbuild_dir/cmake-install binpackage/openvds-$openvds_version
cp dist/* binpackage/openvds-$openvds_version/python/
cd binpackage
zip -r openvds-$openvds_version-$platform_name.zip openvds-$openvds_version
rm -rf openvds-$openvds_version
