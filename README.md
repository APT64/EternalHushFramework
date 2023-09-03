<div id="header" align="center">
  <h1>
    EternalHush Framework
  </h1>
  <p align="center">
     <img src="https://img.shields.io/github/stars/APT64/EternalHushFramework?label=Stars" alt="Stars">
     <img src="https://img.shields.io/badge/1.0.0.0-red?label=Version" alt="Version">
     <img src="https://img.shields.io/badge/windows-orange?label=Platfom" alt="Platfom">
     <img src="https://img.shields.io/badge/MIT-green?label=License" alt="License">
  </p>
</div>

## About project

EternalHush Framework is a new open source project that is an advanced C&C framework. Designed specifically for Windows operating systems, it allows users to easily extend its functionality by developing plugins in Python. This is achieved through its own Python API.
The EternalHush Framework also features an intuitive and user-friendly interface that simplifies the process of controlling and collecting data from infected operating systems.

<img src="https://i.ibb.co/QbVpBY1/1.png" />

I will continue this project until I get tired of updating it. This project is also being created as a universal platform for my next personal cybersecurity experiments for Windows. It is very important for me to get feedback, so please actively write what you want to add or improve in this project.

The project is now in very early stages of development and is posted so that progress can be tracked.

## Features

- API for Python plugins
- Easy integration of external modules
- Intuitive GUI
- Automatic build script

## Building

```console
$ git clone https://github.com/APT64/EternalHushFramework.git
$ cd EternalHushFramework
$ pip install -r requirements.txt
$ python compile.py build
```

This script will automatically download maven and cmake for building and build the project in the output directory ```build```
```console
$ cd build && java -jar EternalHushFramework.jar
```
Read more in [wiki](https://github.com/APT64/EternalHushFramework/wiki)

## Documentation

[Project wiki](https://github.com/APT64/EternalHushFramework/wiki)

## Developers

- [APT64](https://github.com/APT64)

## License

EternalHush is distributed under the MIT License.
CPython is distributed under the PSF License.
