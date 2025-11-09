<!--
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>
    SPDX-License-Identifier: CC0-1.0
-->


# Evernight Vista OOBE

© 2026 Evernight Vista Team

The Out-of-the-box Experience (OOBE) experience that greets a user after system
installation or when starting up a new computer. Guides the user in creating the
system's first user account and configuring initial settings.It's based on plasma-setup.


## Features

- User account creation
- Language selection
- Keyboard layout selection
- Time zone selection
- Network configuration


## Getting Started

- Binary (Fedora 43):
```bash
sudo dnf copr enable march7thdev/Evernight_Vista_Operating_System -y
```

```bash
sudo dnf install evernight-vista-oobe
```

Caution:Please make sure installed evernight-vista-wallpaper and evernight-vista-themeui!

- Clone the repository:

```bash
git clone https://github.com/EvernightFedora/evernight-vista-oobe.git
```

- Build and install:

```bash
cmake -B build/
cmake --build build/ --parallel
sudo cmake --install build/
```

- Trigger system user creation:

```bash
sudo systemd-sysusers
```

- Enable the systemd service:

```bash
sudo systemctl enable plasma-setup.service
```

- Reboot:

With the systemd service enabled, reboot your system and the OOBE Components will run automatically.

- Contact:

Bilibili:长夜月玩Fedora
Mail:13278297951@sina.cn

-----

The project is under active development and is not yet ready for production use.
Contributions and suggestions are very welcome.
