# Distributed builds based on Gitlab CI

## Table of Contents

**[Introduction](#introduction)**

* [What is this document about?](#what-is-this-document-about)
* [Why are the distributed builds useful?](#why-are-the-distributed-builds-useful)
* [Compilation time as a function of CPU speed](#compilation-time-as-a-function-of-cpu-speed)
* [Dependencies](#dependencies)

**[Setting up runners](#setting-up-runners)**

* [Specific and Shared runners](#specific-and-shared-runners)
* [Installing multi-runner as a Windows service](#installing-multi-runner-as-a-windows-service)

**[Gitlab CI build configuration](#gitlab-ci-build-configuration)**

* [The .gitlab-ci.yml file](#the-gitlab-ci-yml-file)

**[Build scripts](#keil-build-script)**

* [Guidelines for writing build scripts](#guidelines-for-writing-build-scripts)

**[References](#references)**

# Introduction

## What is this document about?

This document has all the required information regarding the distributed builds using Gitlab CI. In this document reader can find all the necessary step by step instructions to complete the task from a scratch. 

## Why are the distributed builds useful?

* Distributed decreases the human error by making builds consistent.
* Distributed build takes a great amount of work from a human. It saves a lot of time.
* Distributed builds takes off a load from user PC and passes that load to a server. 

## Compilation time as a function of CPU speed

Experiment results for `UV4` builds:

* If cross-module optimization is `enabled`, then CPU speed (frequency) is the only factor that makes a difference. The faster the CPU the faster a build is. Number of cores makes no difference.
* If cross module optimization is `disabled` then all cores can run the build using their 100 % capacity. And the faster the cores are the faster a build completes.
* If more than one instance of `UV4` is launched for build then one or more of the instances will crash leaving the build in unknown state. Only timeout or manual cancellation in `Gitlab` can stop the stuck build. 

## Dependencies

In order to run builds the build server must be prepared first. Build server consists of software that allows a specific build to be made and a Gitlab runner.

Full list of dependencies:

* [Gitlab Multi-Runner](https://gitlab.com/gitlab-org/gitlab-ci-multi-runner/blob/master/docs/install/windows.md)
* [Micorosft Visual Studio Community Edition (to run builds for C#)](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx)
* [Keil µVision (to run build for embedded software)](http://www2.keil.com/mdk5/uvision/)

The build server must access applications from command line. This means that those applications must be added to a system path, so application could be called using executable name only (i.e. "UV4").

Path in windows can be set by starting `Run` dialog by pressing `Windows button + R` and entering `control sysdm.cpl,,3`. Then pressing `Environment variables`. Find variable `Path` in `System variables` and press `Edit`. Scroll to the last entry in `Variable value` field. Add semicolon `;` to the end of it and append the required path to the executable (excluding the executable itself).

Example path:

`C:\ProgramData\Oracle\Java\javapath;C:\Windows\system32;C:\Keil_v5\UV4`

# Setting up runners

## Specific and Shared runners

Gitlab CI runners can be shared or specific. Shared runners are shared between projects. This means that the runner is registered server-wide. Only site administrator can register such runner. This runner can be used by every repository that has shared runners option enabled (default option).

Specific runners are runners that run build only for specific projects. This means that other repositories cannot see those runners. Project master can register such runner.

Currently shared runners are used.

Each runner can have `tags`. Those tags identify whatever you want. For example by adding a tag named `keil` you can identify that this runner can only run build for Keil µVision. When you create .gitlab-ci.yml file you can specify that the build can only run on runner which has a tag `keil`. So there might be 100 free runners, but the build will only search for a runner which has a tag `keil`.

## Installing multi-runner as a Windows service

Latest instruction on how to install a multi runner as a Windows service is specified in [official repository's site](https://gitlab.com/gitlab-org/gitlab-ci-multi-runner/blob/master/docs/install/windows.md).

Some tips:

* Do not forget to install the runner as a service (description can be found pressing the link above).
* If there are permission issues when running build, check service permissions. Press `Win + R` and enter `services.msc`. Find the `gitlab-runner` service. Go to `Properties` and then `Log on` and reenter Windows user password.
* Don't leave the runners installation directory open, because Windows takes over the control and the runner cannot complete the build. This is a very important step.
* Coordinator URL is `http://domain/ci`. For example `http://rndprojserver/ci`.
* Gitlab CI token can be found in administration if the runner is shared and in project settings if the runner is specific. Example token: `gtvebWwFuXfa9n-RCc6W`.
* Enter `shell` for executor.

After successfully registering a runner it is mandatory to edit the runner's configuration file `config.toml` which can be found in runner's installation directory.

Keywords that must be editted in `config.toml`:

* `concurrent=1`. This means that the machine where the runner is running on can only handle 1 simultaneous build even if the machine has more than one runner. Now it is important to keep in mind that only 1 simultaneous `Keil` build can be launched, because `Keil` crashes when there are more than one buld launched on different instances of the executable.
* `shell = ...` inside `[[runners]]` tag. This must be set to `powershell`, because all the scripts must be written in `Powershell 4.0` language (`shell = "powershell"`).
* `limit = n` inside `[[runners]]` tag. This sets the number of allowed simultaneous builds on the specific runner. So if concurrent is set to 2, but limit is set to 1 for the specific runner, then this runner will be able to run 1 build at a time.

Let's assume that one build machine has 2 runners - one for `.net` builds and one for embedded applications build. The requirement is to allow one embedded application to be build at the same time, but also there is a requirement to run `.net` build at the same time when the embedded application is being built. To get this working `concurrent` must be set to 2 and `limit` for embedded applications runner must be set to 1. `limit` for `.net` build can be higher.

Runner configuration `config.toml` example:

```
concurrent = 2
check_interval = 0

[[runners]]
  name = "Keil Builder"
  url = "http://rndprojserver.lan/ci"
  token = "61efbed5231cb573c7941a90d1f5b2"
  executor = "shell"
  shell = "powershell"
  limit = 1
  [runners.ssh]
  [runners.docker]
    tls_verify = false
    image = ""
    privileged = false
    disable_cache = false
  [runners.parallels]
    base_name = ""
    disable_snapshots = false
  [runners.virtualbox]
    base_name = ""
    disable_snapshots = false
  [runners.cache]
    Insecure = false

[[runners]]
  name = "MSBuild Builder"
  url = "http://rndprojserver.lan/ci"
  token = "2ed38dc3f95ed51a8a1b71214aa450"
  executor = "shell"
  shell = "powershell"
  limit = 1
  [runners.ssh]
  [runners.docker]
    tls_verify = false
    image = ""
    privileged = false
    disable_cache = false
  [runners.parallels]
    base_name = ""
    disable_snapshots = false
  [runners.virtualbox]
    base_name = ""
    disable_snapshots = false
  [runners.cache]
    Insecure = false
```

# Gitlab CI build configuration

## The .gitlab-ci.yml file

The `.gitlab-ci.yml` file in project `root` is used to describe what a project build/test/deploy is. It consists of build staging, build script running and artifacts archive preparation. `Artifacts` are the build output.

`YAML` file uses a simple spacing syntax. This means that no tabs are allowed in `YAML` files.

Example `.gitlab-ci.yml` file:

```
stages:
    - build
    
develop_build_job:  

  stage: build  
  script:
    - .\Project\BuildControl\CiBuildScripts\build_providing_target_names_file.ps1 -buildtargetnames ".\Project\BuildControl\CiBuildScripts\build_develop_job_target_names.txt"    
  artifacts:
    paths:
    - BuildOutput/*
    name: "${CI_BUILD_ID}_Build_ESIM364_${CI_BUILD_REF_NAME}"    
  only:
    - develop    
  tags:
    - keil
    
release_build_job:

  stage: build  
  script:
    - .\Project\BuildControl\CiBuildScripts\build_providing_target_names_file.ps1 -buildtargetnames ".\Project\BuildControl\CiBuildScripts\build_release_job_target_names.txt"
  artifacts:
    paths:
    - BuildOutput/*
    name: "${CI_BUILD_ID}_Build_ESIM364_${CI_BUILD_REF_NAME}"
  only:
    - /^release-.*$/
  tags:
    - keil

tag_build_job:

  stage: build  
  script:
    - .\Project\BuildControl\CiBuildScripts\build_providing_target_names_file.ps1 -buildtargetnames ".\Project\BuildControl\CiBuildScripts\build_tag_job_target_names.txt"
  artifacts:
    paths:
    - BuildOutput/*
    name: "${CI_BUILD_ID}_Build_ESIM364_${CI_BUILD_REF_NAME}"
  only:
    - tags    
  tags:
    - keil
```

The example configuration has one stage only - `build`. A build is triggered on pushes to `develop`, `tags` and `release-` branches. Artifacts archive name will be constructed of unique build ID, text `Build_ESIM364` and branch name. Each trigger has it's own script launching line - different targets to build count and names. 

Fully working example can be found in [ESIM364](http://rndprojserver/eldes_projects/esim364) repository.

Additional information and other useful tips can also be found in official website [Gitlab CI YAML file (.gitlab-ci.yml)](http://docs.gitlab.com/ce/ci/yaml/README.html).

# Build scripts

Build scripts must be written to conform to `Powershell 4.0`. If you are not sure what version of Powershell environment you have you can check that by launching Powershell console, entering `Get-Host` command and inspecting `Version` field. If you do not have `Powershell 4.0` update it by installing [Windows Management Framework 4.0](https://www.microsoft.com/en-us/download/details.aspx?id=40855).

## Guidelines for writing build scripts

Tips for writing build scripts:

* Gitlab understands if the build was successful by reading the `$lastexitcode` environment variable.
* You can always mark the build as failed by writing `exit 1` or other value than 0.
* Successful exit (build) is the one which is marked by `exit 0`.
* If the build had warning and build program exit code was set to 1, it is useful to grab that value and change it to 0, but informing console that the build had warnings. If you leave exit code 1, then the build will be marked as failed by Gitlab.

Fully working example can be found in [ESIM364](http://rndprojserver/eldes_projects/esim364) repository.

# References

* [ESIM364 Repository](http://rndprojserver/eldes_projects/esim364)
* [Windows Management Framework 4.0](https://www.microsoft.com/en-us/download/details.aspx?id=40855)
*
* [Gitlab Multi-Runner](https://gitlab.com/gitlab-org/gitlab-ci-multi-runner/blob/master/docs/install/windows.md)
* [Micorosft Visual Studio Community Edtiion](https://www.visualstudio.com/en-us/products/visual-studio-community-vs.aspx)
* [Keil µVision](http://www2.keil.com/mdk5/uvision/)
* [Gitlab CI YAML file (.gitlab-ci.yml)](http://docs.gitlab.com/ce/ci/yaml/README.html)
