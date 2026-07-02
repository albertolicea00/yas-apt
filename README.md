# 🐧 Yet Another Store for Apt

> A modern, premium GUI wrapper for the **Advanced Package Tool** (`apt`) package manager.

## 🚀 Why this exists

Let's face it: command line package managers are incredibly powerful, but we all suffer from command amnesia. Who hasn't spent five minutes searching help docs or stackoverflow just to remember the exact syntax to clean the cache, pin a package, or list unneeded dependencies?

**Yet Another Store for Apt** brings all that CLI power into a stunning, native-feeling graphical user interface. No more forgotten commands, no more syntax errors. Just pure, visual package management.

## ✨ Features

- **Visual Package Explorer**: Search, filter, and inspect packages with ease.
- **One-Click Management**: Install, update, pin, and uninstall without opening the terminal.
- **Smart Terminal Output View**: See the exact commands being run under the hood (great for learning or troubleshooting).
- **Command Reminder Companion**: Keep track of history and custom scripts.

## 🔗 Official CLI

This application is an unofficial graphical frontend. It runs standard CLI commands behind the scenes.
- **Official Website/Repository**: [Advanced Package Tool](https://wiki.debian.org/Apt)

## 🛠️ Requirements

- The `apt` command-line tool must be installed on your system and available in your shell's `PATH`.

## 🧑‍💻 Building from source


```bash
git clone https://github.com/albertolicea00/yas-apt.git
cd yas-apt
make run     # configure + build + launch
```

Other targets: `make test` · `make release` · `make clean` · `make help` (full list). Prefer raw CMake? `cmake --preset default && cmake --build --preset default`. Full setup details → [CONTRIBUTING.md](CONTRIBUTING.md).

## 🤝 Contributing

Contributions of all kinds are welcome — bug fixes, UI improvements, theme designs, translation corrections, testing on different platforms.

→ **[CONTRIBUTING.md](CONTRIBUTING.md)** — dev setup, project structure, pull request guidelines

- 🐛 [Report a bug](https://github.com/albertolicea00/yas-apt/issues/new?template=bug_report.md)
- 💡 [Request a feature](https://github.com/albertolicea00/yas-apt/issues/new?template=feature_request.md)

## ⚖️ License & Legal

**Yet Another Store for Apt** is released under the **[MIT License](LICENSE)**.

By using this software you agree to the **[EULA](EULA.md)**. You are solely responsible for using it in compliance with applicable law and the terms of service of any package repository or registry you access. The developers do not endorse piracy or licensing infringement.

Third-party tools (primarily **Advanced Package Tool**) are not bundled and governed by their own licenses.

🔐 Security vulnerabilities → **[SECURITY.md](SECURITY.md)**

