This folder contains the different docker files used for building OpenVDS for different distributions

There is also a devenv docker file setting up a minimal developement environment. Following are some basic instructions for setting up the development environment on Windows.

It is important to shut down visual studio, since it will lock some files and building the docker image will not work.

1. Build an image.
From the root OpenVDS source folder write the following command:
$ docker build --tag ovds-alpine-devenv -f docker/alpine-devenv.Dockerfile .
$ docker volume create ovds-alpine-devenv
The volume is used to create a persistant storage so not all is lost between sessions
2. Add configuration to Windows Terminal
Everything should be ready, so now its possible to add a configuration to your WindowsTerminal settings.
```
{
  "guid": "{b453ae62-4e3d-5e58-b989-aaa98ec441b8}",
  "hidden": false,
  "name": "Alpine OpenVDS",
  "commandline": "docker run -it --cap-add=SYS_PTRACE --security-opt seccomp=unconfined -v C:/Projects:/windows_projects_folder -v ovds-alpine-devenv:/work --workdir /work ovds-alpine-devenv"
}
```
Hack away!!!
Its a good idea to clone your OpenVDS repository from your local Windows projects into the work dir for performance reasons.
