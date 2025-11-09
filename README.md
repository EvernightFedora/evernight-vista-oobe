<!--
    SPDX-FileCopyrightText: 2025 Kristen McWilliam <kristen@kde.org>
    SPDX-License-Identifier: CC0-1.0
-->


# Evernight Vista OOBE

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



- Clone the repository:

```bash
git clone https://invent.kde.org/evernightfedora/evernight-vista-oobe.git
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

With the systemd service enabled, reboot your system and the initial setup will run automatically.

-----

The project is under active development and is not yet ready for production use.
Contributions and suggestions are very welcome.
